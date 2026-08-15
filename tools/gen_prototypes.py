#!/usr/bin/env python3
"""
gen_prototypes.py — auto-generate forward declarations for `static` functions.

ปัญหา: ลืมเขียน prototype ของฟังก์ชัน static ไว้บนสุดของไฟล์ทุกที
วิธีแก้: สแกนหา definition ของฟังก์ชัน static ทั้งหมดในไฟล์ แล้วสร้าง/อัปเดต
บล็อก prototype ไว้บนสุดให้อัตโนมัติ (ระหว่างบรรทัด marker) รันซ้ำได้เรื่อย ๆ
โดยไม่ซ้ำซ้อน เพราะจะแทนที่บล็อกเดิมทุกครั้ง

Usage:
    python3 tools/gen_prototypes.py <file.c> [file2.c ...]
    python3 tools/gen_prototypes.py --check <file.c>   # เช็คว่า up-to-date หรือยัง ไม่แก้ไฟล์

ทำงานยังไง:
    1. หา definition รูปแบบ `static <return type> <name>(<args>)\n{`
       (รองรับ signature ที่ขึ้นบรรทัดใหม่ก่อนถึง `{`)
    2. ไม่แตะฟังก์ชันที่ไม่ใช่ static (เช่น main)
    3. แทรก/แทนที่บล็อกระหว่าง marker:
           /* --- BEGIN AUTO PROTOTYPES (tools/gen_prototypes.py) --- */
           ...
           /* --- END AUTO PROTOTYPES --- */
       ถ้ายังไม่มี marker จะแทรกไว้หลังบรรทัด #include สุดท้าย
"""
import re
import sys
from pathlib import Path

BEGIN = "/* --- BEGIN AUTO PROTOTYPES (tools/gen_prototypes.py) --- */"
END = "/* --- END AUTO PROTOTYPES --- */"

# จับ static function definition: static <ret...> name(args) {
FUNC_DEF_RE = re.compile(
    r"^static\b[^;{}]*?\b(\w+)\s*\(([^;{}]*)\)\s*\{",
    re.MULTILINE,
)


def extract_prototypes(source: str):
    protos = []
    for m in FUNC_DEF_RE.finditer(source):
        sig = m.group(0)[:-1].rstrip()  # ตัด '{' ท้ายออก, ตัด whitespace
        sig = re.sub(r"\s+", " ", sig)  # รวม whitespace/newline ให้เป็นบรรทัดเดียว
        protos.append(sig + ";")
    return protos


def build_block(protos):
    if not protos:
        return None
    lines = [BEGIN] + protos + [END]
    return "\n".join(lines) + "\n"


def apply_to_file(path: Path, check_only: bool):
    source = path.read_text(encoding="utf-8")
    protos = extract_prototypes(source)
    block = build_block(protos)

    if block is None:
        print(f"[skip] {path}: ไม่พบฟังก์ชัน static ที่ต้อง declare")
        return False

    marker_re = re.compile(re.escape(BEGIN) + r".*?" + re.escape(END) + r"\n?", re.DOTALL)

    if marker_re.search(source):
        new_source = marker_re.sub(block, source, count=1)
    else:
        # แทรกหลัง #include บรรทัดสุดท้าย (หรือบนสุดไฟล์ถ้าไม่มี #include)
        include_lines = list(re.finditer(r"^#include.*$", source, re.MULTILINE))
        if include_lines:
            insert_at = include_lines[-1].end()
            # ข้ามบรรทัดว่าง/คอมเมนต์ต่อจาก include ไปจนกว่าจะเจอเนื้อหาโค้ด ไม่จำเป็น แค่แทรกต่อท้าย include
            new_source = source[:insert_at] + "\n\n" + block + source[insert_at:]
        else:
            new_source = block + "\n" + source

    changed = new_source != source
    if check_only:
        status = "UP-TO-DATE" if not changed else "NEEDS UPDATE"
        print(f"[check] {path}: {status}")
        return changed

    if changed:
        path.write_text(new_source, encoding="utf-8")
        print(f"[write] {path}: อัปเดต {len(protos)} prototype(s)")
    else:
        print(f"[ok]    {path}: อัปเดตล่าสุดอยู่แล้ว")
    return changed


def main(argv):
    check_only = False
    if argv and argv[0] == "--check":
        check_only = True
        argv = argv[1:]

    if not argv:
        print("usage: gen_prototypes.py [--check] <file.c> [file2.c ...]")
        return 1

    any_changed = False
    for arg in argv:
        p = Path(arg)
        if not p.exists():
            print(f"[error] ไม่พบไฟล์: {p}")
            continue
        any_changed |= apply_to_file(p, check_only)

    if check_only:
        return 1 if any_changed else 0
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

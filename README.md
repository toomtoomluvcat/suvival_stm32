# ชุดซ้อมเขียนโค้ด STM32F411RE (compile-check บนเครื่องคุณเอง)

## วิธีใช้ (ไม่ต้องมีบอร์ดจริง แค่เช็คว่าโค้ด compile ผ่านไหม)

1. ติดตั้ง gcc ถ้ายังไม่มี (Windows ใช้ MinGW/WSL, Mac/Linux มักมีอยู่แล้ว — เช็คด้วย `gcc --version`)
2. เปิด terminal ในโฟลเดอร์นี้ แล้วรัน:
   ```
   gcc -c -Wall -Wextra -std=c99 main.c -o main.o
   ```
3. ถ้าไม่มี error/warning ขึ้น = โค้ด syntax ถูกต้อง (แต่ยังไม่ได้ทดสอบพฤติกรรมฮาร์ดแวร์จริง เพราะไม่มีบอร์ด)
4. แก้โค้ดใน `main.c` แล้ว compile ซ้ำไปเรื่อย ๆ จนกว่าจะผ่าน

> `stm32f4xx.h` ในนี้เป็นไฟล์จำลองที่ทำขึ้นเอง (ไม่ใช่ของจริงจาก ST) แต่ตั้งชื่อ struct/macro (`GPIOA->MODER`, `RCC_AHB1ENR_GPIOAEN` ฯลฯ) ตรงกับของจริงทุกตัว ดังนั้นโค้ดที่ compile ผ่านที่นี่ เอาไปวางใน STM32CubeIDE ตอนสอบจริงได้เลยไม่ต้องแก้ (แค่ตอนสอบจริงให้ include ไฟล์ `stm32f4xx.h` ของจริงจาก CMSIS แทนไฟล์นี้)

## โจทย์ท้าทายที่ให้ลองทำใน main.c

เขียนโปรแกรมเต็ม (setup + main loop) ให้ปุ่มที่ PB3 กดแล้วเลขบน 7-segment เพิ่มขึ้นทีละ 1 วนที่ 0-9 โดยใช้ EXTI

**พิน 7-Segment:**
| บิต | ขา |
|---|---|
| 2^0 | PC7 |
| 2^1 | PA8 |
| 2^2 | PB10 |
| 2^3 | PA9 |

**Hint ลำดับขั้นตอน:**
1. เปิด clock ของ GPIOA, GPIOB, GPIOC, SYSCFG
2. ตั้ง PC7, PA8, PB10, PA9 เป็น output
3. ตั้ง PB3 เป็น input + pull-up
4. ตั้งค่า EXTI line 3 ให้ฟังจาก Port B, trigger falling edge, unmask, enable ใน NVIC (`EXTI3_IRQn` — ต้องเพิ่มใน stm32f4xx.h เอง ถ้ายังไม่มี ดู pattern จาก `EXTI4_IRQn` ที่มีให้แล้ว)
5. เขียน `EXTI3_IRQHandler(void)` : เคลียร์ pending bit, เพิ่มตัวแปร counter (วนที่ 0-9 ด้วย `% 10`), เรียกฟังก์ชันแสดงผลที่ 7-segment

ลองเขียนแล้วส่งกลับมาในแชทได้เลย ผมจะไล่รีวิวให้ (ตรวจ logic + MISRA style) โดยไม่ต้อง compile ในนี้ก็ได้ครับ — คุณ compile เองแล้วบอกผลลัพธ์ error มา เราจะได้แก้ไปด้วยกัน

## โครงสร้างโฟลเดอร์

```
.
├── main.c            ← ไฟล์ที่กำลังซ้อมอยู่ตอนนี้ (compile ตามขั้นตอนด้านบน)
├── stm32f4xx.h        ← header จำลอง ใช้คู่กับ main.c ที่ root
├── build/             ← ไฟล์ที่ compile ออกมาแล้ว (main.o, sim_test) — ลบทิ้งได้เสมอ สร้างใหม่ได้จาก .c
├── sim/               ← test harness สำหรับเช็คคำตอบอัตโนมัติ (ต้องมี stm32f4xx_sim.h/your_code.c เพิ่มเพื่อคอมไพล์ - ไฟล์หายไปตอนนี้)
├── template/          ← โครงไฟล์ตั้งต้นแบบ multi-file (main.c + button_exti.* + seven_seg.* ฯลฯ)
├── reference/          ← เก็บฟังก์ชันสำเร็จรูปไว้ก็อปวางตอนซ้อม (uart.md)
├── doc/               ← รูปประกอบ (pinout, IRQn, ฯลฯ)
├── exam/<YYYY-MM-DD>/  ← โจทย์สอบ/ซ้อมแยกตามวันที่
├── practice/          ← แบบฝึกหัดแยกตามหัวข้อ
│   ├── adc/
│   ├── uart/
│   ├── interupt/
│   ├── pwm/
│   └── seven_segment/
└── tools/
    └── gen_prototypes.py   ← สคริปต์ auto-gen prototype ของฟังก์ชัน static
```

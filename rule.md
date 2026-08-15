# RULE.md — STM32F411RE Coding Exam (Bare-metal, Register-level)

> ไฟล์นี้คือกฎทั้งหมดที่ AI ต้องทำตามเป๊ะ ๆ เวลาช่วยเขียนโค้ดข้อสอบนี้ เป้าหมายคือ
> ได้คะแนนเต็ม 50/50 (Algorithm 25 + Program Structure 15 + MISRA 10) ทุกครั้งที่เขียนโค้ด
> ให้ยึดไฟล์นี้เป็นบรรทัดฐานสูงสุด ห้ามข้ามข้อใดข้อหนึ่ง

---

## 0. บริบทข้อสอบ

- บอร์ด: **STM32F411RE** (Nucleo) + **Training Shield 1 Rev 03.00** (สีแดง — ถ้าได้ shield สีน้ำเงิน ตัวหนังสือบนบอร์ดผิด ให้ยึด mapping ของสีแดงแทน)
- ภาษา: C, bare-metal, เขียน register ตรง ๆ (CMSIS struct pointer เช่น `GPIOA->MODER`) **ห้ามใช้ HAL library**
- เลือกทำ 3 จาก 4 ข้อ: Q1 7-Segment (GPIO, EXTI optional) / Q2 ADC+GPIO / Q3 UART(TX/RX)+ADC+GPIO / Q4 PWM(flexible)+ADC+GPIO
- **AI ใช้ได้ในห้องสอบ แต่กรรมการจะถามหลักการทำงานของโค้ดที่ AI เขียนให้** ดังนั้นโค้ดต้อง (ก) ถูกต้อง (ข) อธิบายได้ง่าย — ให้ AI เขียน comment อธิบาย "ทำไม" กำกับทุกจุดสำคัญเสมอ ไม่ใช่แค่ "ทำอะไร"

---

## 1. Scoring Criteria (คะแนนสะสม ต้องผ่านระดับล่างก่อนถึงจะได้ระดับบน)

### Algorithm development (25 คะแนน)

25=ตรงโจทย์ 100% / 20=80% / 15=60% / 10=40% / 5=20% / 0=ไม่ตรงเลย

### Program Structure (15 คะแนน)

| คะแนน | เกณฑ์                                                                        |
| ----- | ---------------------------------------------------------------------------- |
| 15    | มี template (header comment ตาม format ทางการ)                               |
| 12    | **แยกไฟล์** เหมาะสม เช่น `main.c` + module `.c`/`.h` แยกตามหน้าที่           |
| 9     | ฟังก์ชันแยกดี ตาม One Role = One Function/Single Responsibility + main สะอาด |
| 6     | ฟังก์ชันแยกตามหน้าที่ แต่บางฟังก์ชันยังทำหลายหน้าที่ปนกัน + main สะอาด       |
| 3     | main เริ่มรก ใช้ฟังก์ชันน้อยมาก                                              |
| 0     | โค้ดเกือบทั้งหมดยัดอยู่ใน `int main()`                                       |

**เป้าหมาย: ต้องแยกไฟล์ `.c`/`.h` ตาม module เสมอ (ไม่ใช่ไฟล์เดียว) เพื่อคะแนนเต็ม**

### Software Quality / MISRA C (10 คะแนน)

10=ไม่พลาดกฎ 22 ข้อเลย / 8=พลาด 1-3 ข้อ / 6=พลาด 4-7 ข้อ / 4=พลาด 8-12 ข้อ / 2=พลาดเยอะ+อ่านยาก / 0=ไม่มี comment เลย

**เป้าหมาย: MISRA 0 ข้อพลาด (ดูรายการเต็มข้อ 3)**

---

## 2. Board Pinout (Training Shield 1 Rev 03.00 — สีแดง)

### LED

| ชื่อบนบอร์ด | สี      | ขา MCU  |
| ----------- | ------- | ------- |
| D13         | น้ำเงิน | **PA5** |
| D12         | แดง     | **PA6** |
| D11         | เหลือง  | **PA7** |
| D10         | เขียว   | **PB6** |

### ปุ่มกด (active-low: ไม่กด=1, กด=0 ต้องใช้ pull-up)

| ชื่อบนบอร์ด | ขา MCU   |
| ----------- | -------- |
| D2          | **PA10** |
| D3          | **PB3**  |
| D4          | **PB5**  |
| D5          | **PB4**  |

### 7-Segment (BCD 4 บิต)

| บิต | ขา MCU |
| --- | ------ |
| 2^0 | PC7    |
| 2^1 | PA8    |
| 2^2 | PB10   |
| 2^3 | PA9    |

### Analog input / เซนเซอร์บนบอร์ด (ยืนยันจากรูปวงจรจริง)

| ชื่อบนบอร์ด | อุปกรณ์                      | วัดค่าอะไร       | ขา  | ADC channel |
| ----------- | ---------------------------- | ---------------- | --- | ----------- |
| A0          | NTC Thermistor               | **อุณหภูมิ**     | PA0 | channel 0   |
| A1          | LDR (Photoresistor)          | **ความสว่าง**    | PA1 | channel 1   |
| A2          | Potentiometer (ตัวหมุนสีฟ้า) | ค่าปรับได้ทั่วไป | PA4 | channel 4   |

⚠️ ไม่มี datasheet ระบุ Beta value ของ thermistor หรือค่าความต้านทาน LDR ในเอกสารที่มี —
**ไม่มีสูตรแปลงหน่วยแม่นยำ (°C/lux จริง)** ถ้าโจทย์ต้องการแค่เทียบระดับ (สูง/กลาง/ต่ำ)
ให้ใช้ raw ADC เทียบ threshold ตรง ๆ แบบเดียวกับ Q2 ได้เลย ถ้าต้องการหน่วยจริงต้องถามอาจารย์/ดูเบอร์ชิ้นส่วนบนบอร์ดจริง

หมายเหตุ: บอร์ดมี connector header เปล่าสำหรับ **BH1750** (I2C light sensor) และ **AHT10**
(I2C temp/humidity sensor) ด้วย แต่เป็นโมดูลเสริมแยกต่างหาก **ไม่ใช่เซนเซอร์ built-in ที่ขา A0/A1**
และใช้ I2C ไม่ใช่ ADC — ไม่เกี่ยวกับโจทย์ ADC เว้นแต่โจทย์ระบุให้ต่อโมดูลเสริมจริง

### ADC channel mapping (ทั้งชิป ไม่ใช่แค่บนบอร์ด)

PA0-PA7 → channel 0-7 (เลขขา=เลข channel) / PB0-PB1 → channel 8-9 / PC0-PC5 → channel 10-15

### UART/PWM default AF

USART1: PA9(TX)/PA10(RX) AF7 / USART2: PA2(TX)/PA3(RX) AF7 / TIM2: AF1 / TIM3,TIM4: AF2

---

## 3. MISRA C Rules (22 ข้อ — ห้ามพลาดข้อไหนเลย)

1. ห้ามใช้ comment แบบ `//` ใช้ `/* */` เท่านั้น
2. `{` และ `}` ต้องไม่อยู่บรรทัดเดียวกับ statement (ขึ้นบรรทัดใหม่เสมอ) ยกเว้น `if-else` ที่ `}` คู่กับ `else` บรรทัดเดียวกันได้
3. ต้องมีช่องว่างระหว่าง operand กับ operator เสมอ (`a + b` ไม่ใช่ `a+b`)
4. หนึ่งบรรทัดหนึ่ง assignment เท่านั้น
5. ห้าม magic number ดิบ ๆ ต้องทำ `#define` หรือ `const`
6. หลีกเลี่ยง ternary operator (`? :`) ใช้ if-else แทน
7. ห้ามมี unreachable code (โค้ดหลัง return ที่ไม่มีวันถึง)
8. ห้ามมีตัวแปร/ฟังก์ชันที่ไม่ได้ใช้เลย
9. ห้ามใช้ octal literal (เลขขึ้นต้นด้วย 0 เช่น `012`)
10. ค่าคงที่ที่เป็น unsigned ต้องมี suffix `u`/`U` เสมอ (`5u` ไม่ใช่ `5`)
11. Function ต้องมี prototype แบบเต็มพร้อมชื่อ parameter (`void f(uint8_t x);` ไม่ใช่ `void f();`)
12. ห้ามอ่านค่าตัวแปรก่อนกำหนดค่าเริ่มต้น (initialize ก่อนใช้เสมอ)
13. ห้ามผสม type ไม่เหมาะสม (เช่น boolean กับ arithmetic ปนกัน)
14. ต้องใส่วงเล็บให้ชัดเจนเวลาผสม operator หลายชนิด (`(a + b) - (c * d)` ไม่ใช่ `a + b - c * d`) — สำคัญมากกับ `&`/`==` ปนกัน: `((x & mask) == 0u)` ไม่ใช่ `(x & mask == 0)`
15. ห้ามเทียบ float ด้วย `==`/`!=` ใช้ tolerance เช่น `fabs(a-b) < TOLERANCE`
16. loop counter ห้ามเป็น float
17. เงื่อนไขของ `if`/`while` ต้องเป็น boolean expression จริง (`if (x == 10u)` ไม่ใช่ `if (x)`)
18. body ของ `if`/`while`/`for` ต้องมี `{}` เสมอ แม้มีบรรทัดเดียว
19. ทุก `if...else if` ต้องปิดท้ายด้วย `else` เสมอ (ว่างก็ได้ แต่ใส่ comment `/* No action */`)
20. ทุก `case` ใน `switch` ต้องมี `break` ไม่มีเงื่อนไข (ยกเว้นตั้งใจ fall-through ต้อง comment ระบุ)
21. ทุก `switch` ต้องมี `default`
22. `default` ต้องอยู่ตำแหน่งแรกหรือสุดท้ายของ `switch` เท่านั้น

---

## 4. Program Structure ที่ต้องทำตามเป๊ะ

### Header comment (ทุกไฟล์ .c/.h ต้องมี — ตาม official template)

```c
/*******************************************************************************
 * File Name    : ชื่อไฟล์.c
 * Description  : อธิบายว่าไฟล์นี้ทำอะไร
 * Date         : YYYY-MM-DD
 ******************************************************************************/
```

### โครง .c ต้องมี section comment เรียงตามนี้ (แม้ section ว่างก็ใส่ comment หัวไว้)

```
Includes -> Private includes -> Private typedef/enum/struct/union ->
Private define -> Private macro -> Private constants -> Private variables ->
External variables -> Private function prototypes -> Private user code ->
[main() หรือ Public functions] -> Callback functions -> Private functions
```

### โครง .h ต้องมี

```
#ifndef / #define guard -> Includes -> Exported typedef/enum/struct/union ->
Exported define/macro/constants -> Exported variables -> Exported function prototypes
```

### แยกไฟล์ตาม peripheral (บังคับ เพื่อคะแนนเต็ม)

ตัวอย่าง: `main.c`, `gpio_led.c/.h`, `seven_seg.c/.h`, `button_exti.c/.h`, `adc.c/.h`, `uart.c/.h`, `pwm.c/.h`

### main() ต้องสั้นมาก

```c
int main(void)
{
    clock_init();
    /* เรียก _init() ของทุก module */
    while (1)
    {
        /* เรียกฟังก์ชันสั้น ๆ เท่านั้น ห้ามยัด logic ยาว */
    }
    return 0;
}
```

### ตัวแปร global ข้ามไฟล์ → ใช้ `extern` ใน `.h`, ประกาศจริงใน `.c`

### ฟังก์ชันภายในไฟล์เดียว (ไม่ export) → ต้องมี `static` + prototype ก่อนใช้เสมอ

---

## 5. Register-level Pattern มาตรฐาน (ต้องใช้ pattern นี้ทุกครั้ง)

### Clear-then-set (ใช้ทุกครั้งที่ตั้งค่า field หลายบิต)

```c
GPIOx->MODER &= ~(0x3u << (pin * 2u));   /* เคลียร์ 2 บิตเดิมก่อนเสมอ */
GPIOx->MODER |=  (0x1u << (pin * 2u));   /* ค่าที่เขียน: 00=Input, 01=Output, 10=AltFunc, 11=Analog */
```

⚠️ ถ้าต้องการ Output แล้วดัน OR ด้วย `0x0u` = ไม่ได้เปลี่ยนอะไรเลย (ยังเป็น Input ค้างอยู่) — เช็คทุกครั้งว่าเลขที่ใส่ตรงกับโหมดที่ต้องการจริง

### RCC ต้องเปิด clock ก่อนแตะ register ของ peripheral ใด ๆ เสมอ

```c
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOxEN;   /* GPIO */
RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;    /* ADC, SYSCFG */
RCC->APB1ENR |= RCC_APB1ENR_USART2EN;  /* USART2, TIM2-5 */
```

### GPIO Output

```c
void led_init(void) {
    GPIOx->MODER &= ~(0x3u << (PIN * 2u));
    GPIOx->MODER |=  (0x1u << (PIN * 2u));
}
void led_set(uint8_t state) {
    if (state == 1u) { GPIOx->ODR |= (1u << PIN); }
    else { GPIOx->ODR &= ~(1u << PIN); }
}
```

### GPIO Input (ปุ่ม, polling + edge-detect — แนะนำแทน EXTI เพื่อลดจุดพลาด เพราะ Q1 บอก EXTI optional)

```c
static uint8_t prev_state = 1u;   /* pull-up default = 1 (ไม่กด) */
void button_check(void) {
    uint8_t now_state = (uint8_t)((GPIOx->IDR >> PIN) & 1u);
    if (prev_state == 1u && now_state == 0u) {   /* เก่า=ไม่กด, ใหม่=กด -> เพิ่งกด 1 ครั้ง */
        /* action ที่นี่ */
    } else { /* No action */ }
    prev_state = now_state;
}
```

### EXTI (ถ้าเลือกใช้ interrupt แทน polling)

```
line = เลขขา เสมอ (ไม่ว่าพอร์ตไหน) / index = line÷4 / shift = (line mod 4)×4
Port code: A=0x0, B=0x1, C=0x2
line 0-4  -> handler เดี่ยว: EXTI0_IRQHandler...EXTI4_IRQHandler
line 5-9  -> ใช้ร่วมกัน: EXTI9_5_IRQHandler (ต้องเช็ค PR ว่าบิตไหนติด)
line 10-15-> ใช้ร่วมกัน: EXTI15_10_IRQHandler (ต้องเช็ค PR ว่าบิตไหนติด)
ISR handler: ห้าม static, ชื่อต้องตรง vector table เป๊ะ, ต้องเคลียร์ PR ด้วย |= (เขียน 1 ทับ)
ใช้ FTSR (falling edge) เท่านั้น ไม่เปิด RTSR พร้อมกัน (ไม่งั้น trigger 2 ครั้งต่อการกด)
```

### ADC (channel ต้องตรงกับขาเป๊ะ ทั้ง SMPRx และ SQR3)

```c
void adc_init(void) {
    GPIOx->MODER |= (0x3u << (PIN * 2u));      /* 11 = Analog */
    ADC1->SMPR2 |= ADC_SMPR2_SMPn;              /* n = channel ตรงกับขา */
    ADC1->SQR3  |= (CH << ADC_SQR3_SQ1_Pos);    /* CH ต้องเป็นเลข channel เดียวกับ SMPRx */
    ADC1->CR2   |= ADC_CR2_ADON;
}
uint16_t adc_read(void) {
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while ((ADC1->SR & ADC_SR_EOC) == 0u) { /* รอ */ }
    return (uint16_t)ADC1->DR;
}
```

### UART (TX polling + RX non-blocking, ห้ามใช้ sprintf)

```c
void uart_send_char(uint8_t d) {
    while ((USART2->SR & USART_SR_TXE) == 0u) { /* รอ */ }
    USART2->DR = d;
}
uint8_t uart_check_command(void) {
    uint8_t r = 0u;
    if ((USART2->SR & USART_SR_RXNE) != 0u) { r = (uint8_t)USART2->DR; }
    else { /* No action */ }
    return r;
}
/* PA2=TX, PA3=RX ต้องตั้งทั้งคู่เป็น AF7 (ลืม RX = รับข้อมูลไม่ได้เลย) */
```

### PWM

```c
TIM3->PSC = 15u;              /* 16MHz/(15+1) = 1MHz */
TIM3->ARR = 999u;             /* 1MHz/1000 = 1kHz PWM, ความละเอียด duty 0-999 */
TIM3->CCMR1 |= (0x6u << 4u);  /* 110 = PWM mode 1 */
TIM3->CCMR1 |= TIM_CCMR1_OC1PE;
TIM3->CCER  |= TIM_CCER_CC1E;
TIM3->CR1   |= TIM_CR1_CEN;
/* duty: CCR1 = (ARR * percent) / 100u; */
```

---

## 6. Checklist จุดพลาดที่ห้ามพลาดซ้ำ (เจอจริงตอนซ้อม)

- [ ] Prototype ครบทุกฟังก์ชันก่อน main() (ถ้าเป็น static แล้วไม่มี prototype = compile error ทันที)
- [ ] เรียก `_init()` ของทุก module ใน main() จริง (ไม่ใช่แค่มี prototype เฉย ๆ)
- [ ] RCC เปิด clock ครบทุก peripheral ที่ใช้ (ลืม = โค้ดดูถูกแต่ไม่ทำงานจริงบนบอร์ด)
- [ ] ADC channel (SMPRx) ตรงกับ channel ที่เลือกใน SQR3 เป๊ะ
- [ ] `=` กับ `==` ไม่สลับกัน (`if (x = 0u)` คือบั๊กร้ายแรง ไม่ใช่เปรียบเทียบ)
- [ ] comment `/* ... */` ปิดครบทุกอัน (ไม่ปิด = กลืนโค้ดข้างล่างหายไปเงียบ ๆ)
- [ ] `while(1)` มีอยู่ท้าย main() เสมอ ห้าม return ออกจาก main
- [ ] operator ผสม `&` กับ `==` ต้องมีวงเล็บ: `((x & mask) == 0u)`
- [ ] ตัวแปรเก็บค่า ADC (0-4095) ต้องเป็น `uint16_t` ไม่ใช่ `char`/`uint8_t`
- [ ] literal ทุกตัวมี `u` suffix
- [ ] unsigned ลบกันต้องเช็ค underflow (`if (x==0u) {x=MAX;} else {x=x-1u;}` ไม่ใช่ `(x-1u)%N`)
- [ ] ทุก `if` มี `else` คู่กัน (ว่างก็ใส่ comment)
- [ ] ห้าม `sprintf`/`snprintf` ถ้าโจทย์ห้าม ใช้แปลงเลขเป็น string เอง

---

## 7. คำสั่งสำหรับ AI เวลาช่วยเขียนโค้ดข้อสอบ

1. อ่านโจทย์ แยกลิสต์ขาที่ใช้ทั้งหมดก่อนเขียนโค้ดเสมอ (ดู pinout จากข้อ 2)
2. แยกไฟล์ตาม module เสมอ (main.c + .c/.h อื่น ๆ) ตามข้อ 4
3. ใช้ header comment + section comment ตาม template ข้อ 4 ทุกไฟล์
4. เขียนตาม pattern มาตรฐานข้อ 5 ห้ามมั่ว syntax เอง
5. ตรวจสอบทุกข้อใน MISRA checklist ข้อ 3 ก่อนส่งคำตอบ
6. ตรวจ checklist จุดพลาดข้อ 6 ก่อนส่งคำตอบ
7. ใส่ comment อธิบาย **"ทำไม"** กำกับทุกจุดสำคัญ (ไม่ใช่แค่ "ทำอะไร") เพื่อให้ผู้ใช้เอาไปตอบกรรมการต่อได้ทันที
8. ท้ายคำตอบ สรุป "หลักการทำงาน" ของโค้ดเป็นประโยคสั้น ๆ 3-5 บรรทัด ให้ผู้ใช้จำไปตอบปากเปล่าได้ง่าย

# STM32F411RE — Function Templates (ก็อปวางแล้วแก้เลขขา/ค่าตามโจทย์)

> วิธีใช้: ก็อปฟังก์ชันที่ต้องใช้ไปวาง แล้วแก้ **ตัวเลขที่มี comment กำกับ** ให้ตรงกับขา/ช่องที่โจทย์กำหนด ทุกอันผ่านการ compile-check มาแล้วในช่วงซ้อม

---

## 0. โครง main() มาตรฐาน (ใช้ทุกข้อ)

```c
#include <stdint.h>
#include "stm32f4xx.h"

/* ===== Prototypes ===== */
static void clock_init(void);
/* ... เติม prototype ของฟังก์ชันที่ใช้จริงตรงนี้ ... */

int main(void)
{
    clock_init();
    /* เรียก init function อื่นๆ ตรงนี้ */

    while (1)
    {
        /* เรียกฟังก์ชันสั้นๆ ในนี้ ห้ามยัด logic ยาว */
    }

    return 0;
}
```

---

## 1. GPIO Output (LED)

```c
static void led_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;              /* เปลี่ยน PORT ตามขาจริง */

    GPIOA->MODER &= ~(0x3u << (5u * 2u));              /* เปลี่ยนเลขขา (5) */
    GPIOA->MODER |=  (0x1u << (5u * 2u));               /* 01 = Output */
}

static void led_set(uint8_t state)
{
    if (state == 1u)
    {
        GPIOA->ODR |= (1u << 5u);                       /* เปลี่ยนเลขขา */
    }
    else
    {
        GPIOA->ODR &= ~(1u << 5u);
    }
}

static void led_toggle(void)
{
    GPIOA->ODR ^= (1u << 5u);                            /* เปลี่ยนเลขขา */
}
```

## 2. GPIO Input (ปุ่มกด, polling ไม่ใช้ interrupt)

```c
static void button_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;               /* เปลี่ยน PORT */

    GPIOB->MODER &= ~(0x3u << (3u * 2u));               /* เปลี่ยนเลขขา, 00 = Input */
    GPIOB->PUPDR &= ~(0x3u << (3u * 2u));
    GPIOB->PUPDR |=  (0x1u << (3u * 2u));               /* 01 = Pull-up */
}

static uint8_t button_read(void)
{
    return (uint8_t)((GPIOB->IDR >> 3u) & 1u);          /* เปลี่ยนเลขขา; ปุ่มกด pull-up จะได้ 0 ตอนกด */
}
```

---

## 3. EXTI (Interrupt จากปุ่มกด)

```c
/* ตัวอย่าง: ปุ่มที่ PB3 -> EXTI line 3 */
static void exti_button_init(void)
{
    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB2ENR  |= RCC_APB2ENR_SYSCFGEN;             /* ต้องเปิดเสมอเมื่อใช้ EXTI */

    GPIOB->MODER  &= ~(0x3u << (3u * 2u));              /* Input */
    GPIOB->PUPDR  &= ~(0x3u << (3u * 2u));
    GPIOB->PUPDR  |=  (0x1u << (3u * 2u));              /* Pull-up */

    /* index = line/4 = 3/4 = 0, shift = (line%4)*4 = 12 */
    SYSCFG->EXTICR[0] &= ~(0xFu << 12u);
    SYSCFG->EXTICR[0] |=  (0x1u << 12u);                /* 0x1 = Port B */

    EXTI->FTSR |= EXTI_FTSR_TR3;                        /* falling edge (กดแล้วปล่อย = falling ถ้า pull-up) */
    EXTI->IMR  |= EXTI_IMR_MR3;                         /* unmask */

    NVIC_EnableIRQ(EXTI3_IRQn);
}

void EXTI3_IRQHandler(void)
{
    if ((EXTI->PR & EXTI_PR_PR3) != 0u)
    {
        EXTI->PR |= EXTI_PR_PR3;                        /* เคลียร์ pending โดยเขียน 1 ทับ */
        /* ทำงานตรงนี้ เช่น เรียกฟังก์ชันนับค่า / เปลี่ยนสถานะ */
    }
}
```

**สูตร index/shift ของ EXTICR:** `index = line÷4`, `shift = (line mod 4)×4`
**เลข Port ใน EXTICR:** PA=0x0, PB=0x1, PC=0x2

⚠️ **ขา 0-4 มี handler แยกของตัวเอง** (`EXTI0_IRQHandler` ... `EXTI4_IRQHandler`) แต่ **ขา 5-9 ใช้ร่วมกัน `EXTI9_5_IRQHandler`** และ **ขา 10-15 ใช้ร่วมกัน `EXTI15_10_IRQHandler`** — ถ้าใช้ช่วงนี้ต้องเช็ค `EXTI->PR` ว่าบิตไหนติดก่อนว่าเป็นขาไหนจริง

---

## 4. ADC (Polling, single channel)

```c
static void adc_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    GPIOA->MODER &= ~GPIO_MODER_MODER4;                 /* เปลี่ยนเลขขา (PA4) */
    GPIOA->MODER |= (0x3u << (4u * 2u));                /* 11 = Analog */

    ADC1->SMPR2 |= ADC_SMPR2_SMP4;                      /* channel ต้องตรงกับขา! PA4 = channel 4 */
    ADC1->SQR1  &= ~ADC_SQR1_L;                         /* L=0 แปลว่า 1 channel ใน sequence */
    ADC1->SQR3  &= ~ADC_SQR3_SQ1;
    ADC1->SQR3  |= (4u << ADC_SQR3_SQ1_Pos);            /* SQ1 ต้องตรงกับ channel เดียวกันเป๊ะ */

    ADC1->CR2 |= ADC_CR2_ADON;
}

static uint16_t adc_read(void)
{
    ADC1->CR2 |= ADC_CR2_SWSTART;

    while ((ADC1->SR & ADC_SR_EOC) == 0u)
    {
        /* รอแปลงเสร็จ */
    }

    return (uint16_t)ADC1->DR;
}
```

**ตาราง channel:** PA0-7 = ch0-7, PB0-1 = ch8-9, PC0-5 = ch10-15 (ดูใน cheat sheet)

---

## 5. UART (USART2, Polling TX + Non-blocking RX)

```c
static void uart_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* PA2 = TX */
    GPIOA->MODER  &= ~(0x3u << (2u * 2u));
    GPIOA->MODER  |=  (0x2u << (2u * 2u));              /* 10 = Alt Function */
    GPIOA->AFR[0] &= ~(0xFu << (2u * 4u));
    GPIOA->AFR[0] |=  (7u   << (2u * 4u));              /* AF7 = USART2 */

    /* PA3 = RX */
    GPIOA->MODER  &= ~(0x3u << (3u * 2u));
    GPIOA->MODER  |=  (0x2u << (3u * 2u));
    GPIOA->AFR[0] &= ~(0xFu << (3u * 4u));
    GPIOA->AFR[0] |=  (7u   << (3u * 4u));

    USART2->BRR = 139u;                                 /* 115200 baud @ 16MHz */
    USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE);
    USART2->CR1 |= USART_CR1_UE;
}

static void uart_send_char(uint8_t data)
{
    while ((USART2->SR & USART_SR_TXE) == 0u)
    {
        /* รอ TX ว่าง */
    }
    USART2->DR = data;
}

static void uart_send_string(const char *str)
{
    while (*str != '\0')
    {
        uart_send_char((uint8_t)*str);
        str++;
    }
}

/* เช็ค RX แบบ non-blocking: เรียกใน loop ได้เลย ไม่ค้าง */
static uint8_t uart_check_command(void)
{
    uint8_t received = 0u;

    if ((USART2->SR & USART_SR_RXNE) != 0u)
    {
        received = (uint8_t)USART2->DR;
    }
    else
    {
        /* No action: ยังไม่มีข้อมูลเข้ามา */
    }

    return received;   /* คืนค่า 0 ถ้าไม่มีข้อมูล */
}

/* แปลงตัวเลขเป็น string เอง (ห้ามใช้ sprintf) */
static void uint16_to_string(uint16_t value, char *out)
{
    char temp[6];
    uint8_t idx = 0u;
    uint8_t i;

    if (value == 0u)
    {
        out[0] = '0';
        out[1] = '\0';
    }
    else
    {
        while (value > 0u)
        {
            temp[idx] = (char)('0' + (value % 10u));
            value = value / 10u;
            idx++;
        }
        for (i = 0u; i < idx; i++)
        {
            out[i] = temp[idx - 1u - i];
        }
        out[idx] = '\0';
    }
}
```

---

## 6. PWM (TIM3, Channel 1, ขา PA6)

```c
#define PWM_ARR_VALUE (999u)      /* ความละเอียด duty 0-100% */

static void pwm_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    GPIOA->MODER  &= ~(0x3u << (6u * 2u));              /* เปลี่ยนเลขขา (PA6) */
    GPIOA->MODER  |=  (0x2u << (6u * 2u));              /* Alt Function */
    GPIOA->AFR[0] &= ~(0xFu << (6u * 4u));
    GPIOA->AFR[0] |=  (2u   << (6u * 4u));              /* AF2 = TIM3 */

    TIM3->PSC = 15u;                                    /* 16MHz/(15+1) = 1MHz timer clock */
    TIM3->ARR = PWM_ARR_VALUE;                          /* 1MHz/1000 = 1kHz PWM */
    TIM3->CCR1 = 0u;                                    /* duty เริ่มต้น 0% */

    TIM3->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM3->CCMR1 |= (0x6u << 4u);                        /* 110 = PWM mode 1 */
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;                     /* preload enable */

    TIM3->CCER |= TIM_CCER_CC1E;                        /* enable output channel 1 */
    TIM3->CR1  |= TIM_CR1_CEN;                          /* start timer */
}

static void pwm_set_duty(uint16_t percent)
{
    if (percent <= 100u)
    {
        TIM3->CCR1 = (uint32_t)((PWM_ARR_VALUE * percent) / 100u);
    }
    else
    {
        /* No action: ค่า percent เกิน 100 ไม่ทำอะไร */
    }
}
```

---

## 7. Delay (busy-wait ธรรมดา ไม่ใช้ SysTick)

```c
static void delay_1s(void)
{
    for (uint32_t i = 0u; i < 1333333u; i++)
    {
        /* รอ */
    }
}
```

---

## 8. 7-Segment (แสดงเลข 0-9 จาก 4 ขา)

```c
static void seven_seg_init(void)
{
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    /* ตั้งขาที่ใช้เป็น Output ทั้งหมด — เปลี่ยนตามผังขาจริงของโจทย์ */
    GPIOC->MODER &= ~(0x3u << (7u * 2u));
    GPIOC->MODER |=  (0x1u << (7u * 2u));               /* PC7 = bit 2^0 */

    GPIOA->MODER &= ~(0x3u << (8u * 2u));
    GPIOA->MODER |=  (0x1u << (8u * 2u));               /* PA8 = bit 2^1 */

    GPIOB->MODER &= ~(0x3u << (10u * 2u));
    GPIOB->MODER |=  (0x1u << (10u * 2u));              /* PB10 = bit 2^2 */

    GPIOA->MODER &= ~(0x3u << (9u * 2u));
    GPIOA->MODER |=  (0x1u << (9u * 2u));               /* PA9 = bit 2^3 */
}

static void seven_seg_display(uint8_t number)
{
    uint8_t bit0 = (number >> 0u) & 1u;
    uint8_t bit1 = (number >> 1u) & 1u;
    uint8_t bit2 = (number >> 2u) & 1u;
    uint8_t bit3 = (number >> 3u) & 1u;

    if (bit0 == 1u) { GPIOC->ODR |= (1u << 7u); }  else { GPIOC->ODR &= ~(1u << 7u); }
    if (bit1 == 1u) { GPIOA->ODR |= (1u << 8u); }  else { GPIOA->ODR &= ~(1u << 8u); }
    if (bit2 == 1u) { GPIOB->ODR |= (1u << 10u); } else { GPIOB->ODR &= ~(1u << 10u); }
    if (bit3 == 1u) { GPIOA->ODR |= (1u << 9u); }  else { GPIOA->ODR &= ~(1u << 9u); }
}
```

---

## ⚠️ เช็คก่อนส่งเสมอ

- [ ] Prototype ครบทุกฟังก์ชันก่อน `main()`
- [ ] `RCC` เปิด clock ครบทุก peripheral ที่ใช้จริง (GPIO ทุกพอร์ตที่ใช้ + ADC/USART/TIM/SYSCFG)
- [ ] ตัวเลข channel/AF/pin ในแต่ละฟังก์ชันตรงกับที่โจทย์กำหนดจริง (แก้จาก template แล้วเช็คซ้ำ)
- [ ] `main()` มี `while (1)` และไม่ `return` ออกมา
- [ ] literal ทุกตัวมี `u` suffix, ไม่มี magic number, ทุก if มี else

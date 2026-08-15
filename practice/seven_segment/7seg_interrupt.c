#include <stdint.h>
#include "stm32f4xx.h"

/* ตัวแปร global เก็บเลขปัจจุบันบน 7-segment
 * ต้องเป็น volatile เพราะถูกแก้ค่าใน interrupt (EXTI3_IRQHandler)
 * แต่ก็ถูกอ่าน/ใช้งานนอก interrupt ด้วย ถ้าไม่ใส่ compiler อาจ optimize ผิดพลาด */
volatile uint8_t counter = 0u;

/***********************************************************************
 * @fn      - display_digit
 * @brief   - แปลงเลข 0-9 เป็นบิต BCD 4 บิต แล้วส่งออกไปยังขา 7-segment
 * @param   - number : เลขที่ต้องการแสดง (0-9)
 * @return  - void
 ************************************************************************/
void display_digit(uint8_t number)
{
    /* แยกแต่ละบิตของ number ออกมาด้วยการ shift ขวา แล้ว mask ด้วย & 1 */
    uint8_t bit0 = (number >> 0) & 1u;   /* บิต 2^0 -> PC7 */
    uint8_t bit1 = (number >> 1) & 1u;   /* บิต 2^1 -> PA8 */
    uint8_t bit2 = (number >> 2) & 1u;   /* บิต 2^2 -> PB10 */
    uint8_t bit3 = (number >> 3) & 1u;   /* บิต 2^3 -> PA9 */

    /* ตั้งค่าขาแต่ละขาตามบิตที่แยกออกมา ทีละขาอิสระต่อกัน */
    if (bit0 == 1u)
    {
        GPIOC->ODR |= (1 << 7);
    }
    else
    {
        GPIOC->ODR &= ~(1 << 7);
    }

    if (bit1 == 1u)
    {
        GPIOA->ODR |= (1 << 8);
    }
    else
    {
        GPIOA->ODR &= ~(1 << 8);
    }

    if (bit2 == 1u)
    {
        GPIOB->ODR |= (1 << 10);
    }
    else
    {
        GPIOB->ODR &= ~(1 << 10);
    }

    if (bit3 == 1u)
    {
        GPIOA->ODR |= (1 << 9);
    }
    else
    {
        GPIOA->ODR &= ~(1 << 9);
    }
}

int main(void)
{
    /* --- เปิด clock ให้ทุกพอร์ตที่ใช้ + SYSCFG (จำเป็นสำหรับ EXTI) --- */
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    /* --- ตั้งขา 7-segment ทั้ง 4 ขาเป็น output --- */
    GPIOC->MODER &= ~(0b11 << (7 * 2));
    GPIOC->MODER |=  (0b01 << (7 * 2));   /* PC7 = 2^0 */

    GPIOA->MODER &= ~(0b11 << (8 * 2));
    GPIOA->MODER |=  (0b01 << (8 * 2));   /* PA8 = 2^1 */

    GPIOB->MODER &= ~(0b11 << (10 * 2));
    GPIOB->MODER |=  (0b01 << (10 * 2));  /* PB10 = 2^2 */

    GPIOA->MODER &= ~(0b11 << (9 * 2));
    GPIOA->MODER |=  (0b01 << (9 * 2));   /* PA9 = 2^3 */

    /* --- ตั้ง PB3 เป็น input + pull-up (ปุ่มกด active-low) --- */
    GPIOB->MODER &= ~(0b11 << (3 * 2));   /* 00 = Input */
    GPIOB->PUPDR &= ~(0b11 << (3 * 2));
    GPIOB->PUPDR |=  (0b01 << (3 * 2));   /* Pull-up */

    /* --- ผูก PB3 เข้ากับ EXTI line 3 ผ่าน SYSCFG multiplexer --- */
    /* EXTI line 0-3 อยู่ใน EXTICR[0] (EXTICR1), เส้น 3 อยู่ตำแหน่งบิต [15:12] */
    SYSCFG->EXTICR[0] &= ~(0xFu << 12);
    SYSCFG->EXTICR[0] |=  (0x1u << 12);   /* 0001 = Port B */

    /* --- ตั้ง trigger เป็น falling edge (ปุ่มกด active-low ตกจาก 1->0) --- */
    EXTI->FTSR |= EXTI_FTSR_TR3;
    /* --- unmask (เปิดใช้งาน) EXTI line 3 --- */
    EXTI->IMR  |= EXTI_IMR_MR3;

    /* --- เปิด interrupt ของ EXTI3 ใน NVIC --- */
    NVIC_EnableIRQ(EXTI3_IRQn);

    /* --- แสดงเลข 0 ตอนเริ่มต้นโปรแกรม --- */
    display_digit(0u);

    while (1)
    {
        /* No action: งานทั้งหมดทำใน EXTI3_IRQHandler */
    }

    return 0;
}

/***********************************************************************
 * @fn      - EXTI3_IRQHandler
 * @brief   - เรียกอัตโนมัติเมื่อ PB3 มีการเปลี่ยนสถานะ (falling edge)
 *            เพิ่มค่า counter ทีละ 1 วนที่ 0-9 แล้วอัปเดต 7-segment
 ************************************************************************/
void EXTI3_IRQHandler(void)
{
    /* เช็คว่า interrupt นี้มาจาก EXTI line 3 จริงไหม */
    if ((EXTI->PR & (1 << 3)) != 0u)
    {
        /* ปุ่มกด active-low: IDR bit3 == 0 แปลว่ากดอยู่ */
        if ((GPIOB->IDR & (1 << 3)) == 0u)
        {
            counter = (uint8_t)((counter + 1u) % 10u);   /* เพิ่มค่า วนที่ 0-9 */
            display_digit(counter);
        }
        else
        {
            /* No action: เป็น edge ตอนปล่อยปุ่ม ไม่ต้องทำอะไร */
        }

        EXTI->PR = (1 << 3);   /* เคลียร์ pending bit โดยเขียน 1 ทับ */
    }
    else
    {
        /* No action */
    }
}

void ADC_IRQHandler(void) { }
void USART2_IRQHandler(void) { }
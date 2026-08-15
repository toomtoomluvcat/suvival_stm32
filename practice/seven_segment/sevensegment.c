#include <stdint.h>
#include "stm32f4xx.h"

/* --- ฟังก์ชันแสดงเลข 0-9 บน 7-segment --- */
void display_digit(uint8_t number)
{
    uint8_t bit0 = (number >> 0) & 1u;   /* บิต 2^0 -> PC7 */
    uint8_t bit1 = (number >> 1) & 1u;   /* บิต 2^1 -> PA8 */
    uint8_t bit2 = (number >> 2) & 1u;   /* บิต 2^2 -> PB10 */
    uint8_t bit3 = (number >> 3) & 1u;   /* บิต 2^3 -> PA9 */

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
    /* --- เปิด clock ให้ครบ 3 พอร์ต --- */
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    /* --- ตั้งทั้ง 4 ขาเป็น output --- */
    GPIOC->MODER &= ~(0b11 << (7 * 2));
    GPIOC->MODER |=  (0b01 << (7 * 2));   /* PC7 */

    GPIOA->MODER &= ~(0b11 << (8 * 2));
    GPIOA->MODER |=  (0b01 << (8 * 2));   /* PA8 */

    GPIOB->MODER &= ~(0b11 << (10 * 2));
    GPIOB->MODER |=  (0b01 << (10 * 2));  /* PB10 */

    GPIOA->MODER &= ~(0b11 << (9 * 2));
    GPIOA->MODER |=  (0b01 << (9 * 2));   /* PA9 */

    /* --- ทดสอบ: แสดงเลข 8 ค้างไว้ --- */
    display_digit(8u);

    while (1)
    {
        /* No action */
    }

    return 0;
}

void EXTI4_IRQHandler(void) { }
void ADC_IRQHandler(void) { }
void USART2_IRQHandler(void) { }
/*******************************************************************************
 * File Name    : main.c
 * Description  : Lab 1.3 - General Purpose Input
 *                PA5 LED turns ON while PB4 button is pressed, OFF otherwise.
 *                Hand-written register definitions (Chapter 1.6 of lab1.pdf).
 ******************************************************************************/
#include <stdint.h>

/* ----- Register Definition ----- */
#define RCC_Base        0x40023800
#define RCC_AHB1ENR     (*(volatile uint32_t*)(RCC_Base + 0x30))

#define GPIOA_Base      0x40020000
#define GPIOA_MODER     (*(volatile uint32_t*)(GPIOA_Base + 0x00))
#define GPIOA_OTYPER    (*(volatile uint32_t*)(GPIOA_Base + 0x04))
#define GPIOA_OSPEEDR   (*(volatile uint32_t*)(GPIOA_Base + 0x08))
#define GPIOA_PUPDR     (*(volatile uint32_t*)(GPIOA_Base + 0x0C))
#define GPIOA_IDR       (*(volatile uint32_t*)(GPIOA_Base + 0x10))
#define GPIOA_ODR       (*(volatile uint32_t*)(GPIOA_Base + 0x14))

#define GPIOB_Base      0x40020400   /* GPIOB base address */
#define GPIOB_MODER     (*(volatile uint32_t*)(GPIOB_Base + 0x00))
#define GPIOB_OTYPER    (*(volatile uint32_t*)(GPIOB_Base + 0x04))
#define GPIOB_OSPEEDR   (*(volatile uint32_t*)(GPIOB_Base + 0x08))
#define GPIOB_PUPDR     (*(volatile uint32_t*)(GPIOB_Base + 0x0C))
#define GPIOB_IDR       (*(volatile uint32_t*)(GPIOB_Base + 0x10))
#define GPIOB_ODR       (*(volatile uint32_t*)(GPIOB_Base + 0x14))

int main(void) {
    /* Enable clock for GPIOA (bit0) and GPIOB (bit1) */
    RCC_AHB1ENR |= 0b11;

    /* --- Setup PA5 LED --- */
    GPIOA_MODER   &= ~(0b11 << 10);
    GPIOA_MODER   |=  (0b01 << 10);
    GPIOA_OTYPER  &= ~(0b1  << 5);
    GPIOA_OSPEEDR &= ~(0b11 << 10);

    /* --- Setup PB4 BTN --- */
    GPIOB_MODER &= ~(0b11 << 8);   /* pin4 -> bits 9:8, MODER4 = 00: input */
    GPIOB_PUPDR &= ~(0b11 << 8);
    GPIOB_PUPDR |=  (0b01 << 8);   /* PUPDR4 = 01: pull-up */

    while (1) {
        if ((GPIOB_IDR & (1 << 4)) == 0) {   /* active-low: pressed = 0 */
            GPIOA_ODR |= (1 << 5);
        } else {
            GPIOA_ODR &= ~(1 << 5);
        }
    }
    return 0;
}

/*******************************************************************************
 * File Name    : main.c
 * Description  : Lab 1.1 - General Purpose Output
 *                Turn on the Blue LED (PA5) using hand-written register
 *                definitions 
 ******************************************************************************/
#include <stdint.h>

/* ----- Register Definition ----- */
#define GPIOA_Base      0x40020000
#define GPIOA_MODER     (*(volatile uint32_t*)(GPIOA_Base + 0x00))
#define GPIOA_OTYPER    (*(volatile uint32_t*)(GPIOA_Base + 0x04))
#define GPIOA_OSPEEDR   (*(volatile uint32_t*)(GPIOA_Base + 0x08))
#define GPIOA_ODR       (*(volatile uint32_t*)(GPIOA_Base + 0x14))  /* ODR offset = 0x14 */

int main(void) {
    /* Enable clock for GPIOA (RCC_AHB1ENR bit 0 = GPIOAEN) */
    (*(volatile uint32_t*) 0x40023830) |= 0b1;

    /* --- Setup PA5 LED --- */
    GPIOA_MODER   &= ~(0b11 << 10);   /* clear MODER5 */
    GPIOA_MODER   |=  (0b01 << 10);   /* MODER5 = 01: general purpose output */
    GPIOA_OTYPER &= ~(0b1 << 5);
    GPIOA_OSPEEDR &= ~(0b11 << 10);   /* OSPEEDR5 = 00: low speed */

    /* --- Turn on PA5 LED --- */
    GPIOA_ODR |= (0b1 << 5);          /* ODR5 = 1: HIGH */

    while (1) {
        /* No Action */
    }
    return 0;
}

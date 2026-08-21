/*******************************************************************************
 * File Name    : main.c
 * Description  : Lab 1.2 - Blinking an LED
 *                Toggle the Blue LED (PA5) every ~1 second using a busy-wait
 *                for-loop delay (Chapter 1.3 of lab1.pdf).
 *
 *                DELAY_THRESHOLD derivation:
 *                  - for-loop costs 12 commands / iteration
 *                  - MCU executes 16,000,000 commands / second (16 MHz)
 *                  - threshold = 16,000,000 / 12 = 1,333,333.33 -> 1,333,333
 ******************************************************************************/
#include <stdint.h>

/* ----- Register Definition ----- */
#define GPIOA_Base      0x40020000
#define GPIOA_MODER     (*(volatile uint32_t*)(GPIOA_Base + 0x00))
#define GPIOA_OTYPER    (*(volatile uint32_t*)(GPIOA_Base + 0x04))
#define GPIOA_OSPEEDR   (*(volatile uint32_t*)(GPIOA_Base + 0x08))
#define GPIOA_ODR       (*(volatile uint32_t*)(GPIOA_Base + 0x14))

/* ----- Constant Declaration ----- */
#define DELAY_THRESHOLD (1333333u)   /* 16,000,000 / 12 command/iteration ~= 1 second */

int main(void) {
    /* Enable clock for GPIOA */
    (*(volatile uint32_t*) 0x40023830) |= 0b1;

    /* --- Setup PA5 LED --- */
    GPIOA_MODER   &= ~(0b11 << 10);
    GPIOA_MODER   |=  (0b01 << 10);
    GPIOA_OTYPER  &= ~(0b1  << 5);
    GPIOA_OSPEEDR &= ~(0b11 << 10);

    while (1) {
        /* --- Toggle PA5 LED --- */
        GPIOA_ODR ^= (0b1 << 5);   /* XOR flips ODR5 every pass */

        for (uint32_t iter = 0; iter < DELAY_THRESHOLD; iter++) {
            /* No Action */
        }
    }
    return 0;
}


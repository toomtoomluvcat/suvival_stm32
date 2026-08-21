/*******************************************************************************
 * File Name    : main.c
 * Description  : Lab 1.5 - Polling with Blocking Function
 *                - PB6 LED turns ON while PB4 button is pressed, OFF otherwise.
 *                - PA5 LED blinks at 0.5 Hz (one full on/off cycle = 2s,
 *                  so each state lasts 1s -> toggle once per DELAY_THRESHOLD).
 *                (Chapter 1.11 of lab1.pdf)
 ******************************************************************************/
#include <stdint.h>
#define STM32F411xE
#include "stm32f4xx.h"

/* ----- Constant Declaration ----- */
#define DELAY_THRESHOLD (1333333u)   /* ~1 second busy-wait delay (see Lab 1.2) */

int main(void) {
    /* --- Setup peripheral clock --- */
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN);

    /* --- Setup PA5 LED --- */
    GPIOA->MODER   &= ~GPIO_MODER_MODER5;
    GPIOA->MODER   |= (0b01 << GPIO_MODER_MODER5_Pos);
    GPIOA->OTYPER  &= ~GPIO_OTYPER_OT5;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5;

    /* --- Setup PB6 LED --- */
    GPIOB->MODER   &= ~GPIO_MODER_MODER6;
    GPIOB->MODER   |= (0b01 << GPIO_MODER_MODER6_Pos);
    GPIOB->OTYPER  &= ~GPIO_OTYPER_OT6;
    GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED6;

    /* --- Setup PB4 BTN --- */
    GPIOB->MODER &= ~GPIO_MODER_MODER4;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD4;
    GPIOB->PUPDR |= (0b01 << GPIO_PUPDR_PUPD4_Pos);

    while (1) {
        if ((GPIOB->IDR & GPIO_IDR_ID4) == 0) {
            GPIOB->ODR |= GPIO_ODR_OD6;
        } else {
            GPIOB->ODR &= ~GPIO_ODR_OD6;
        }

        GPIOA->ODR ^= GPIO_ODR_OD5;   /* toggle PA5 LED */

        for (uint32_t iter = 0; iter < DELAY_THRESHOLD; iter++) {
            /* No Action */
        }
    }
    return 0;
}

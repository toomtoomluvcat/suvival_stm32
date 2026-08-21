/*******************************************************************************
 * File Name    : main.c
 * Description  : Lab 1.4 - General Purpose Input with Register Definition File
 *                Same behavior as Lab 1.3 (PA5 LED follows PB4 button) but
 *                using the CMSIS device header + peripheral macros instead
 *                of hand-rolled register definitions (Chapter 1.8).
 ******************************************************************************/
#include <stdint.h>
#define STM32F411xE
#include "stm32f4xx.h"

int main(void) {
    /* --- Setup peripheral clock --- */
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN);

    /* --- Setup PA5 LED --- */
    GPIOA->MODER   &= ~GPIO_MODER_MODER5;
    GPIOA->MODER   |= (0b01 << GPIO_MODER_MODER5_Pos);
    GPIOA->OTYPER  &= ~GPIO_OTYPER_OT5;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5;

    /* --- Setup PB4 BTN --- */
    GPIOB->MODER &= ~GPIO_MODER_MODER4;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD4;
    GPIOB->PUPDR |= (0b01 << GPIO_PUPDR_PUPD4_Pos);

    while (1) {
        if ((GPIOB->IDR & GPIO_IDR_ID4) == 0) {
            GPIOA->ODR |= GPIO_ODR_OD5;
        } else {
            GPIOA->ODR &= ~GPIO_ODR_OD5;
        }
    }
    return 0;
}

/*******************************************************************************
 * File Name    : main.c
 * Description  : Lab 3.3 - Receiving a byte
 *                - Sending '1' from the PC turns the Blue LED (PA5) ON.
 *                - Sending 'G' toggles the Green LED (PB6).
 *                - Any other byte turns the Blue LED back OFF.
 ******************************************************************************/
#include <stdint.h>
#define STM32F411xE
#include "stm32f4xx.h"

/* --- Constant Declaration --- */
#define DELAY_THRESHOLD (1333333u)

/* --- Macro Declaration --- */
#define DELAY_SEC for (uint32_t iter = 0; iter < DELAY_THRESHOLD; iter++)

/* --- Main function --- */
int main(void) {
    /* --- Setup peripheral clock --- */
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN);
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* --- Setup PA5 LED (Blue) --- */
    GPIOA->MODER   &= ~GPIO_MODER_MODER5;
    GPIOA->MODER   |= (0b01 << GPIO_MODER_MODER5_Pos);
    GPIOA->OTYPER  &= ~GPIO_OTYPER_OT5;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5;

    /* --- Setup PB6 LED (Green) --- */
    GPIOB->MODER   &= ~GPIO_MODER_MODER6;
    GPIOB->MODER   |= (0b01 << GPIO_MODER_MODER6_Pos);
    GPIOB->OTYPER  &= ~GPIO_OTYPER_OT6;
    GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED6;

    /* --- Setup USART2 Rx Pin (PA3 = AF07) --- */
    GPIOA->MODER &= ~GPIO_MODER_MODER3;
    GPIOA->MODER |= (0b10 << GPIO_MODER_MODER3_Pos);
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT3;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED3;
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFRL3;
    GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL3_Pos);

    /* --- Setup USART 2 --- */
    USART2->CR1 |= USART_CR1_UE;
    USART2->CR1 &= ~USART_CR1_M;
    USART2->CR2 &= ~USART_CR2_STOP;
    USART2->BRR = 139;                          /* round(16,000,000 / 115200) = 139 */
    USART2->CR1 |= USART_CR1_RE;                /* enable receiver, start searching for start bit */

    while (1) {
        if ((USART2->SR & USART_SR_RXNE) != 0) {
            if (USART2->DR == '1') {
                GPIOA->ODR |= GPIO_ODR_OD5;
            } else if (USART2->DR == 'G') {
                GPIOB->ODR ^= GPIO_ODR_OD6;
            } else {
                GPIOA->ODR &= ~GPIO_ODR_OD5;
            }
        }
        DELAY_SEC;
    }
    return 0;
}

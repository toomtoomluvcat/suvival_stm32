/*******************************************************************************
 * File Name    : main.c
 * Description  : Lab3.1B - GPIO Alternate Function
 *                Adds Alternate Function routing so PA2 actually drives
 *                USART2_TX (AF07, per Table 9: Alternate function mapping).
 *                NOTE: clock enables (GPIOA/USART2) are only added in
 *                Lab3.1C. See lab_3.1C/main.c for the complete program.
 ******************************************************************************/
#include <stdint.h>
#define STM32F411xE
#include "stm32f4xx.h"

int main(void) {
    /* --- Setup PA2 to AF07 (USART2 Tx) --- */
    GPIOA->MODER   &= ~GPIO_MODER_MODER2;
    GPIOA->MODER   |= (0b10 << GPIO_MODER_MODER2_Pos);  /* 10: Alternate function mode */
    GPIOA->OTYPER  &= ~GPIO_OTYPER_OT2;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED2;
    GPIOA->AFR[0]  &= ~GPIO_AFRL_AFRL2;
    GPIOA->AFR[0]  |= (7 << GPIO_AFRL_AFSEL2_Pos);      /* AF7 = USART2_TX on PA2 */

    /* --- Setup USART 2 --- */
    USART2->CR1 |= USART_CR1_UE;
    USART2->CR1 &= ~USART_CR1_M;
    USART2->CR2 &= ~USART_CR2_STOP;
    USART2->BRR = 139;                          /* round(16e6 / 115200) = 139 */
    USART2->CR1 |= USART_CR1_TE;

    USART2->DR = 'A';

    while (1) {

    }
    return 0;
}

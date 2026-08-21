/*******************************************************************************
 * File Name    : main.c
 * Description  : Lab3.1C - Transmitting a byte (Assignment)
 *                Completes Lab3.1A/B into a fully working program: enables
 *                GPIOA + USART2 clocks, and re-transmits the ASCII byte
 *                (75 = 'K') once every ~1 second over USART2 @ 115200 bps.
 ******************************************************************************/
#include <stdint.h>
#define STM32F411xE
#include "stm32f4xx.h"

/* --- Constant Declaration --- */
#define DELAY_THRESHOLD (1333333u)  /* 16,000,000 / 12 command/iteration ~= 1 second */
#define UART_DATA_ASCII (75U)       /* ASCII 75 = 'K' */

/* --- Macro Declaration --- */
#define DELAY_SEC for (uint32_t iter = 0; iter < DELAY_THRESHOLD; iter++)

/* --- Main function --- */
int main(void) {
    /* --- Setup Peripheral Clock --- */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;        /* GPIOA clock (needed for PA2) */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;       /* USART2 clock */

    /* --- Setup PA2 to AF07 (USART2 Tx) --- */
    GPIOA->MODER   &= ~GPIO_MODER_MODER2;
    GPIOA->MODER   |= (0b10 << GPIO_MODER_MODER2_Pos);  /* Alternate function mode */
    GPIOA->OTYPER  &= ~GPIO_OTYPER_OT2;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED2;
    GPIOA->AFR[0]  &= ~GPIO_AFRL_AFRL2;
    GPIOA->AFR[0]  |= (7 << GPIO_AFRL_AFSEL2_Pos);      /* AF7 = USART2_TX */

    /* --- Setup USART2 --- */
    USART2->CR1 |= USART_CR1_UE;
    USART2->CR1 &= ~USART_CR1_M;
    USART2->CR2 &= ~USART_CR2_STOP;
    USART2->BRR = 139;                          /* round(16,000,000 / 115200) = 139 */
    USART2->CR1 |= USART_CR1_TE;

    while (1) {
        USART2->DR = UART_DATA_ASCII;
        DELAY_SEC;
    }
    return 0;
}

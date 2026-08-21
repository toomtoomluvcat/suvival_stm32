/*******************************************************************************
 * File Name    : main.c
 * Description  : Lab 3.4 - Byte Echo
 *                - Any byte 'x' received from the PC is echoed back as
 *                  "You've entered: x\n\r" (built with sprintf, sent with
 *                  UART2_TxString).
 *                - Sending '1' turns the Green LED (PB6) ON, anything else
 *                  turns it OFF.
 ******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#define STM32F411xE
#include "stm32f4xx.h"

/* ----- Variable Declaration -----*/
char stringOut[50];

/* ----- Static Function Prototypes -----*/
static void UART2_TxString(char strOut[]);

/* --- Main function --- */
int main(void) {
    /* --- Setup peripheral clock ---*/
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN);
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* --- Setup PB6 LED ---*/
    GPIOB->MODER   &= ~GPIO_MODER_MODER6;
    GPIOB->MODER   |= (0b01 << GPIO_MODER_MODER6_Pos);
    GPIOB->OTYPER  &= ~GPIO_OTYPER_OT6;
    GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED6;

    /* --- Setup USART2 Tx/Rx Pin --- */
    GPIOA->MODER   &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
    GPIOA->MODER   |= (0b10 << GPIO_MODER_MODER2_Pos) |
                      (0b10 << GPIO_MODER_MODER3_Pos);
    GPIOA->OTYPER  &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);
    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3);
    GPIOA->AFR[0]  &= ~(GPIO_AFRL_AFRL2 | GPIO_AFRL_AFRL3);
    GPIOA->AFR[0]  |= (7 << GPIO_AFRL_AFSEL2_Pos) |
                      (7 << GPIO_AFRL_AFSEL3_Pos);

    /* --- Setup USART2 --- */
    USART2->CR1 |= USART_CR1_UE;
    USART2->CR1 &= ~USART_CR1_M;
    USART2->CR2 &= ~USART_CR2_STOP;
    USART2->BRR = 139;                          /* round(16,000,000 / 115200) = 139 */
    USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE);

    while (1) {
        if ((USART2->SR & USART_SR_RXNE) != 0) {

            /* --- Read received byte ---*/
            char received = (char)USART2->DR;

            /* --- Control PB6 LED ---*/
            if (received == '1') {
                GPIOB->ODR |= GPIO_ODR_OD6;
            } else {
                GPIOB->ODR &= ~GPIO_ODR_OD6;
            }

            /* --- Echo received byte back to PC ---*/
            sprintf(stringOut, "You've entered: %c\n\r", received);
            UART2_TxString(stringOut);
        }
    }
    return 0;
}

/* ----- Private Function -----*/
static void UART2_TxString(char strOut[]) {
    for (uint8_t idx = 0; strOut[idx] != '\0'; idx++) {
        while ((USART2->SR & USART_SR_TXE) == 0);
        USART2->DR = strOut[idx];
    }
}

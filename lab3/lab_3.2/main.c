/*******************************************************************************
 * File Name    : main.c
 * Description  : Lab 3.2 - Transmitting a String
 *                Transmits a null-terminated string ("Hello world!\n\r")
 *                over USART2 @ 115200 bps every ~1 second. TXE is polled
 *                before every byte write so the DR register is never
 *                overwritten before the previous byte has moved out.
 ******************************************************************************/
#include <stdint.h>
#define STM32F411xE
#include "stm32f4xx.h"

/* --- Constant Declaration --- */
#define DELAY_THRESHOLD (1333333u)  /* ~1 second busy-wait delay (see Lab 1.2) */

/* --- Macro Declaration --- */
#define DELAY_SEC for (uint32_t iter = 0; iter < DELAY_THRESHOLD; iter++)

/* ----- Variable Declaration -----*/
char stringOut[50] = "Hello world!\n\r";

/* ----- Static Function Prototypes -----*/
static void UART2_TxString(char strOut[]);

/* --- Main function --- */
int main(void) {
    /* --- Setup Peripheral Clock --- */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* --- Setup PA2 to AF07 (USART2 Tx) --- */
    GPIOA->MODER   &= ~GPIO_MODER_MODER2;
    GPIOA->MODER   |= (0b10 << GPIO_MODER_MODER2_Pos);
    GPIOA->OTYPER  &= ~GPIO_OTYPER_OT2;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED2;
    GPIOA->AFR[0]  &= ~GPIO_AFRL_AFRL2;
    GPIOA->AFR[0]  |= (7 << GPIO_AFRL_AFSEL2_Pos);

    /* --- Setup USART2 --- */
    USART2->CR1 |= USART_CR1_UE;
    USART2->CR1 &= ~USART_CR1_M;
    USART2->CR2 &= ~USART_CR2_STOP;
    USART2->BRR = 139;                          /* round(16,000,000 / 115200) = 139 */
    USART2->CR1 |= USART_CR1_TE;

    while (1) {
        UART2_TxString(stringOut);
        DELAY_SEC;
    }
    return 0;
}

/* ----- Private Function -----*/
static void UART2_TxString(char strOut[]) {
    for (uint8_t idx = 0; strOut[idx] != '\0'; idx++) {
        while ((USART2->SR & USART_SR_TXE) == 0);  /* wait until TXE=1 (DR free) */
        USART2->DR = strOut[idx];
    }
}

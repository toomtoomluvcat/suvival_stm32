/*******************************************************************************
 * File Name    : main.c
 * Description  : Lab3.1A - Transmitting a byte over UART
 *                Follows reference manual section 19.3 procedure steps 1-7
 *                (word length, stop bit, baud rate, TE, single DR write).
 *                NOTE: this is a teaching snapshot, not the final working
 *                program - GPIO alternate-function routing (PA2->USART2_TX)
 *                is only added in Lab3.1B, and clock enables are only added
 *                in Lab3.1C. See lab_3.1C/main.c for the complete program.
 ******************************************************************************/
#include <stdint.h>
#define STM32F411xE
#include "stm32f4xx.h"

int main(void) {
    /* --- Setup USART 2 --- */
    USART2->CR1 |= USART_CR1_UE;               /* Step 1: enable USART */
    USART2->CR1 &= ~USART_CR1_M;                /* Step 2: 8 data bits (M = 0) */
    USART2->CR2 &= ~USART_CR2_STOP;             /* Step 3: 1 stop bit (STOP = 00) */
    /* Step 4 skipped: no DMA / multi-buffer communication in this lab */
    USART2->BRR = 139;                          /* Step 5: BRR = round(16e6 / 115200) = 139 */
    USART2->CR1 |= USART_CR1_TE;                /* Step 6: enable transmitter */

    USART2->DR = 'A';                           /* Step 7: write data to send */
    /* Step 8 skipped: USART2 is left on permanently in this lab */

    while (1) {

    }
    return 0;
}

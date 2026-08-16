/*******************************************************************************
 * File Name    : uart.h
 * Description  : Header สำหรับโมดูลรับ-ส่งข้อมูลผ่าน USART2 (PA2=TX, PA3=RX)
 * Date         : 2026-08-16
 ******************************************************************************/
#ifndef UART_H
#define UART_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported typedef/enum/struct/union -----------------------------------------*/

/* Exported define/macro/constants --------------------------------------------*/

/* Exported variables -----------------------------------------------------------*/

/* Exported function prototypes ------------------------------------------------*/
void uart_init(void);
void uart_send_char(uint8_t data);
void uart_send_string(const char * str);
void uart_send_number(uint32_t number);
uint8_t uart_receive_char(void);

#endif /* UART_H */

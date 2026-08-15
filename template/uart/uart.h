#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_init(void);
void uart_send_char(uint8_t data);
void uart_send_string(const char *str);
uint8_t uart_check_command(void);
void uint16_to_string(uint16_t value, char *out);

#endif /* UART_H */
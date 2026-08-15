#include "stm32f4xx.h"
#include "uart.h"

void uart_init(void)
{
    /* PA2 = TX */
    GPIOA->MODER  &= ~(0x3u << (2u * 2u));
    GPIOA->MODER  |=  (0x2u << (2u * 2u));
    GPIOA->AFR[0] &= ~(0xFu << (2u * 4u));
    GPIOA->AFR[0] |=  (7u   << (2u * 4u));

    /* PA3 = RX */
    GPIOA->MODER  &= ~(0x3u << (3u * 2u));
    GPIOA->MODER  |=  (0x2u << (3u * 2u));
    GPIOA->AFR[0] &= ~(0xFu << (3u * 4u));
    GPIOA->AFR[0] |=  (7u   << (3u * 4u));

    USART2->BRR = 139u;
    USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE);
    USART2->CR1 |= USART_CR1_UE;
}

void uart_send_char(uint8_t data)
{
    while ((USART2->SR & USART_SR_TXE) == 0u)
    {
        /* รอ TX ว่าง */
    }
    USART2->DR = data;
}

void uart_send_string(const char *str)
{
    while (*str != '\0')
    {
        uart_send_char((uint8_t)*str);
        str++;
    }
}

uint8_t uart_check_command(void)
{
    uint8_t received = 0u;

    if ((USART2->SR & USART_SR_RXNE) != 0u)
    {
        received = (uint8_t)USART2->DR;
    }
    else
    {
        /* No action: ยังไม่มีข้อมูลเข้ามา */
    }

    return received;
}

void uint16_to_string(uint16_t value, char *out)
{
    char temp[6];
    uint8_t idx = 0u;
    uint8_t i;

    if (value == 0u)
    {
        out[0] = '0';
        out[1] = '\0';
    }
    else
    {
        while (value > 0u)
        {
            temp[idx] = (char)('0' + (value % 10u));
            value = value / 10u;
            idx++;
        }
        for (i = 0u; i < idx; i++)
        {
            out[i] = temp[idx - 1u - i];
        }
        out[idx] = '\0';
    }
}
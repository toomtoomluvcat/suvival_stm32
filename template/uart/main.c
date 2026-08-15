#include "stm32f4xx.h"
#include "adc.h"
#include "uart.h"
#include "led_toggle.h"

static void clock_init(void);
static void delay_1s(void);

int main(void)
{
    clock_init();
    led_init();
    adc_init();
    uart_init();

    while (1)
    {
        uint16_t adc_value = adc_read();
        char num_str[6];
        char msg[20];
        uint8_t i;
        uint8_t j = 0u;
        const char *prefix = "ADC = ";
        uint8_t received;

        uint16_to_string(adc_value, num_str);

        for (i = 0u; prefix[i] != '\0'; i++) { msg[j] = prefix[i]; j++; }
        for (i = 0u; num_str[i] != '\0'; i++) { msg[j] = num_str[i]; j++; }
        msg[j] = '\r'; j++;
        msg[j] = '\n'; j++;
        msg[j] = '\0';

        uart_send_string(msg);

        received = uart_check_command();
        if (received == (uint8_t)'1')
        {
            led_toggle();
        }
        else
        {
            /* No action */
        }

        delay_1s();
    }

    return 0;
}

static void clock_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}

static void delay_1s(void)
{
    for (uint32_t i = 0u; i < 1333333u; i++)
    {
        /* รอ */
    }
}
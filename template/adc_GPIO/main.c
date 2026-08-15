#include "stm32f4xx.h"
#include "adc.h"
#include "led_threshold.h"

#define MIN_ADC_THRESHOLD (4095u * 1u / 3u)
#define MAX_ADC_THRESHOLD (4095u * 2u / 3u)

static void clock_init(void);

int main(void)
{
    clock_init();
    led_init();
    adc_init();

    while (1)
    {
        uint16_t adc_value = adc_read();

        if (adc_value < MIN_ADC_THRESHOLD)
        {
            led_set(0u, 1u);
            led_set(1u, 0u);
        }
        else if (adc_value > MAX_ADC_THRESHOLD)
        {
            led_set(0u, 0u);
            led_set(1u, 1u);
        }
        else
        {
            led_set(0u, 1u);
            led_set(1u, 1u);
        }
    }

    return 0;
}

static void clock_init(void)
{
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN);
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}
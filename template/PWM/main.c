#include "stm32f4xx.h"
#include "adc.h"
#include "pwm.h"

static void clock_init(void);

int main(void)
{
    clock_init();
    adc_init();
    pwm_init();

    while (1)
    {
        uint16_t adc_value = adc_read();
        uint16_t duty_percent = (uint16_t)((adc_value * 100u) / 4095u);
        pwm_set_duty(duty_percent);
    }

    return 0;
}

static void clock_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
}
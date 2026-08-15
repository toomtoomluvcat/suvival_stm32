#include "stm32f4xx.h"
#include "led_control.h"


#define MIN_ADC_THRESHOLD (4095u * 1u / 3u)
#define MAX_ADC_THRESHOLD (4095u * 2u / 3u)

static void clock_init(void);

int main(void)
{
    clock_init();
    led_init();
    while(1)
    {
        
    }
}

static void clock_init(void)
{
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN);
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}
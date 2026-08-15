#include "stm32f4xx.h"
#include "led_threshold.h"

void led_init(void)
{
    GPIOB->MODER &= ~(0x3u << (0u * 2u));
    GPIOB->MODER |=  (0x1u << (0u * 2u));       /* PB0 = เขียว */

    GPIOB->MODER &= ~(0x3u << (1u * 2u));
    GPIOB->MODER |=  (0x1u << (1u * 2u));       /* PB1 = แดง */
}

void led_set(uint8_t pin, uint8_t state)
{
    if (state == 1u)
    {
        GPIOB->ODR |= (1u << pin);
    }
    else
    {
        GPIOB->ODR &= ~(1u << pin);
    }
}
#include "stm32f4xx.h"
#include "led_toggle.h"

void led_init(void)
{
    GPIOA->MODER &= ~(0x3u << (5u * 2u));
    GPIOA->MODER |=  (0x1u << (5u * 2u));   /* PA5 */
}

void led_toggle(void)
{
    GPIOA->ODR ^= (1u << 5u);
}
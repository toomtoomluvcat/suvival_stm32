#include "stm32f4xx.h"
#include "led_toggle.h"

void led_init(void)
{

    /*น้ำเงิน*/
    GPIOA->MODER &= ~(0x3u << (5u * 2u));
    GPIOA->MODER |=  (0x1u << (5u * 2u));    
    
    /*แดง*/
    GPIOA->MODER &= ~(0x3u << (6u * 2u));
    GPIOA->MODER |=  (0x1u << (6u * 2u));    
    
    /*เหลือง*/
    GPIOA->MODER &= ~(0x3u << (7u * 2u));
    GPIOA->MODER |=  (0x1u << (7u * 2u));    
    
    /*เขียว*/
    GPIOB->MODER &= ~(0x3u << (6u * 2u));
    GPIOB->MODER |=  (0x1u << (6u * 2u));    
    
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
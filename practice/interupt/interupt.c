/*โปรแกรมเปิดปิดไฟล์แบบ Inputerupt*/
#include <stdint.h>
#include "stm32f4xx.h"

int main(void)
{
    /*เปิด clock*/
    RCC->AHB1ENR |=( RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    /*ตั้ง PA5 เป็น output*/
    GPIOA->MODER &= ~(0b11 << (5 * 2));
    GPIOA->MODER |=  (0b01 << (5 * 2)); 

    /*ตั้ง GPIOB -> เป็น input*/
    GPIOB -> MODER &= ~(0b11 << (4*2));
    GPIOB -> PUPDR &= ~(0b11 << (4*2));
    GPIOB -> PUPDR |= (0b01 << (4*2));

    SYSCFG->EXTICR[1] &= ~(SYSCFG_EXTICR2_EXTI4);
    SYSCFG->EXTICR[1] |= (1); //อยู่ port B พอดี

    EXTI ->FTSR |= EXTI_FTSR_TR4;
    EXTI ->IMR |=  EXTI_IMR_MR4;

    NVIC_EnableIRQ(EXTI4_IRQn);

    while (1)
    {
        /* code */
    }
    

    return 0;
}

void EXTI4_IRQHandler(void) { 
    if(EXTI->PR & (1 << 4))
    {
        /* Active low*/

       if((GPIOB -> IDR & (1 << 4)) == 0)
       {
        GPIOA ->  ODR |= (1 << 5);
       }else
       {
        /*กำหนดค่าใน Input*/
        GPIOA -> ODR &= ~(1 << 5);
       }
       /*เคลีย flag*/
       EXTI -> PR = (1 << 4);
    }
}
void ADC_IRQHandler(void) { }
void USART2_IRQHandler(void) { }

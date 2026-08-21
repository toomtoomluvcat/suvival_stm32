/*******************************************************************************
 * File Name    : main.c
 * Description  : Lab 5.3 - EXTI Interrupt with Button
 *                - PA5 LED turns ON while PB4 button is pressed, OFF otherwise.
 *                - Both rising and falling edges of PB4 trigger EXTI4_IRQHandler;
 *                  the LED state is decided by reading GPIOB->IDR inside the ISR.
 *                (Chapter 5.3 of lab5.pdf)
 ******************************************************************************/
#include <stdint.h>
#define STM32F411xE
#include "stm32f4xx.h"

/* ----- EXTI4 Interrupt Handler ----- */
void EXTI4_IRQHandler(void) {
    if ((EXTI->PR & EXTI_PR_PR4) != 0) {
        if ((GPIOB->IDR & GPIO_IDR_ID4) == 0) {
            GPIOA->ODR |= GPIO_ODR_OD5;
        } else {
            GPIOA->ODR &= ~GPIO_ODR_OD5;
        }
    }

    EXTI->PR = EXTI_PR_PR4;   /* clear pending flag */
}

int main(void) {
    /* --- Setup peripheral clock --- */
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN);
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    /* --- Setup PA5 LED --- */
    GPIOA->MODER   &= ~GPIO_MODER_MODER5;
    GPIOA->MODER   |= (0b01 << GPIO_MODER_MODER5_Pos);
    GPIOA->OTYPER  &= ~GPIO_OTYPER_OT5;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5;

    /* --- Setup PB4 BTN --- */
    GPIOB->MODER &= ~GPIO_MODER_MODER4;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD4;
    GPIOB->PUPDR |= (0b01 << GPIO_PUPDR_PUPD4_Pos);

    /* --- Setup EXTI line 4 for both edges --- */
    EXTI->IMR  |= EXTI_IMR_IM4;
    EXTI->RTSR |= EXTI_RTSR_TR4;
    EXTI->FTSR |= EXTI_FTSR_TR4;

    /* --- Map EXTI line 4 to Port B --- */
    SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR2_EXTI4;
    SYSCFG->EXTICR[1] |= (1 << SYSCFG_EXTICR2_EXTI4_Pos);

    /* --- Enable EXTI4 interrupt --- */
    NVIC_EnableIRQ(EXTI4_IRQn);
    NVIC_SetPriority(EXTI4_IRQn, 0);

    while (1) {
        /* No Action */
    }
    return 0;
}
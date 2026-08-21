/*******************************************************************************
 * File Name    : main.c
 * Description  : Lab 5.2 (Interrupt Version) - Reading Potentiometer
 *                - PA5 LED turns ON when the potentiometer ADC reading
 *                  is <= 2048 (counter-clockwise side), OFF otherwise.
 *                - ADC conversion result is handled inside ADC_IRQHandler
 *                  instead of polling the EOC flag in main().
 *                (Chapter 4.1 of lab4.pdf)
 ******************************************************************************/
#include <stdint.h>
#define STM32F411xE
#include "stm32f4xx.h"

/* ----- Constant Declaration ----- */
#define THRESHOLD (133333u)   /* busy-wait delay between ADC conversions */

/* ----- ADC Interrupt Handler ----- */
void ADC_IRQHandler(void) {
    if ((ADC1->SR & ADC_SR_EOC) != 0) {
        uint16_t adc_value = ADC1->DR;

        if (adc_value <= 2048) {
            GPIOA->ODR |= GPIO_ODR_OD5;
        } else {
            GPIOA->ODR &= ~GPIO_ODR_OD5;
        }
    }
}

int main(void) {
    /* --- Setup peripheral clock --- */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    /* --- Setup PA5 LED --- */
    GPIOA->MODER   &= ~GPIO_MODER_MODER5;
    GPIOA->MODER   |= (0b01 << GPIO_MODER_MODER5_Pos);
    GPIOA->OTYPER  &= ~GPIO_OTYPER_OT5;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5;

    /* --- Setup PA4 as Analog input (ADC1_IN4) --- */
    GPIOA->MODER &= ~GPIO_MODER_MODER4;
    GPIOA->MODER |= (0b11 << GPIO_MODER_MODER4_Pos);

    /* --- ADC channel 4 setup --- */
    ADC1->CR2   |= ADC_CR2_ADON;
    ADC1->SMPR2 |= ADC_SMPR2_SMP4;

    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR1 |= (1 << ADC_SQR1_L_Pos);

    ADC1->SQR3 &= ~ADC_SQR3_SQ1;
    ADC1->SQR3 |= (4 << ADC_SQR3_SQ1_Pos);

    /* --- Enable EOC interrupt --- */
    ADC1->CR1 |= ADC_CR1_EOCIE;

    /* --- Enable ADC interrupt in NVIC --- */
    NVIC_EnableIRQ(ADC_IRQn);
    NVIC_SetPriority(ADC_IRQn, 0);

    while (1) {
        /* --- Start ADC Conversion --- */
        ADC1->CR2 |= ADC_CR2_SWSTART;

        for (uint32_t iter = 0; iter < THRESHOLD; iter++) {
            /* No Action */
        }
    }
    return 0;
}
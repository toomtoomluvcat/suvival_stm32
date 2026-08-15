#include "stm32f4xx.h"
#include "adc.h"

void adc_init(void)
{
    GPIOA->MODER &= ~GPIO_MODER_MODER4;        
    GPIOA->MODER |= (0x3u << (4u * 2u));

    ADC1->SMPR2 |= ADC_SMPR2_SMP4;          
    ADC1->SQR1  &= ~ADC_SQR1_L;
    ADC1->SQR3  &= ~ADC_SQR3_SQ1;
    ADC1->SQR3  |= (4u << ADC_SQR3_SQ1_Pos);

    ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t adc_read(void)
{
    ADC1->CR2 |= ADC_CR2_SWSTART;

    while ((ADC1->SR & ADC_SR_EOC) == 0u)
    {
        /* รอแปลงเสร็จ */
    }

    return (uint16_t)ADC1->DR;
}
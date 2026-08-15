#include <stdint.h>
#include "stm32f4xx.h"

#define ADC_THRESHOLD (4095u * 25u / 100u)

/* --- Function prototypes: ประกาศไว้ก่อนเสมอ --- */
static void clock_init(void);
static void led_init(void);
static void adc_init(void);
static uint16_t adc_read(void);
static void led_set(uint8_t state);

int main(void)
{
    /* main สะอาด อ่านแล้วเข้าใจ flow ได้ทันทีโดยไม่ต้องรู้รายละเอียด register */
    clock_init();
    led_init();
    adc_init();

    while (1)
    {
        uint16_t adc_value = adc_read();

        if (adc_value > ADC_THRESHOLD)
        {
            led_set(1u);
        }
        else
        {
            led_set(0u);
        }
    }

    return 0;
}

/* --- แต่ละฟังก์ชันทำหน้าที่เดียว (Single Responsibility) --- */

static void clock_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}

static void led_init(void)
{
    GPIOA->MODER &= ~(0b11 << (5 * 2));
    GPIOA->MODER |=  (0b01 << (5 * 2));
}

static void adc_init(void)
{
    GPIOA->MODER &= ~GPIO_MODER_MODER0; /* เปลี่ยนตัวนี้*/
    GPIOA->MODER |= (0b11 << (0 * 2));

    ADC1->SMPR2 |= ADC_SMPR2_SMP0; /*เปลี่ยนตัวนี้*/
    ADC1->SQR1  &= ~ADC_SQR1_L;
    ADC1->SQR3  &= ~ADC_SQR3_SQ1; 
    ADC1->SQR3  |= (0 << ADC_SQR3_SQ1_Pos); /*เปลี่ยนตัวนี้*/

    ADC1->CR2 |= ADC_CR2_ADON;
}

static uint16_t adc_read(void)
{
    ADC1->CR2 |= ADC_CR2_SWSTART;

    while ((ADC1->SR & ADC_SR_EOC) == 0u)
    {
        /* wait */
    }

    return (uint16_t)ADC1->DR;
}

static void led_set(uint8_t state)
{
    if (state == 1u)
    {
        GPIOA->ODR |= (1 << 5);
    }
    else
    {
        GPIOA->ODR &= ~(1 << 5);
    }
}

void EXTI3_IRQHandler(void) { }
void EXTI4_IRQHandler(void) { }
void ADC_IRQHandler(void) { }
void USART2_IRQHandler(void) { }
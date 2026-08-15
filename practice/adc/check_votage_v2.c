#include <stdint.h>
#include "stm32f4xx.h"

static void clock_init(void);


/*โจทย์ที่ 2 — ADC + GPIO แบบ 2 ระดับ threshold (แทน Q2)

อ่านค่าจาก ADC ขา PA4 (ตัวแปรตัวต้านทานปรับค่าได้ หรือเซนเซอร์) แล้วควบคุม LED 2 ดวง:

ถ้าค่า ADC (0-4095) น้อยกว่า 1365 (≈1/3 ของค่าสูงสุด) → 
LED เขียว (PB0) ติด, LED แดง (PB1) ดับ
ถ้าค่า ADC อยู่ระหว่าง 1365-2730 → LED เขียวและแดงติดพร้อมกันทั้งคู่ (เตือน)
ถ้าค่า ADC มากกว่า 2730 → LED แดง (PB1) ติด, LED เขียว (PB0) ดับ*/
static void clock_init(void);
static void led_init(void);
static void adc_init(void);
static uint16_t adc_read(void);
static void led_set( uint8_t pin, uint8_t state);

#define MIN_ADC_THERSHOLD (4095 * 1u / 3u)
#define MAX_ADC_THERSHOLD (4095 * 2u / 3u)

int your_code(void)
{
    clock_init();
    led_init();
    adc_init();

    while(1){

        uint16_t adc_value = adc_read();

        if (adc_value < MIN_ADC_THERSHOLD)
        {
            led_set(0u,1u);
            led_set(1u,0u);
        }else if (adc_value >MAX_ADC_THERSHOLD)
        {

            led_set(0u,0u);
            led_set(1u,1u);
        }
        else
        {

            led_set(0u,1u);
            led_set(1u,1u);
        }
    }
    return 0;
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

void clock_init(void)
{
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN); /* ถ้าจะใช้ PB */
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}

void adc_init(void)
{
    GPIOA->MODER &= ~GPIO_MODER_MODER4; /*เปลี่ยนตัวนี้*/
    GPIOA->MODER |= (0b11 << (4u * 2u)); /*เปลี่ยนตัวนี้*/

    ADC1->SMPR2 |= ADC_SMPR2_SMP4; /*เปลี่ยนตัวนี้*/
    ADC1->SQR1  &= ~ADC_SQR1_L;
    ADC1->SQR3  &= ~ADC_SQR3_SQ1;
    ADC1->SQR3  |= (4 << ADC_SQR3_SQ1_Pos); /*เปลี่ยนตัวนี้*/

    ADC1->CR2 |= ADC_CR2_ADON;
}

void led_init(void)
{
    /*เขียว*/
    GPIOB->MODER &= ~(0b11 << (0u * 2u));
    GPIOB->MODER |=  (0b01 << (0u * 2u));

    /*แดง*/
    GPIOB->MODER &= ~(0b11 << (1u * 2u));
    GPIOB->MODER |=  (0b01 << (1u * 2u));
}

static void led_set( uint8_t pin, uint8_t state)
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

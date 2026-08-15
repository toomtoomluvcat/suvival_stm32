#include <stdint.h>
#include "stm32f4xx.h"

int main(void)
{
    RCC->AHB1ENR |=( RCC_AHB1ENR_GPIOAEN );
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; 


    GPIOA->MODER &= ~GPIO_MODER_MODER4; 
    GPIOA->MODER |= (0b11 << (4 * 2)); /*ต้องเปลี่ยนเลขตรงนี้*/

    ADC1->SMPR2 |= ADC_SMPR2_SMP1;              /* sample time ของ channel 1 (ไม่ใช่ SMP4 เพราะเราใช้ PA1=ch1) */
    ADC1->SQR1  &= ~ADC_SQR1_L;                 /* L = 0 -> แปลงแค่ 1 ช่องต่อรอบ */
    ADC1->SQR3  &= ~ADC_SQR3_SQ1;                /* เคลียร์ field ก่อน */
    ADC1->SQR3  |= (4 << ADC_SQR3_SQ1_Pos);  /*ต้องเปลี่ยนเลขตรงนี้*/

      ADC1->CR2 |= ADC_CR2_ADON;

    uint16_t adc_value = 0u;
     while (1)
    {
        /* 5. สั่งเริ่มแปลงค่า */
        ADC1->CR2 |= ADC_CR2_SWSTART;

        /* 6. รอจน "แปลงเสร็จ" (เช็ค flag EOC ใน SR) */
        while ((ADC1->SR & ADC_SR_EOC) == 0u)
        {
            /* wait */
        }

        /* 7. อ่านค่าผลลัพธ์ */
        adc_value = (uint16_t)ADC1->DR;
    }
    return 0;
}

void EXTI3_IRQHandler(void) { }
void EXTI4_IRQHandler(void) { }
void ADC_IRQHandler(void) { }
void USART2_IRQHandler(void) { }
#include <stdint.h>
#include "stm32f4xx.h"

static void clock_init(void);
static void adc_init(void);
static void pwm_init(void);
static void pwm_set_duty(uint16_t percent);
static uint16_t adc_read(void);

#define PWM_ARR_VALUE (999u)
#define PWM_MIN_DUTY_PERCENT (5u)

/*Requirement: เหมือนข้อที่แล้ว (อ่าน ADC จาก PA1 → แปลงเป็น duty → สั่ง PWM ที่ PA6) แต่เพิ่มเงื่อนไข:

ถ้า duty cycle ที่คำนวณได้ ต่ำกว่า 5% ให้บังคับ ปิด PWM ไปเลย 
(duty = 0%) แทนที่จะปล่อยให้ LED กะพริบจาง ๆ แทบมองไม่เห็น 
(กันปัญหาไฟ "แทบไม่ติด" ซึ่งดูรกตาและกินไฟเปล่า ๆ)*/
int  main(void)
{
    clock_init();
    adc_init();
    pwm_init();
    
    while (1)
    {
          uint16_t adc_value = adc_read();   /* 0-4095 */

        /* แปลง 0-4095 ให้เป็น 0-100 (%) ด้วยสามัญสำนึกเรื่องสัดส่วน */
        uint16_t duty_percent = (uint16_t)((adc_value * 100u) / 4095u);
        pwm_set_duty(duty_percent);
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
static void pwm_set_duty(uint16_t percent)
{
    if(percent <=PWM_MIN_DUTY_PERCENT)
    {
        TIM3->CCR1 = 0u;
    }
    else if (percent <= 100u)
    {
        /* คำนวณสัดส่วน: CCR1 = ARR * (percent/100) */
        TIM3->CCR1 = (uint32_t)((PWM_ARR_VALUE * percent) / 100u);
    }
    else
    {
        /* No action: ค่า percent ผิดปกติ (เกิน 100) ไม่ทำอะไร */
    }
}

static void pwm_init(void)
{
    /* PA6 -> Alternate Function mode (10) แทนที่จะเป็น output ธรรมดา
       เพราะให้ Timer เป็นคนคุมขานี้แทนเรา ไม่ใช่เขียน ODR เอง */
    GPIOA->MODER &= ~GPIO_MODER_MODER6;
    GPIOA->MODER |= (0b10u << (6u * 2u));

    /* เลือก AF2 = TIM3 ผ่าน AFR[0] (พิน 0-7 ใช้ AFR[0], 4 บิตต่อพิน) */
    GPIOA->AFR[0] &= ~(0xFu << (6u * 4u));
    GPIOA->AFR[0] |= (2u << (6u * 4u));

    /* PSC = 15 -> clock timer = 16MHz / (15+1) = 1MHz
       ARR = 999 -> นับ 1000 ขั้นต่อรอบ -> ความถี่ PWM = 1MHz/1000 = 1kHz */
    TIM3->PSC = 15u;
    TIM3->ARR = PWM_ARR_VALUE;
    TIM3->CCR1 = 0u;   /* duty เริ่มต้น 0% (ไฟดับ) */

    /* ตั้งโหมด "PWM mode 1" ใน CCMR1 (ค่า 0b110 ในบิต OC1M) */
    TIM3->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM3->CCMR1 |= (0x6u << 4u);
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;   /* preload enable: ค่า CCR1 อัปเดตตอนจบรอบ ไม่ใช่กลางรอบ (กันไฟกะพริบแปลก ๆ) */

    TIM3->CCER |= TIM_CCER_CC1E;   /* เปิดให้ channel 1 ส่งสัญญาณออกขาจริง */
    TIM3->CR1  |= TIM_CR1_CEN;     /* เริ่มนับ (start timer) */
}



static void adc_init(void)
{
    GPIOA->MODER &= ~GPIO_MODER_MODER1;
    GPIOA->MODER |= (0b11u << (1u * 2u));

    ADC1->SMPR2 |= ADC_SMPR2_SMP1;          /* sample time ของ channel 1 */
    ADC1->SQR1  &= ~ADC_SQR1_L;             /* แปลงแค่ 1 ช่อง */
    ADC1->SQR3  &= ~ADC_SQR3_SQ1;
    ADC1->SQR3  |= (1u << ADC_SQR3_SQ1_Pos); /* channel แรก = channel 1 */

    ADC1->CR2 |= ADC_CR2_ADON;
}


static void clock_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
}

#include "stm32f4xx.h"
#include "pwm.h"

#define PWM_ARR_VALUE        (999u)
#define PWM_MIN_DUTY_PERCENT (5u)

void pwm_init(void)
{
    GPIOA->MODER  &= ~(0x3u << (6u * 2u));
    GPIOA->MODER  |=  (0x2u << (6u * 2u));
    GPIOA->AFR[0] &= ~(0xFu << (6u * 4u));
    GPIOA->AFR[0] |=  (2u   << (6u * 4u));   /* AF2 = TIM3 */

    TIM3->PSC = 15u;
    TIM3->ARR = PWM_ARR_VALUE;
    TIM3->CCR1 = 0u;

    TIM3->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM3->CCMR1 |= (0x6u << 4u);
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;

    TIM3->CCER |= TIM_CCER_CC1E;
    TIM3->CR1  |= TIM_CR1_CEN;
}

void pwm_set_duty(uint16_t percent)
{
    if (percent <= PWM_MIN_DUTY_PERCENT)
    {
        TIM3->CCR1 = 0u;
    }
    else if (percent <= 100u)
    {
        TIM3->CCR1 = (uint32_t)((PWM_ARR_VALUE * percent) / 100u);
    }
    else
    {
        /* No action: ค่า percent เกิน 100 ไม่ทำอะไร */
    }
}
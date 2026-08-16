/*******************************************************************************
 * File Name    : led_pwm.c
 * Description  : ควบคุมความสว่าง LED ทั้ง 4 ดวง (D13=PA5/TIM2_CH1,
 *                D12=PA6/TIM3_CH1, D11=PA7/TIM3_CH2, D10=PB6/TIM4_CH1)
 *                ด้วย PWM โดย duty cycle แปรผันตรงกับค่า ADC จาก potentiometer
 *                เขียนแยกทีละขา/ทีละ timer ตรง ๆ ไม่ผ่านฟังก์ชันกลางที่รับ
 *                pointer ของ port/timer เป็นพารามิเตอร์ เพื่อให้อ่านโค้ด
 *                แต่ละบล็อกแล้วรู้ทันทีว่ากำลังตั้งค่าอะไรให้ขาไหน
 * Date         : 2026-08-15
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "led_pwm.h"
#include "adc.h"
#include "stm32f4xx.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* ค่าตั้งเวลา PWM: 16MHz / (15+1) = 1MHz, / (999+1) = 1kHz, duty ละเอียด 0-999 */
#define PWM_PSC_VALUE       (15u)
#define PWM_ARR_VALUE       (999u)

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/

/* Private user code ------------------------------------------------------------*/

/* Public functions ------------------------------------------------------------*/

/*
 * ตั้งขา LED ทั้ง 4 ให้เป็น Alternate Function ของ timer ที่ตรงกัน แล้ว
 * ตั้งค่า PWM base (PSC/ARR) และเปิด channel PWM ของ TIM2, TIM3 (2 channel),
 * TIM4 ให้ครบทุกดวง เริ่มต้น duty = 0 (LED ดับสนิท) จนกว่าจะมีค่า ADC เข้ามา
 * เขียนทุกขั้นตอนตรง ๆ ทีละขา/ทีละ timer เรียงลำดับ D13 -> D12 -> D11 -> D10
 */
void led_pwm_init(void)
{
    /* ----- D13 = PA5 = TIM2_CH1 (AF1) ----- */
    GPIOA->MODER &= ~(0x3u << (5u * 2u));
    GPIOA->MODER |= (0x2u << (5u * 2u));            /* Alternate Function mode */
    GPIOA->AFR[0] &= ~(0xFu << (5u * 4u));
    GPIOA->AFR[0] |= (1u << (5u * 4u));              /* AF1 = TIM2 */

    TIM2->PSC = PWM_PSC_VALUE;
    TIM2->ARR = PWM_ARR_VALUE;
    TIM2->CCMR1 &= ~(0x7u << 4u);
    TIM2->CCMR1 |= (0x6u << 4u);                     /* OC1M = 110 = PWM mode 1 */
    TIM2->CCMR1 |= TIM_CCMR1_OC1PE;
    TIM2->CCER |= TIM_CCER_CC1E;
    TIM2->CR1 |= TIM_CR1_CEN;

    /* ----- D12 = PA6 = TIM3_CH1 (AF2) ----- */
    GPIOA->MODER &= ~(0x3u << (6u * 2u));
    GPIOA->MODER |= (0x2u << (6u * 2u));
    GPIOA->AFR[0] &= ~(0xFu << (6u * 4u));
    GPIOA->AFR[0] |= (2u << (6u * 4u));              /* AF2 = TIM3 */

    /* ----- D11 = PA7 = TIM3_CH2 (AF2) ----- */
    GPIOA->MODER &= ~(0x3u << (7u * 2u));
    GPIOA->MODER |= (0x2u << (7u * 2u));
    GPIOA->AFR[0] &= ~(0xFu << (7u * 4u));
    GPIOA->AFR[0] |= (2u << (7u * 4u));              /* AF2 = TIM3 */

    /* TIM3 ใช้ร่วมกัน 2 channel (D12, D11) จึงตั้ง PSC/ARR/CEN แค่ครั้งเดียว */
    TIM3->PSC = PWM_PSC_VALUE;
    TIM3->ARR = PWM_ARR_VALUE;
    TIM3->CCMR1 &= ~(0x7u << 4u);
    TIM3->CCMR1 |= (0x6u << 4u);                     /* channel 1 (D12): PWM mode 1 */
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;
    TIM3->CCER |= TIM_CCER_CC1E;
    TIM3->CCMR1 &= ~(0x7u << 12u);
    TIM3->CCMR1 |= (0x6u << 12u);                    /* channel 2 (D11): PWM mode 1 */
    TIM3->CCMR1 |= TIM_CCMR1_OC2PE;
    TIM3->CCER |= TIM_CCER_CC2E;
    TIM3->CR1 |= TIM_CR1_CEN;

    /* ----- D10 = PB6 = TIM4_CH1 (AF2) ----- */
    GPIOB->MODER &= ~(0x3u << (6u * 2u));
    GPIOB->MODER |= (0x2u << (6u * 2u));
    GPIOB->AFR[0] &= ~(0xFu << (6u * 4u));
    GPIOB->AFR[0] |= (2u << (6u * 4u));              /* AF2 = TIM4 */

    TIM4->PSC = PWM_PSC_VALUE;
    TIM4->ARR = PWM_ARR_VALUE;
    TIM4->CCMR1 &= ~(0x7u << 4u);
    TIM4->CCMR1 |= (0x6u << 4u);                     /* OC1M = 110 = PWM mode 1 */
    TIM4->CCMR1 |= TIM_CCMR1_OC1PE;
    TIM4->CCER |= TIM_CCER_CC1E;
    TIM4->CR1 |= TIM_CR1_CEN;
}

/*
 * แปลงค่า ADC (0-4095) เป็น duty cycle (0-PWM_ARR_VALUE) แล้วเขียนลง CCR
 * ของทุกช่องพร้อมกัน เหตุผล: ค่า 0 (หมุนสุดทวนเข็ม) ต้องได้ duty = 0
 * (LED ดับสนิทตามโจทย์) และค่ายิ่งมาก (หมุนตามเข็ม) ยิ่ง duty สูง = สว่างขึ้น
 */
void led_pwm_update(uint16_t adc_value)
{
    uint32_t duty;

    duty = ((uint32_t)adc_value * PWM_ARR_VALUE) / ADC_MAX_VALUE;

    TIM2->CCR1 = duty;   /* D13 */
    TIM3->CCR1 = duty;   /* D12 */
    TIM3->CCR2 = duty;   /* D11 */
    TIM4->CCR1 = duty;   /* D10 */
}

/* Callback functions ------------------------------------------------------------*/

/* Private functions ------------------------------------------------------------*/

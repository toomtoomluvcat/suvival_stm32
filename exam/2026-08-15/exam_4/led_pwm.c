/*******************************************************************************
 * File Name    : led_pwm.c
 * Description  : ควบคุมความสว่าง LED ทั้ง 4 ดวง (D13=PA5/TIM2_CH1,
 *                D12=PA6/TIM3_CH1, D11=PA7/TIM3_CH2, D10=PB6/TIM4_CH1)
 *                ด้วย PWM โดย duty cycle แปรผันตรงกับค่า ADC จาก potentiometer
 * Date         : 2026-08-15
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "led_pwm.h"
#include "adc.h"
#include "stm32f4xx.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* ขา LED แต่ละดวง + timer/channel ที่ผูกกับขานั้น (ดู datasheet AF mapping) */
#define LED_D13_PIN         (5u)    /* PA5  = TIM2_CH1, AF1 */
#define LED_D12_PIN         (6u)    /* PA6  = TIM3_CH1, AF2 */
#define LED_D11_PIN         (7u)    /* PA7  = TIM3_CH2, AF2 */
#define LED_D10_PIN         (6u)    /* PB6  = TIM4_CH1, AF2 */

#define GPIO_AF_TIM2        (1u)
#define GPIO_AF_TIM34       (2u)

#define GPIO_MODER_ALT_FUNC (0x2u)
#define GPIO_MODER_MASK     (0x3u)
#define GPIO_MODER_PIN_BITS (2u)

#define GPIO_AFR_MASK       (0xFu)
#define GPIO_AFR_PIN_BITS   (4u)
#define GPIO_AFR_HIGH_START (8u)   /* pin 8-15 ใช้ AFR[1], ต้องลบ 8 ก่อนคำนวณ shift */

/* ค่าตั้งเวลา PWM: 16MHz / (15+1) = 1MHz, / (999+1) = 1kHz, duty ละเอียด 0-999 */
#define PWM_PSC_VALUE       (15u)
#define PWM_ARR_VALUE       (999u)

/* โหมด PWM mode 1 (110) ของ CCMR, และตำแหน่งบิตของแต่ละ channel */
#define TIM_PWM_MODE1       (0x6u)
#define TIM_CCMR_OC1M_POS   (4u)
#define TIM_CCMR_OC2M_POS   (12u)

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/
static void configure_af_pin(GPIO_TypeDef * port, uint8_t pin, uint8_t af_value);
static void configure_timer_base(TIM_TypeDef * tim);
static void configure_channel1_pwm(TIM_TypeDef * tim);
static void configure_channel2_pwm(TIM_TypeDef * tim);

/* Private user code ------------------------------------------------------------*/

/* Public functions ------------------------------------------------------------*/

/*
 * ตั้งขา LED ทั้ง 4 ให้เป็น Alternate Function ของ timer ที่ตรงกัน แล้ว
 * ตั้งค่า PWM base (PSC/ARR) และเปิด channel PWM ของ TIM2, TIM3 (2 channel),
 * TIM4 ให้ครบทุกดวง เริ่มต้น duty = 0 (LED ดับสนิท) จนกว่าจะมีค่า ADC เข้ามา
 */
void led_pwm_init(void)
{
    configure_af_pin(GPIOA, LED_D13_PIN, GPIO_AF_TIM2);
    configure_af_pin(GPIOA, LED_D12_PIN, GPIO_AF_TIM34);
    configure_af_pin(GPIOA, LED_D11_PIN, GPIO_AF_TIM34);
    configure_af_pin(GPIOB, LED_D10_PIN, GPIO_AF_TIM34);

    configure_timer_base(TIM2);
    configure_timer_base(TIM3);
    configure_timer_base(TIM4);

    configure_channel1_pwm(TIM2);   /* D13 = PA5 */
    configure_channel1_pwm(TIM3);   /* D12 = PA6 */
    configure_channel2_pwm(TIM3);   /* D11 = PA7 */
    configure_channel1_pwm(TIM4);   /* D10 = PB6 */
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

    TIM2->CCR1 = duty;
    TIM3->CCR1 = duty;
    TIM3->CCR2 = duty;
    TIM4->CCR1 = duty;
}

/* Callback functions ------------------------------------------------------------*/

/* Private functions ------------------------------------------------------------*/

/*
 * ตั้งขาหนึ่งขาให้เป็น Alternate Function mode พร้อมเลือกเลข AF ที่ถูกต้อง
 * เหตุผล: แค่ตั้ง MODER เป็น AF ยังไม่พอ ต้องเลือกเลข AF ใน AFR ให้ตรงกับ
 * timer ที่ต้องการด้วย ไม่งั้นขาจะไปต่อกับ peripheral อื่นแทน
 */
static void configure_af_pin(GPIO_TypeDef * port, uint8_t pin, uint8_t af_value)
{
    uint8_t afr_index;
    uint8_t afr_pin;

    port->MODER &= ~(GPIO_MODER_MASK << (pin * GPIO_MODER_PIN_BITS));
    port->MODER |= (GPIO_MODER_ALT_FUNC << (pin * GPIO_MODER_PIN_BITS));

    if (pin < GPIO_AFR_HIGH_START)
    {
        afr_index = 0u;
        afr_pin = pin;
    }
    else
    {
        afr_index = 1u;
        afr_pin = pin - GPIO_AFR_HIGH_START;
    }

    port->AFR[afr_index] &= ~((uint32_t)GPIO_AFR_MASK << (afr_pin * GPIO_AFR_PIN_BITS));
    port->AFR[afr_index] |= ((uint32_t)af_value << (afr_pin * GPIO_AFR_PIN_BITS));
}

/*
 * ตั้งความถี่ PWM ให้ timer หนึ่งตัว (PSC/ARR เหมือนกันทุกตัว เพื่อให้
 * LED ทุกดวงกระพริบที่ความถี่เดียวกัน) แล้วเปิด counter ให้เริ่มนับ
 */
static void configure_timer_base(TIM_TypeDef * tim)
{
    tim->PSC = PWM_PSC_VALUE;
    tim->ARR = PWM_ARR_VALUE;
    tim->CR1 |= TIM_CR1_CEN;
}

/*
 * เปิด PWM mode 1 บน channel 1 ของ timer ที่ระบุ (ใช้ layout register
 * เดียวกันได้ทุก general purpose timer จึงใช้ฟังก์ชันเดียวกับ TIM2/TIM3/TIM4)
 */
static void configure_channel1_pwm(TIM_TypeDef * tim)
{
    tim->CCMR1 &= ~((uint32_t)TIM_PWM_MODE1 << TIM_CCMR_OC1M_POS);
    tim->CCMR1 |= ((uint32_t)TIM_PWM_MODE1 << TIM_CCMR_OC1M_POS);
    tim->CCMR1 |= TIM_CCMR1_OC1PE;
    tim->CCER |= TIM_CCER_CC1E;
}

/*
 * เปิด PWM mode 1 บน channel 2 ของ timer ที่ระบุ (ใช้กับ TIM3_CH2 = PA7)
 */
static void configure_channel2_pwm(TIM_TypeDef * tim)
{
    tim->CCMR1 &= ~((uint32_t)TIM_PWM_MODE1 << TIM_CCMR_OC2M_POS);
    tim->CCMR1 |= ((uint32_t)TIM_PWM_MODE1 << TIM_CCMR_OC2M_POS);
    tim->CCMR1 |= TIM_CCMR1_OC2PE;
    tim->CCER |= TIM_CCER_CC2E;
}

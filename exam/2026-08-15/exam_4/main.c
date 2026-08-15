/*******************************************************************************
 * File Name    : main.c
 * Description  : โปรแกรมหลัก Exam4 - หมุน potentiometer สุดทวนเข็มนาฬิกา
 *                LED ทั้ง 4 ดวงดับสนิท หมุนตามเข็มนาฬิกา LED สว่างขึ้นเรื่อย ๆ
 *                อ่านค่า potentiometer ด้วย ADC EOC interrupt (ไม่ polling)
 * Date         : 2026-08-15
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "adc.h"
#include "led_pwm.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/
static void clock_init(void);

/* Private user code ------------------------------------------------------------*/

/* main() ------------------------------------------------------------*/
int main(void)
{
    clock_init();
    led_pwm_init();
    adc_init();

    while (1)
    {
        /* ADC วิ่งแปลงค่าต่อเนื่องเองด้วย EOC interrupt (ดู adc.c)
         * main loop แค่มาเช็คว่ามีค่าใหม่หรือยัง แล้วอัปเดตความสว่าง LED
         * เหตุผลที่ไม่คำนวณ PWM ใน ISR: ให้ ISR สั้นที่สุด งานหนักมาทำที่นี่ */
        if (adc_new_data_available() == 1u)
        {
            led_pwm_update(adc_get_latest_value());
        }
        else
        {
            /* No action */
        }
    }

    return 0;
}

/* Callback functions ------------------------------------------------------------*/

/* Private functions ------------------------------------------------------------*/

/*
 * เปิด clock ให้ครบทุก peripheral ที่ใช้: GPIOA/GPIOB (LED + potentiometer),
 * ADC1 (อ่านค่า potentiometer), TIM2/TIM3/TIM4 (สร้างสัญญาณ PWM ให้ LED)
 * เหตุผล: ลืมเปิด clock ตัวไหน โค้ด compile ผ่านปกติแต่ตัวนั้นจะไม่ทำงานจริง
 */
static void clock_init(void)
{
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN);
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->APB1ENR |= (RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN);
}

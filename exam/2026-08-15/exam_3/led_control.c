/*******************************************************************************
 * File Name    : led_control.c
 * Description  : เปิด/ปิด LED สีน้ำเงิน(PA5=D13), แดง(PA6=D12), เหลือง(PA7=D11)
 *                พร้อมกันทั้ง 3 ดวงตามคำสั่งที่รับมาทาง UART
 * Date         : 2026-08-15
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "led_control.h"
#include "stm32f4xx.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define LED_BLUE_PIN         (5u)   /* PA5 = D13 = น้ำเงิน */
#define LED_RED_PIN          (6u)   /* PA6 = D12 = แดง */
#define LED_YELLOW_PIN       (7u)   /* PA7 = D11 = เหลือง */

#define GPIO_MODER_OUTPUT    (0x1u)
#define GPIO_MODER_MASK      (0x3u)
#define GPIO_MODER_PIN_BITS  (2u)

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/

/* Private user code ------------------------------------------------------------*/

/* Public functions ------------------------------------------------------------*/

/*
 * ตั้งขา LED ทั้ง 3 ดวงเป็น output
 * เหตุผล: ต้องตั้ง mode เป็น output ก่อนเขียน ODR เสมอ ไม่งั้นขายังเป็น
 * input (ค่า reset) แล้วเขียน ODR จะไม่มีผลอะไรกับ LED จริงเลย
 */
void led_control_init(void)
{
    GPIOA->MODER &= ~(GPIO_MODER_MASK << (LED_BLUE_PIN * GPIO_MODER_PIN_BITS));
    GPIOA->MODER |= (GPIO_MODER_OUTPUT << (LED_BLUE_PIN * GPIO_MODER_PIN_BITS));

    GPIOA->MODER &= ~(GPIO_MODER_MASK << (LED_RED_PIN * GPIO_MODER_PIN_BITS));
    GPIOA->MODER |= (GPIO_MODER_OUTPUT << (LED_RED_PIN * GPIO_MODER_PIN_BITS));

    GPIOA->MODER &= ~(GPIO_MODER_MASK << (LED_YELLOW_PIN * GPIO_MODER_PIN_BITS));
    GPIOA->MODER |= (GPIO_MODER_OUTPUT << (LED_YELLOW_PIN * GPIO_MODER_PIN_BITS));
}

/*
 * เปิด LED ทั้ง 3 ดวงพร้อมกัน (ตอบสนองคำสั่ง 'O')
 */
void led_all_on(void)
{
    GPIOA->ODR |= (1u << LED_BLUE_PIN);
    GPIOA->ODR |= (1u << LED_RED_PIN);
    GPIOA->ODR |= (1u << LED_YELLOW_PIN);
}

/*
 * ปิด LED ทั้ง 3 ดวงพร้อมกัน (ตอบสนองคำสั่ง 'F')
 */
void led_all_off(void)
{
    GPIOA->ODR &= ~(1u << LED_BLUE_PIN);
    GPIOA->ODR &= ~(1u << LED_RED_PIN);
    GPIOA->ODR &= ~(1u << LED_YELLOW_PIN);
}

/* Callback functions ------------------------------------------------------------*/

/* Private functions ------------------------------------------------------------*/

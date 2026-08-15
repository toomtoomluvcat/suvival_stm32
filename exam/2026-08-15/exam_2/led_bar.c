/*******************************************************************************
 * File Name    : led_bar.c
 * Description  : ควบคุม LED 4 ดวง (D13=PA5, D12=PA6, D11=PA7, D10=PB6) ให้
 *                ติดทีละดวงตามค่า potentiometer แบบ "บาร์กราฟ"
 *                ค่า ADC น้อย (หมุนสุดตามเข็มนาฬิกาตามโจทย์) = ดับหมด
 *                ค่า ADC มาก (หมุนทวนเข็มนาฬิกา) = ติดมากขึ้นทีละดวง
 * Date         : 2026-08-15
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "led_bar.h"
#include "adc.h"
#include "stm32f4xx.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/
/* ตารางเก็บว่า LED แต่ละดวงอยู่พอร์ตไหน ขาไหน เรียงจากดวงแรกที่ติดไปดวงสุดท้าย */
typedef struct
{
    GPIO_TypeDef * port;
    uint8_t pin;
} led_pin_t;

/* Private define ------------------------------------------------------------*/
#define NUM_LEDS             (4u)

#define GPIO_MODER_OUTPUT    (0x1u)
#define GPIO_MODER_MASK      (0x3u)
#define GPIO_MODER_PIN_BITS  (2u)

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/
/* ลำดับการติด: D13(น้ำเงิน) -> D12(แดง) -> D11(เหลือง) -> D10(เขียว) */
static const led_pin_t led_table[NUM_LEDS] =
{
    { GPIOA, 5u },   /* D13 = ดวงแรกที่ติด */
    { GPIOA, 6u },   /* D12 */
    { GPIOA, 7u },   /* D11 */
    { GPIOB, 6u }    /* D10 = ดวงสุดท้ายที่ติด */
};

/* Private variables ------------------------------------------------------------*/

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/
static void set_led_state(uint8_t index, uint8_t state);

/* Private user code ------------------------------------------------------------*/

/* Public functions ------------------------------------------------------------*/

/*
 * ตั้งขา LED ทั้ง 4 ดวงตามตาราง led_table ให้เป็น output ทั้งหมด
 */
void led_bar_init(void)
{
    uint8_t i;

    for (i = 0u; i < NUM_LEDS; i = i + 1u)
    {
        led_table[i].port->MODER &= ~(GPIO_MODER_MASK << (led_table[i].pin * GPIO_MODER_PIN_BITS));
        led_table[i].port->MODER |= (GPIO_MODER_OUTPUT << (led_table[i].pin * GPIO_MODER_PIN_BITS));
    }
}

/*
 * แปลงค่า ADC (0-4095) เป็นจำนวน LED ที่ต้องติด (0-4 ดวง) แล้วเปิด/ปิด
 * ให้ตรงกับจำนวนนั้น เหตุผล: หาร ADC_MAX_VALUE+1 ออกเป็น NUM_LEDS+1 ช่วง
 * เท่า ๆ กัน ทำให้ค่า ADC=0 ได้ count=0 (ดับหมด) และ ADC=4095 ได้ count=4
 * (ติดหมด) แบบขั้นบันไดเท่า ๆ กันทุกช่วง
 */
void led_bar_update(uint16_t adc_value)
{
    uint8_t count;
    uint8_t i;

    count = (uint8_t)(((uint32_t)adc_value * (NUM_LEDS + 1u)) / (ADC_MAX_VALUE + 1u));

    for (i = 0u; i < NUM_LEDS; i = i + 1u)
    {
        if (i < count)
        {
            set_led_state(i, 1u);
        }
        else
        {
            set_led_state(i, 0u);
        }
    }
}

/* Callback functions ------------------------------------------------------------*/

/* Private functions ------------------------------------------------------------*/

/*
 * เปิด/ปิด LED ดวงที่ index (อ้างอิงจาก led_table) ตามค่า state
 */
static void set_led_state(uint8_t index, uint8_t state)
{
    if (state == 1u)
    {
        led_table[index].port->ODR |= (1u << led_table[index].pin);
    }
    else
    {
        led_table[index].port->ODR &= ~(1u << led_table[index].pin);
    }
}

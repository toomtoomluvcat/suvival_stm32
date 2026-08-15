/*******************************************************************************
 * File Name    : adc.c
 * Description  : อ่านค่า potentiometer (A0 = PA0 = ADC1 channel 0) แบบ
 *                single conversion + polling (สั่งแปลง -> รอ EOC -> อ่านค่า)
 * Date         : 2026-08-15
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "adc.h"
#include "stm32f4xx.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define POT_PIN             (0u)    /* PA0 = บอร์ด A0 (potentiometer) */
#define POT_ADC_CHANNEL     (0u)    /* PA0 -> ADC channel 0 (เลขขา = เลข channel) */

#define GPIO_MODER_ANALOG   (0x3u)
#define GPIO_MODER_MASK     (0x3u)
#define GPIO_MODER_PIN_BITS (2u)

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/
static void configure_analog_pin(void);

/* Private user code ------------------------------------------------------------*/

/* Public functions ------------------------------------------------------------*/

/*
 * ตั้งขา potentiometer เป็น Analog mode, เลือก channel/sampling time
 * แล้วเปิดการทำงานของ ADC1 ไว้รอเรียกอ่านค่า
 */
void adc_init(void)
{
    configure_analog_pin();

    ADC1->SMPR2 |= ADC_SMPR2_SMP0;              /* channel 0 อยู่ใน SMPR2 */
    ADC1->SQR1 &= ~ADC_SQR1_L;                  /* L = 0000 คือแปลง 1 channel ใน sequence */
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;
    ADC1->SQR3 |= (POT_ADC_CHANNEL << ADC_SQR3_SQ1_Pos);

    ADC1->CR2 |= ADC_CR2_ADON;
}

/*
 * สั่งเริ่มแปลงค่า 1 ครั้ง รอจนกว่าจะแปลงเสร็จ (EOC ติด) แล้วคืนค่าที่อ่านได้
 * เหตุผลที่ใช้ polling: main loop เรียกฟังก์ชันนี้ซ้ำ ๆ ตลอดเวลาอยู่แล้ว
 * ไม่จำเป็นต้องใช้ interrupt ให้ซับซ้อนเกินความจำเป็นของโจทย์ข้อนี้
 */
uint16_t adc_read(void)
{
    ADC1->CR2 |= ADC_CR2_SWSTART;

    while ((ADC1->SR & ADC_SR_EOC) == 0u)
    {
        /* รอแปลงเสร็จ */
    }

    return (uint16_t)ADC1->DR;
}

/* Callback functions ------------------------------------------------------------*/

/* Private functions ------------------------------------------------------------*/

/*
 * ตั้งขา potentiometer เป็น Analog mode (11)
 * เหตุผล: ADC อ่านค่าได้ถูกต้องต้องตั้ง MODER เป็น Analog เท่านั้น
 */
static void configure_analog_pin(void)
{
    GPIOA->MODER &= ~(GPIO_MODER_MASK << (POT_PIN * GPIO_MODER_PIN_BITS));
    GPIOA->MODER |= (GPIO_MODER_ANALOG << (POT_PIN * GPIO_MODER_PIN_BITS));
}

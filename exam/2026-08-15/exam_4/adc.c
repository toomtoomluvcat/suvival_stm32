/*******************************************************************************
 * File Name    : adc.c
 * Description  : อ่านค่า potentiometer (A0 = PA0 = ADC1 channel 0) ด้วย
 *                continuous conversion mode + EOC interrupt เมื่อแปลงเสร็จ
 *                แต่ละครั้ง ISR จะเก็บค่าไว้เฉย ๆ แล้วให้ main loop ไปดึงไป
 *                ใช้ต่อ (ISR สั้นที่สุดเท่าที่ทำได้ ไม่ทำงานหนักใน interrupt)
 * Date         : 2026-08-15
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "adc.h"
#include "stm32f4xx.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define POT_GPIO_PORT       GPIOA
#define POT_PIN             (0u)    /* PA0 = บอร์ด A0 (potentiometer) */
#define POT_ADC_CHANNEL     (0u)    /* PA0 -> ADC channel 0 (เลขขา = เลข channel) */

#define GPIO_MODER_ANALOG   (0x3u)
#define GPIO_MODER_MASK     (0x3u)
#define GPIO_MODER_PIN_BITS (2u)

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/
/* ค่า ADC ล่าสุดที่ ISR อ่านมา + flag บอกว่ามีค่าใหม่ให้ main loop ไปอ่าน
 * เหตุผลที่เป็น volatile: ตัวแปรนี้ถูกเขียนใน ISR และอ่านใน main loop */
static volatile uint16_t latest_adc_value = 0u;
static volatile uint8_t data_ready_flag = 0u;

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/
static void configure_analog_pin(void);
static void configure_adc_channel(void);

/* Private user code ------------------------------------------------------------*/

/* Public functions ------------------------------------------------------------*/

/*
 * ตั้งค่าขา potentiometer เป็น Analog mode, ตั้ง channel/sampling time,
 * เปิด continuous mode + EOC interrupt แล้วเปิด ADC และยิง conversion แรก
 * เหตุผล: โจทย์บังคับให้ใช้ ADC EOC interrupt (ไม่ใช่ polling) ดังนั้นต้อง
 * เปิด CONT bit ให้ ADC วิ่งแปลงค่าต่อเนื่อง แล้วให้ EOC interrupt ยิง
 * ทุกครั้งที่แปลงเสร็จหนึ่งค่า โดยไม่ต้องมีใครมา SWSTART ซ้ำอีก
 */
void adc_init(void)
{
    configure_analog_pin();
    configure_adc_channel();

    ADC1->CR1 |= ADC_CR1_EOCIE;      /* เปิด interrupt เมื่อแปลงเสร็จ (EOC) */
    ADC1->CR2 |= ADC_CR2_CONT;       /* continuous conversion: แปลงซ้ำเรื่อย ๆ เอง */
    ADC1->CR2 |= ADC_CR2_ADON;       /* เปิดการทำงานของ ADC */

    NVIC_EnableIRQ(ADC_IRQn);

    ADC1->CR2 |= ADC_CR2_SWSTART;    /* เริ่มแปลงค่าครั้งแรก จากนั้น CONT จะแปลงต่อเองทุกครั้ง */
}

/*
 * ให้ main loop เรียกเช็คว่า ISR อ่านค่าใหม่มาหรือยัง
 * เหตุผล: อ่าน-แล้วเคลียร์ flag ในฟังก์ชันเดียวกัน ป้องกัน main loop
 * ประมวลผลค่าเดิมซ้ำหลายรอบทั้งที่ยังไม่มีค่าใหม่จาก ADC
 */
uint8_t adc_new_data_available(void)
{
    uint8_t ready;

    ready = data_ready_flag;
    data_ready_flag = 0u;

    return ready;
}

/*
 * คืนค่า ADC ล่าสุดที่ ISR เก็บไว้ (0-4095)
 */
uint16_t adc_get_latest_value(void)
{
    return latest_adc_value;
}

/* Callback functions ------------------------------------------------------------*/

/*
 * ISR ของ ADC1 (STM32F411 มี ADC ตัวเดียว ใช้ vector ชื่อ ADC_IRQHandler)
 * เหตุผลที่ทำสั้นมาก: การอ่าน ADC1->DR จะเคลียร์ flag EOC ให้อัตโนมัติ
 * (ตาม reference manual) จึงไม่ต้องเคลียร์ SR เอง แล้วงานหนัก (คำนวณ PWM)
 * ถูกโยนไปให้ main loop ทำแทน เพื่อไม่ให้ ISR ค้างนาน
 */
void ADC_IRQHandler(void)
{
    if ((ADC1->SR & ADC_SR_EOC) != 0u)
    {
        latest_adc_value = (uint16_t)ADC1->DR;
        data_ready_flag = 1u;
    }
    else
    {
        /* No action */
    }
}

/* Private functions ------------------------------------------------------------*/

/*
 * ตั้งขา potentiometer เป็น Analog mode (11)
 * เหตุผล: ADC อ่านค่าได้ถูกต้องต้องตั้ง MODER เป็น Analog เท่านั้น
 * ถ้ายังเป็น Input/Output ค่าที่อ่านได้จะไม่ใช่แรงดันจริงจากขา
 */
static void configure_analog_pin(void)
{
    POT_GPIO_PORT->MODER &= ~(GPIO_MODER_MASK << (POT_PIN * GPIO_MODER_PIN_BITS));
    POT_GPIO_PORT->MODER |= (GPIO_MODER_ANALOG << (POT_PIN * GPIO_MODER_PIN_BITS));
}

/*
 * ตั้ง sampling time และเลือก channel ที่จะแปลงใน regular sequence
 * เหตุผล: channel ใน SMPRx และ SQR3 ต้องตรงกับขาที่ใช้จริงเป๊ะ (PA0 = channel 0)
 * ไม่งั้น ADC จะไปแปลงค่าจากขาอื่นที่ไม่ใช่ potentiometer
 */
static void configure_adc_channel(void)
{
    ADC1->SMPR2 |= ADC_SMPR2_SMP0;              /* channel 0 อยู่ใน SMPR2 */
    ADC1->SQR1 &= ~ADC_SQR1_L;                  /* L = 0000 คือแปลง 1 channel ใน sequence */
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;
    ADC1->SQR3 |= (POT_ADC_CHANNEL << ADC_SQR3_SQ1_Pos);
}

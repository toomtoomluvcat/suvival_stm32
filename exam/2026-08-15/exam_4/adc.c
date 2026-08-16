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
#define POT_ADC_CHANNEL     (0u)    /* PA0 -> ADC channel 0 (เลขขา = เลข channel) */

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/
/* ค่า ADC ล่าสุดที่ ISR อ่านมา + flag บอกว่ามีค่าใหม่ให้ main loop ไปอ่าน
 * เหตุผลที่เป็น volatile: ตัวแปรนี้ถูกเขียนใน ISR และอ่านใน main loop */
static volatile uint16_t latest_adc_value = 0u;
static volatile uint8_t data_ready_flag = 0u;

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/

/* Private user code ------------------------------------------------------------*/

/* Public functions ------------------------------------------------------------*/

/*
 * ตั้งขา potentiometer, เลือก channel/sampling time, เปิด continuous
 * mode + EOC interrupt แล้วเปิด ADC และยิง conversion แรก (เขียนทุก
 * ขั้นตอนไว้ในฟังก์ชันเดียวเรียงตามลำดับที่ทำจริง)
 * เหตุผล: โจทย์บังคับให้ใช้ ADC EOC interrupt (ไม่ใช่ polling) ดังนั้นต้อง
 * เปิด CONT bit ให้ ADC วิ่งแปลงค่าต่อเนื่อง แล้วให้ EOC interrupt ยิง
 * ทุกครั้งที่แปลงเสร็จหนึ่งค่า โดยไม่ต้องมีใครมา SWSTART ซ้ำอีก
 */
void adc_init(void)
{
    /* PA0 = Analog mode (11) */
    GPIOA->MODER &= ~(0x3u << (0u * 2u));
    GPIOA->MODER |= (0x3u << (0u * 2u));

    /* channel 0 อยู่ใน SMPR2 */
    ADC1->SMPR2 |= ADC_SMPR2_SMP0;

    /* L = 0000 คือแปลง 1 channel ใน sequence */
    ADC1->SQR1 &= ~ADC_SQR1_L;

    /* ลำดับที่ 1 ของ sequence อ่าน channel 0 */
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;
    ADC1->SQR3 |= (POT_ADC_CHANNEL << ADC_SQR3_SQ1_Pos);

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

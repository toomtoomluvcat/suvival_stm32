/*******************************************************************************
 * File Name    : sensor.c
 * Description  : อ่านค่าอุณหภูมิ (A1=PA1=channel1) และความสว่าง
 *                (A2=PA4=channel4) ผ่าน ADC1 ทีละ channel แบบ polling
 *                *** ข้อสมมติ (ปรับได้ถ้าฮาร์ดแวร์จริงต่างจากนี้) ***
 *                - เซนเซอร์อุณหภูมิ = LM35 (10mV ต่อ 1 องศาเซลเซียส, 0V=0 องศา)
 *                - เซนเซอร์ความสว่าง = LDR แปลงค่าดิบเป็น Lux แบบเชิงเส้นอย่างง่าย
 * Date         : 2026-08-15
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "sensor.h"
#include "stm32f4xx.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define TEMP_PIN             (1u)   /* PA1 = A1 = เซนเซอร์อุณหภูมิ */
#define TEMP_CHANNEL         (1u)

#define LIGHT_PIN            (4u)   /* PA4 = A2 = เซนเซอร์ความสว่าง */
#define LIGHT_CHANNEL        (4u)

#define GPIO_MODER_ANALOG    (0x3u)
#define GPIO_MODER_MASK      (0x3u)
#define GPIO_MODER_PIN_BITS  (2u)

#define ADC_MAX_VALUE        (4095u)
#define ADC_VREF_MILLIVOLT   (3300u)

#define LM35_MILLIVOLT_TO_MILLIDEGC (100u) /* 10mV/องศา -> 1mV = 100 millidegree C */
#define LDR_LUX_SCALE_DIVISOR       (10u)  /* สูตรอย่างง่าย: lux = adc_raw / 10 */

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/
static void configure_analog_pin(uint8_t pin);
static uint16_t adc_convert_channel(uint8_t channel);

/* Private user code ------------------------------------------------------------*/

/* Public functions ------------------------------------------------------------*/

/*
 * ตั้งขาเซนเซอร์ทั้ง 2 เป็น Analog mode, ตั้ง sampling time ให้ทั้ง 2
 * channel แล้วเปิด ADC1 ไว้รอใช้งาน (ยังไม่เริ่มแปลงค่าจนกว่าจะถูกเรียกอ่าน)
 */
void sensor_init(void)
{
    configure_analog_pin(TEMP_PIN);
    configure_analog_pin(LIGHT_PIN);

    ADC1->SMPR2 |= ADC_SMPR2_SMP1;
    ADC1->SMPR2 |= ADC_SMPR2_SMP4;

    ADC1->CR2 |= ADC_CR2_ADON;
}

/*
 * อ่านค่าอุณหภูมิปัจจุบัน คืนค่าเป็นหน่วย millidegree Celsius
 * เหตุผล: แปลง ADC ดิบ -> แรงดัน(mV) -> องศา ตามสเปก LM35 (10mV ต่อ 1 องศา)
 * ใช้เลขจำนวนเต็มล้วน (คูณก่อนหารเสมอ) เพื่อไม่ให้ผลลัพธ์เพี้ยนจาก integer truncation
 */
uint32_t sensor_read_temperature_millidegc(void)
{
    uint16_t adc_raw;
    uint32_t millivolt;
    uint32_t millidegc;

    adc_raw = adc_convert_channel(TEMP_CHANNEL);
    millivolt = ((uint32_t)adc_raw * ADC_VREF_MILLIVOLT) / ADC_MAX_VALUE;
    millidegc = millivolt * LM35_MILLIVOLT_TO_MILLIDEGC;

    return millidegc;
}

/*
 * อ่านค่าความสว่างปัจจุบัน คืนค่าเป็นหน่วย Lux (โดยประมาณ)
 * เหตุผล: ใช้สูตรเชิงเส้นอย่างง่ายจาก ADC ดิบ เพราะ LDR จริงมีความสัมพันธ์
 * แบบไม่เชิงเส้น แต่โจทย์ต้องการความง่ายในการอธิบาย จึงประมาณแบบเส้นตรง
 */
uint32_t sensor_read_light_lux(void)
{
    uint16_t adc_raw;
    uint32_t lux;

    adc_raw = adc_convert_channel(LIGHT_CHANNEL);
    lux = (uint32_t)adc_raw / LDR_LUX_SCALE_DIVISOR;

    return lux;
}

/* Callback functions ------------------------------------------------------------*/

/* Private functions ------------------------------------------------------------*/

/*
 * ตั้งขาหนึ่งขาเป็น Analog mode (11) เพื่อให้ ADC อ่านแรงดันจริงจากขานั้นได้
 */
static void configure_analog_pin(uint8_t pin)
{
    GPIOA->MODER &= ~(GPIO_MODER_MASK << (pin * GPIO_MODER_PIN_BITS));
    GPIOA->MODER |= (GPIO_MODER_ANALOG << (pin * GPIO_MODER_PIN_BITS));
}

/*
 * เลือก channel ที่ต้องการแปลงค่าใน SQR3 แล้วสั่งเริ่มแปลง รอจน EOC ติด
 * แล้วอ่านค่าออกมา เหตุผลที่เลือก channel ใหม่ทุกครั้ง: ADC1 มีตัวเดียว
 * ต้องใช้ร่วมกันระหว่าง 2 เซนเซอร์ จึงสลับ channel ก่อนแปลงค่าทุกครั้งที่เรียก
 */
static uint16_t adc_convert_channel(uint8_t channel)
{
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;
    ADC1->SQR3 |= ((uint32_t)channel << ADC_SQR3_SQ1_Pos);

    ADC1->CR2 |= ADC_CR2_SWSTART;

    while ((ADC1->SR & ADC_SR_EOC) == 0u)
    {
        /* รอแปลงเสร็จ */
    }

    return (uint16_t)ADC1->DR;
}

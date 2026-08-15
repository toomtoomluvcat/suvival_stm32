/*******************************************************************************
 * File Name    : seven_seg.c
 * Description  : ควบคุมขา GPIO ที่ต่อกับ 7-segment display แบบ BCD 4 บิต
 *                (PC7=2^0, PA8=2^1, PB10=2^2, PA9=2^3)
 * Date         : 2026-08-15
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "seven_seg.h"
#include "stm32f4xx.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* ขา MCU ที่ต่อกับแต่ละบิตของ 7-segment (ดู pinout Training Shield สีแดง) */
#define SEG_BIT0_PIN        (7u)    /* PC7  = บิต 2^0 */
#define SEG_BIT1_PIN        (8u)    /* PA8  = บิต 2^1 */
#define SEG_BIT2_PIN        (10u)   /* PB10 = บิต 2^2 */
#define SEG_BIT3_PIN        (9u)    /* PA9  = บิต 2^3 */

/* ตำแหน่งบิตของ digit ที่จะดึงออกมา (2^0 - 2^3) */
#define BIT_POSITION_0      (0u)
#define BIT_POSITION_1      (1u)
#define BIT_POSITION_2      (2u)
#define BIT_POSITION_3      (3u)

/* ค่าคงที่สำหรับตั้ง MODER ให้เป็น General purpose output mode */
#define GPIO_MODER_OUTPUT   (0x1u)
#define GPIO_MODER_MASK     (0x3u)
#define GPIO_MODER_PIN_BITS (2u)

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/
static void configure_output_pin(GPIO_TypeDef * port, uint8_t pin);
static void set_pin_state(GPIO_TypeDef * port, uint8_t pin, uint8_t state);

/* Private user code ------------------------------------------------------------*/

/* Public functions ------------------------------------------------------------*/

/*
 * ตั้งค่าขา PC7, PA8, PB10, PA9 เป็น output ทั้งหมด
 * เหตุผล: ต้องตั้ง mode ให้ตรงก่อนเขียนค่า ODR เสมอ ไม่งั้นขายังเป็น
 * input (ค่า reset ของ MODER) แล้วเขียน ODR จะไม่มีผลอะไรกับขาจริงเลย
 */
void seven_seg_init(void)
{
    configure_output_pin(GPIOC, SEG_BIT0_PIN);
    configure_output_pin(GPIOA, SEG_BIT1_PIN);
    configure_output_pin(GPIOB, SEG_BIT2_PIN);
    configure_output_pin(GPIOA, SEG_BIT3_PIN);
}

/*
 * แสดงเลข 0-9 บน 7-segment โดยแยกแต่ละบิตของ digit ออกมาแล้วเขียนลงขา
 * ที่ตรงกัน เหตุผล: 7-segment ต่อแบบ BCD 4 บิต ต้องแปลงเลข -> บิต -> ขา
 * ให้ครบทั้ง 4 เส้นทุกครั้งที่อัปเดตค่า ไม่งั้นเลขที่ค้างอยู่จะไม่เปลี่ยน
 */
void seven_seg_display(uint8_t digit)
{
    uint8_t bit0;
    uint8_t bit1;
    uint8_t bit2;
    uint8_t bit3;

    bit0 = (uint8_t)((digit >> BIT_POSITION_0) & 1u);
    bit1 = (uint8_t)((digit >> BIT_POSITION_1) & 1u);
    bit2 = (uint8_t)((digit >> BIT_POSITION_2) & 1u);
    bit3 = (uint8_t)((digit >> BIT_POSITION_3) & 1u);

    set_pin_state(GPIOC, SEG_BIT0_PIN, bit0);
    set_pin_state(GPIOA, SEG_BIT1_PIN, bit1);
    set_pin_state(GPIOB, SEG_BIT2_PIN, bit2);
    set_pin_state(GPIOA, SEG_BIT3_PIN, bit3);
}

/* Callback functions ------------------------------------------------------------*/

/* Private functions ------------------------------------------------------------*/

/*
 * ตั้งค่าขาหนึ่งขาให้เป็น General purpose output mode
 * เหตุผล: ใช้ pattern clear-then-set เสมอ เพื่อไม่ให้บิตเก่าของ MODER
 * (ที่อาจไม่ใช่ 00 จาก reset) มาปนกับค่าที่ตั้งใหม่
 */
static void configure_output_pin(GPIO_TypeDef * port, uint8_t pin)
{
    port->MODER &= ~(GPIO_MODER_MASK << (pin * GPIO_MODER_PIN_BITS));
    port->MODER |= (GPIO_MODER_OUTPUT << (pin * GPIO_MODER_PIN_BITS));
}

/*
 * เขียนค่า 0/1 ลงขาหนึ่งขาผ่าน ODR
 * เหตุผล: รวม logic set/clear บิตไว้ที่เดียว ลดโอกาสเขียนขาผิดหรือลืมกลับบิต
 */
static void set_pin_state(GPIO_TypeDef * port, uint8_t pin, uint8_t state)
{
    if (state == 1u)
    {
        port->ODR |= (1u << pin);
    }
    else
    {
        port->ODR &= ~(1u << pin);
    }
}

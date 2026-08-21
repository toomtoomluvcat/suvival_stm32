/*******************************************************************************
 * File Name    : button.c
 * Description  : อ่านปุ่มกด 3 ปุ่ม (PB4=เพิ่ม, PA10=ลด, PB3=รีเซ็ต) แบบ
 *                polling เช็คสถานะ "ปัจจุบัน" ตรง ๆ (ไม่ edge-detect)
 *                เหตุผลที่ไม่ edge-detect: โจทย์บอกว่ากดค้างต้องให้เลข
 *                ขยับต่อเนื่องไปเรื่อย ๆ ถ้าใช้ edge-detect (แบบ Exam1 เดิม
 *                ที่ต้องปล่อยแล้วกดใหม่ถึงจะขยับอีกที) จะขัดกับโจทย์ข้อนี้
 * Date         : 2026-08-16
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "button.h"
#define STM32F411xE         /* บอก CMSIS ว่าใช้ชิปรุ่น STM32F411xE (Nucleo-F411RE) */
#include "stm32f4xx.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define BUTTON_PRESSED       (0u)    /* active-low: กด = 0 */

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/

/* Private user code ------------------------------------------------------------*/

/* Public functions ------------------------------------------------------------*/

/*
 * ตั้งค่าขาปุ่มกดทั้ง 3 ขาเป็น input + internal pull-up (เขียนตรง ๆ ทีละขา)
 * เหตุผล: ปุ่มบนบอร์ดเป็น active-low ไม่มีตัวต้านทานภายนอก ต้องเปิด
 * pull-up เอง ไม่งั้นตอนไม่กดขาจะลอย (floating) อ่านค่าไม่แน่นอน
 */
void button_init(void)
{
    /* PB4 = ปุ่มเพิ่มค่า (D5) */
    GPIOB->MODER &= ~(0x3u << (4u * 2u));
    GPIOB->PUPDR &= ~(0x3u << (4u * 2u));
    GPIOB->PUPDR |= (0x1u << (4u * 2u));

    /* PA10 = ปุ่มลดค่า (D2) */
    GPIOA->MODER &= ~(0x3u << (10u * 2u));
    GPIOA->PUPDR &= ~(0x3u << (10u * 2u));
    GPIOA->PUPDR |= (0x1u << (10u * 2u));

    /* PB3 = ปุ่มรีเซ็ต (D3) */
    GPIOB->MODER &= ~(0x3u << (3u * 2u));
    GPIOB->PUPDR &= ~(0x3u << (3u * 2u));
    GPIOB->PUPDR |= (0x1u << (3u * 2u));
}

/*
 * คืนค่า 1 ตราบเท่าที่ปุ่มเพิ่มค่า (PB4) ยังถูกกดอยู่ตอนนี้ (เช็คสถานะสด ๆ
 * ทุกครั้งที่เรียก ไม่มีการจำสถานะเก่า) เหตุผล: โจทย์ต้องการให้กดค้างแล้ว
 * เลขขยับต่อเนื่อง main loop จึงต้องเห็นว่า "ตอนนี้ปุ่มกดอยู่ไหม" ทุกรอบ
 */
uint8_t button_increase_is_pressed(void)
{
    uint8_t now_state;
    uint8_t is_pressed;

    now_state = (uint8_t)((GPIOB->IDR >> 4u) & 1u);
    is_pressed = 0u;

    if (now_state == BUTTON_PRESSED)
    {
        is_pressed = 1u;
    }
    else
    {
        /* No action */
    }

    return is_pressed;
}

/*
 * คืนค่า 1 ตราบเท่าที่ปุ่มลดค่า (PA10) ยังถูกกดอยู่ตอนนี้
 * (logic เหมือน button_increase_is_pressed ทุกอย่าง เปลี่ยนแค่ขาที่อ่าน)
 */
uint8_t button_decrease_is_pressed(void)
{
    uint8_t now_state;
    uint8_t is_pressed;

    now_state = (uint8_t)((GPIOA->IDR >> 10u) & 1u);
    is_pressed = 0u;

    if (now_state == BUTTON_PRESSED)
    {
        is_pressed = 1u;
    }
    else
    {
        /* No action */
    }

    return is_pressed;
}

/*
 * คืนค่า 1 ตราบเท่าที่ปุ่มรีเซ็ต (PB3) ยังถูกกดอยู่ตอนนี้
 * (logic เหมือน button_increase_is_pressed ทุกอย่าง เปลี่ยนแค่ขาที่อ่าน)
 */
uint8_t button_reset_is_pressed(void)
{
    uint8_t now_state;
    uint8_t is_pressed;

    now_state = (uint8_t)((GPIOB->IDR >> 3u) & 1u);
    is_pressed = 0u;

    if (now_state == BUTTON_PRESSED)
    {
        is_pressed = 1u;
    }
    else
    {
        /* No action */
    }

    return is_pressed;
}

/* Callback functions ------------------------------------------------------------*/

/* Private functions ------------------------------------------------------------*/

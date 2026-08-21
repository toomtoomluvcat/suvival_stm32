/*******************************************************************************
 * File Name    : main.c
 * Description  : โปรแกรมหลัก Exam1 (16 ส.ค.) - แสดงเลข 0-9 บน 7-segment
 *                PB4 = เพิ่มค่า (กดค้างขยับต่อเนื่อง, wrap 9->0)
 *                PA10 = ลดค่า (กดค้างขยับต่อเนื่อง, ค้างที่ 0 ไม่ wrap ไป 9)
 *                PB3 = รีเซ็ตเป็น 0 (ใช้งานไม่ได้ตอนเลขเป็น 5)
 * Date         : 2026-08-16
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#define STM32F411xE         /* บอก CMSIS ว่าใช้ชิปรุ่น STM32F411xE (Nucleo-F411RE) */
#include "stm32f4xx.h"
#include "seven_seg.h"
#include "button.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define DIGIT_MIN            (0u)
#define DIGIT_MAX            (9u)
#define RESET_LOCKED_VALUE   (5u)   /* ปุ่มรีเซ็ตใช้งานไม่ได้ตอนเลขเป็นค่านี้ */

#define HOLD_STEP_DELAY_LOOP (500000u) /* จำนวนรอบ busy-wait ต่อ 1 step ตอนกดค้าง */

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/
static uint8_t counter = 0u;

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/
static void clock_init(void);
static void step_delay(void);

/* Private user code ------------------------------------------------------------*/

/* main() ------------------------------------------------------------*/
int main(void)
{
    clock_init();
    seven_seg_init();
    button_init();

    /* แสดงค่าเริ่มต้น (0) ทันทีตอนเปิดเครื่อง ก่อนรอปุ่มกดใด ๆ */
    seven_seg_display(counter);

    while (1)
    {
        /* เช็คสถานะปุ่ม "สด ๆ" ทุกรอบ (ไม่ edge-detect) เพื่อให้กดค้างแล้ว
         * เลขขยับต่อเนื่องไปเรื่อย ๆ ตามโจทย์ */
        if (button_increase_is_pressed() == 1u)
        {
            /* wrap-around: 9 -> 0 เมื่อกดเพิ่มต่อจากค่าสูงสุด */
            if (counter == DIGIT_MAX)
            {
                counter = DIGIT_MIN;
            }
            else
            {
                counter = counter + 1u;
            }
        }
        else
        {
            /* No action */
        }

        if (button_decrease_is_pressed() == 1u)
        {
            /* โจทย์ข้อนี้ห้าม wrap จาก 0 ไป 9 ค่าจึงค้างที่ 0 แทนการวนกลับ */
            if (counter == DIGIT_MIN)
            {
                /* No action: ค้างที่ 0 */
            }
            else
            {
                counter = counter - 1u;
            }
        }
        else
        {
            /* No action */
        }

        if (button_reset_is_pressed() == 1u)
        {
            /* ปุ่มรีเซ็ตใช้งานไม่ได้ตอนเลขเป็น 5 ตามโจทย์ */
            if (counter == RESET_LOCKED_VALUE)
            {
                /* No action: ล็อกปุ่มรีเซ็ตไว้ตอนเลขเป็น 5 */
            }
            else
            {
                counter = DIGIT_MIN;
            }
        }
        else
        {
            /* No action */
        }

        seven_seg_display(counter);

        /* หน่วงเวลาทุกรอบ loop เพื่อให้ตอนกดปุ่มค้าง เลขขยับเป็นจังหวะที่
         * ตาเห็นทัน ไม่ใช่วิ่งเร็วจนมองไม่เห็นการเปลี่ยนแปลง (ใช้ busy-wait
         * ธรรมดา ไม่ใช้ timer/interrupt เพื่อให้โค้ดเรียบง่ายที่สุด) */
        step_delay();
    }

    return 0;
}

/* Callback functions ------------------------------------------------------------*/

/* Private functions ------------------------------------------------------------*/

/*
 * เปิด clock ให้ครบทุก peripheral ที่ใช้: GPIOA/B/C (ปุ่มกด + 7-segment)
 */
static void clock_init(void)
{
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
}

/*
 * หน่วงเวลาแบบ busy-wait (วนลูปเปล่า ๆ นับรอบ) ไม่ใช้ timer เพื่อให้โค้ด
 * ง่ายที่สุดเท่าที่จะทำได้ ตัวเลขรอบวนเป็นค่าที่ปรับได้ตาม HOLD_STEP_DELAY_LOOP
 */
static void step_delay(void)
{
    uint32_t i;

    for (i = 0u; i < HOLD_STEP_DELAY_LOOP; i = i + 1u)
    {
        /* หน่วงเวลาเฉย ๆ ไม่ทำอะไร */
    }
}

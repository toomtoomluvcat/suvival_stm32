/*******************************************************************************
 * File Name    : main.c
 * Description  : โปรแกรมหลัก Exam1 - แสดงเลข 0-9 บน 7-segment ควบคุมด้วย
 *                ปุ่มกด 3 ปุ่ม (PA10=เพิ่ม, PB3=ลด, PB5=รีเซ็ต) แบบ wrap-around
 * Date         : 2026-08-15
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "seven_seg.h"
#include "button.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define DIGIT_MIN   (0u)
#define DIGIT_MAX   (9u)

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/
static uint8_t counter = 0u;

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/
static void clock_init(void);

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
        if (button_increase_pressed() == 1u)
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
            seven_seg_display(counter);
        }
        else
        {
            /* No action */
        }

        if (button_decrease_pressed() == 1u)
        {
            /* wrap-around: 0 -> 9 เมื่อกดลดต่อจากค่าต่ำสุด
             * ห้ามใช้ (counter - 1u) % 10u เพราะ counter เป็น unsigned
             * ถ้า counter = 0 จะ underflow เป็นเลขมหาศาลก่อนหารเสมอ */
            if (counter == DIGIT_MIN)
            {
                counter = DIGIT_MAX;
            }
            else
            {
                counter = counter - 1u;
            }
            seven_seg_display(counter);
        }
        else
        {
            /* No action */
        }

        if (button_reset_pressed() == 1u)
        {
            counter = DIGIT_MIN;
            seven_seg_display(counter);
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
 * เปิด clock ให้ GPIOA/B/C ก่อนแตะ register ใด ๆ ของพอร์ตเหล่านี้
 * เหตุผล: ปุ่มและ 7-segment กระจายอยู่บน PA, PB, PC ทั้งหมด ถ้าลืมเปิด
 * clock พอร์ตไหน โค้ดจะ compile ผ่านปกติแต่ขานั้นจะไม่ตอบสนองจริงบนบอร์ด
 */
static void clock_init(void)
{
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
}

/*******************************************************************************
 * File Name    : total.c
 * Description  : รวมโค้ดทั้งหมดของ Exam1 (16 ส.ค.) ไว้ในไฟล์เดียว
 *                (main.c + button.c/.h + seven_seg.c/.h) เพื่อก็อปไปวางใน
 *                โปรเจกต์อื่นแล้ว build/run ได้เลยโดยไม่ต้องแยกไฟล์
 *
 *                แสดงเลข 0-9 บน 7-segment
 *                PB4  = เพิ่มค่า (กดค้างขยับต่อเนื่อง, wrap 9->0)
 *                PA10 = ลดค่า (กดค้างขยับต่อเนื่อง, ค้างที่ 0 ไม่ wrap ไป 9)
 *                PB3  = รีเซ็ตเป็น 0 (ใช้งานไม่ได้ตอนเลขเป็น 5)
 * Date         : 2026-08-16
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#ifndef STM32F411xE
#define STM32F411xE         /* บอก CMSIS ว่าใช้ชิปรุ่น STM32F411xE (Nucleo-F411RE)
                              * (ครอบด้วย #ifndef กัน warning "redefined" กรณีโปรเจกต์
                              * ปลายทางตั้งค่า -DSTM32F411xE ไว้แล้วใน build settings) */
#endif
#include "stm32f4xx.h"
#include <stdint.h>

/* ===========================================================================
 * seven_seg.h (รวมเข้ามาในไฟล์เดียว)
 * ===========================================================================
 * Description  : Header สำหรับโมดูลควบคุม 7-segment display (BCD 4 บิต)
 * ===========================================================================*/
static void seven_seg_init(void);
static void seven_seg_display(uint8_t digit);

/* ===========================================================================
 * button.h (รวมเข้ามาในไฟล์เดียว)
 * ===========================================================================
 * Description  : Header สำหรับโมดูลอ่านปุ่มกดแบบ polling ธรรมดา (เช็คสถานะ
 *                ปัจจุบันของขาโดยตรง ไม่ทำ edge-detect เพราะโจทย์ต้องการ
 *                ให้กดค้างแล้วค่าขยับต่อเนื่อง)
 * ===========================================================================*/
static void button_init(void);
static uint8_t button_increase_is_pressed(void);
static uint8_t button_decrease_is_pressed(void);
static uint8_t button_reset_is_pressed(void);

/* Private define ------------------------------------------------------------*/
#define DIGIT_MIN            (0u)
#define DIGIT_MAX            (9u)
#define RESET_LOCKED_VALUE   (5u)   /* ปุ่มรีเซ็ตใช้งานไม่ได้ตอนเลขเป็นค่านี้ */

#define HOLD_STEP_DELAY_LOOP (500000u) /* จำนวนรอบ busy-wait ต่อ 1 step ตอนกดค้าง */

#define BUTTON_PRESSED       (0u)    /* active-low: กด = 0 */

/* Private variables ------------------------------------------------------------*/
static uint8_t counter = 0u;

/* Private function prototypes ------------------------------------------------*/
static void clock_init(void);
static void step_delay(void);

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

/* ===========================================================================
 * button.c (รวมเข้ามาในไฟล์เดียว)
 * ===========================================================================
 * Description  : อ่านปุ่มกด 3 ปุ่ม (PB4=เพิ่ม, PA10=ลด, PB3=รีเซ็ต) แบบ
 *                polling เช็คสถานะ "ปัจจุบัน" ตรง ๆ (ไม่ edge-detect)
 *                เหตุผลที่ไม่ edge-detect: โจทย์บอกว่ากดค้างต้องให้เลข
 *                ขยับต่อเนื่องไปเรื่อย ๆ ถ้าใช้ edge-detect (แบบ Exam1 เดิม
 *                ที่ต้องปล่อยแล้วกดใหม่ถึงจะขยับอีกที) จะขัดกับโจทย์ข้อนี้
 * ===========================================================================*/

/*
 * ตั้งค่าขาปุ่มกดทั้ง 3 ขาเป็น input + internal pull-up (เขียนตรง ๆ ทีละขา)
 * เหตุผล: ปุ่มบนบอร์ดเป็น active-low ไม่มีตัวต้านทานภายนอก ต้องเปิด
 * pull-up เอง ไม่งั้นตอนไม่กดขาจะลอย (floating) อ่านค่าไม่แน่นอน
 */
static void button_init(void)
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
static uint8_t button_increase_is_pressed(void)
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
static uint8_t button_decrease_is_pressed(void)
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
static uint8_t button_reset_is_pressed(void)
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

/* ===========================================================================
 * seven_seg.c (รวมเข้ามาในไฟล์เดียว)
 * ===========================================================================
 * Description  : ควบคุมขา GPIO ที่ต่อกับ 7-segment display แบบ BCD 4 บิต
 *                (PC7=2^0, PA8=2^1, PB10=2^2, PA9=2^3)
 * ===========================================================================*/

/*
 * ตั้งค่าขา PC7, PA8, PB10, PA9 เป็น output ทั้งหมด (เขียนตรง ๆ ทีละขา
 * ไม่ผ่านฟังก์ชันกลาง เพื่อให้อ่านแล้วรู้ทันทีว่าขาไหนเป็นบิตอะไร)
 * เหตุผล: ต้องตั้ง mode ให้ตรงก่อนเขียนค่า ODR เสมอ ไม่งั้นขายังเป็น
 * input (ค่า reset ของ MODER) แล้วเขียน ODR จะไม่มีผลอะไรกับขาจริงเลย
 */
static void seven_seg_init(void)
{
    /* PC7 = บิต 2^0 */
    GPIOC->MODER &= ~(0x3u << (7u * 2u));
    GPIOC->MODER |= (0x1u << (7u * 2u));

    /* PA8 = บิต 2^1 */
    GPIOA->MODER &= ~(0x3u << (8u * 2u));
    GPIOA->MODER |= (0x1u << (8u * 2u));

    /* PB10 = บิต 2^2 */
    GPIOB->MODER &= ~(0x3u << (10u * 2u));
    GPIOB->MODER |= (0x1u << (10u * 2u));

    /* PA9 = บิต 2^3 */
    GPIOA->MODER &= ~(0x3u << (9u * 2u));
    GPIOA->MODER |= (0x1u << (9u * 2u));
}

/*
 * แสดงเลข 0-9 บน 7-segment โดยแยกแต่ละบิตของ digit ออกมา แล้วเขียนลงขา
 * ที่ตรงกันทีละบิต เหตุผล: 7-segment ต่อแบบ BCD 4 บิต ต้องแปลงเลข -> บิต
 * -> ขา ให้ครบทั้ง 4 เส้นทุกครั้งที่อัปเดตค่า ไม่งั้นเลขที่ค้างอยู่จะไม่เปลี่ยน
 */
static void seven_seg_display(uint8_t digit)
{
    uint8_t bit0;
    uint8_t bit1;
    uint8_t bit2;
    uint8_t bit3;

    /* ดึงแต่ละบิตออกมาด้วยการ shift ขวาแล้ว mask ด้วย & 1 */
    bit0 = (uint8_t)((digit >> 0u) & 1u);
    bit1 = (uint8_t)((digit >> 1u) & 1u);
    bit2 = (uint8_t)((digit >> 2u) & 1u);
    bit3 = (uint8_t)((digit >> 3u) & 1u);

    /* บิต 2^0 -> PC7 */
    if (bit0 == 1u)
    {
        GPIOC->ODR |= (1u << 7u);
    }
    else
    {
        GPIOC->ODR &= ~(1u << 7u);
    }

    /* บิต 2^1 -> PA8 */
    if (bit1 == 1u)
    {
        GPIOA->ODR |= (1u << 8u);
    }
    else
    {
        GPIOA->ODR &= ~(1u << 8u);
    }

    /* บิต 2^2 -> PB10 */
    if (bit2 == 1u)
    {
        GPIOB->ODR |= (1u << 10u);
    }
    else
    {
        GPIOB->ODR &= ~(1u << 10u);
    }

    /* บิต 2^3 -> PA9 */
    if (bit3 == 1u)
    {
        GPIOA->ODR |= (1u << 9u);
    }
    else
    {
        GPIOA->ODR &= ~(1u << 9u);
    }
}

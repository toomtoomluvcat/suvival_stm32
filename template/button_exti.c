/*******************************************************************************
 * File Name    : button_exti.c
 * Description  : Button EXTI control (PA10=increase, PB3=decrease, PB5=reset)
 * Date         : 2026-08-15
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "button_exti.h"
#include "seven_seg.h"

/* Private includes -------------------------------------------------------------*/

/* Private typedef --------------------------------------------------------------*/

/* Private enum ------------------------------------------------------------------*/

/* Private struct ----------------------------------------------------------------*/

/* Private union -----------------------------------------------------------------*/

/* Private define ----------------------------------------------------------------*/
#define DIGIT_MAX (9u)

/* Private macro -----------------------------------------------------------------*/

/* Private constants -------------------------------------------------------------*/

/* Private variables --------------------------------------------------------------*/
volatile uint8_t g_counter = 0u;

/* External variables --------------------------------------------------------------*/

/* Private function prototypes -------------------------------------------------------*/

/* Private user code ---------------------------------------------------------------*/

/* Public functions ------------------------------------------------------------------*/
/*********************************************************************
 * @fn                 - button_exti_init
 * @brief              - ตั้งค่าปุ่ม PA10/PB3/PB5 เป็น input pull-up และตั้ง EXTI
 *                        แบบ falling edge (trigger ครั้งเดียวต่อการกด 1 ครั้ง)
 *
 * @return             - void
 *
 * @Note               - ต้องเปิด clock ของ GPIOA/GPIOB/SYSCFG ก่อนเรียกฟังก์ชันนี้
 *//////////////////////////////////////////////////////////////////////
void button_exti_init(void)
{
    /* PA10 = เพิ่มค่า -> EXTI line 10 */
    GPIOA->MODER &= ~(0x3u << (10u * 2u));
    GPIOA->PUPDR &= ~(0x3u << (10u * 2u));
    GPIOA->PUPDR |=  (0x1u << (10u * 2u));

    /* PB3 = ลดค่า -> EXTI line 3 */
    GPIOB->MODER &= ~(0x3u << (3u * 2u));
    GPIOB->PUPDR &= ~(0x3u << (3u * 2u));
    GPIOB->PUPDR |=  (0x1u << (3u * 2u));

    /* PB5 = รีเซ็ต -> EXTI line 5 */
    GPIOB->MODER &= ~(0x3u << (5u * 2u));
    GPIOB->PUPDR &= ~(0x3u << (5u * 2u));
    GPIOB->PUPDR |=  (0x1u << (5u * 2u));

    /* line 10 -> index=2, shift=8, Port A=0x0 */
    SYSCFG->EXTICR[2] &= ~(0xFu << 8u);
    SYSCFG->EXTICR[2] |=  (0x0u << 8u);

    /* line 3 -> index=0, shift=12, Port B=0x1 */
    SYSCFG->EXTICR[0] &= ~(0xFu << 12u);
    SYSCFG->EXTICR[0] |=  (0x1u << 12u);

    /* line 5 -> index=1, shift=4, Port B=0x1 */
    SYSCFG->EXTICR[1] &= ~(0xFu << 4u);
    SYSCFG->EXTICR[1] |=  (0x1u << 4u);

    EXTI->FTSR |= (1u << 10u);
    EXTI->IMR  |= (1u << 10u);

    EXTI->FTSR |= (1u << 3u);
    EXTI->IMR  |= (1u << 3u);

    EXTI->FTSR |= (1u << 5u);
    EXTI->IMR  |= (1u << 5u);

    NVIC_EnableIRQ(EXTI15_10_IRQn);
    NVIC_EnableIRQ(EXTI3_IRQn);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/* Callback functions ----------------------------------------------------------*/
/*********************************************************************
 * @fn                 - EXTI15_10_IRQHandler
 * @brief              - ISR ปุ่ม PA10 (เพิ่มค่า) เรียกอัตโนมัติเมื่อ EXTI line 10 trigger
 *
 * @return             - void
 *
 * @Note               - ต้องเป็นชื่อนี้เป๊ะๆ ตรงกับ vector table (ห้าม static)
 *//////////////////////////////////////////////////////////////////////
void EXTI15_10_IRQHandler(void)
{
    if ((EXTI->PR & (1u << 10u)) != 0u)
    {
        EXTI->PR |= (1u << 10u);
        g_counter = (uint8_t)((g_counter + 1u) % 10u);
        seven_seg_display(g_counter);
    }
    else
    {
        /* No action */
    }
}

/*********************************************************************
 * @fn                 - EXTI3_IRQHandler
 * @brief              - ISR ปุ่ม PB3 (ลดค่า) เรียกอัตโนมัติเมื่อ EXTI line 3 trigger
 *
 * @return             - void
 *
 * @Note               -
 *//////////////////////////////////////////////////////////////////////
void EXTI3_IRQHandler(void)
{
    if ((EXTI->PR & (1u << 3u)) != 0u)
    {
        EXTI->PR |= (1u << 3u);

        if (g_counter == 0u)
        {
            g_counter = DIGIT_MAX;
        }
        else
        {
            g_counter = g_counter - 1u;
        }

        seven_seg_display(g_counter);
    }
    else
    {
        /* No action */
    }
}

/*********************************************************************
 * @fn                 - EXTI9_5_IRQHandler
 * @brief              - ISR ปุ่ม PB5 (รีเซ็ต) เรียกอัตโนมัติเมื่อ EXTI line 5 trigger
 *
 * @return             - void
 *
 * @Note               -
 *//////////////////////////////////////////////////////////////////////
void EXTI9_5_IRQHandler(void)
{
    if ((EXTI->PR & (1u << 5u)) != 0u)
    {
        EXTI->PR |= (1u << 5u);
        g_counter = 0u;
        seven_seg_display(g_counter);
    }
    else
    {
        /* No action */
    }
}

/* Private functions -----------------------------------------------------------*/

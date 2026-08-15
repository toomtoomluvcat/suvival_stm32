#include <stdint.h>
#include "stm32f4xx.h"

static void clock_init(void);
static void button_exti_init(void);

static volatile uint8_t counter = 0u;

int main(void)
{
    clock_init();
    button_exti_init();
    while (1)
    {
        /* ไม่ต้องทำอะไร */
    }
    return 0;
}

static void clock_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;   /* 🔧 [1] เปลี่ยน GPIOB -> GPIOA/GPIOC ตาม PORT ของขาปุ่ม */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;  /* ตัวนี้ตายตัว ไม่ต้องแก้ (ใช้ EXTI ต้องเปิดเสมอ) */
}

static void button_exti_init(void)
{
    /* --- ตั้งขาเป็น Input + Pull-up --- */
    GPIOB->MODER &= ~(0x3u << (3u * 2u));   /* 🔧 [2] GPIOB -> port จริง, เลข 3 (x2 จุดนี้) -> เลขขาจริง */
    GPIOB->PUPDR &= ~(0x3u << (3u * 2u));   /* 🔧 [3] เหมือนกัน: GPIOB -> port จริง, เลข 3 -> เลขขาจริง */
    GPIOB->PUPDR |=  (0x1u << (3u * 2u));   /* 🔧 [3] (ต่อ) เลข 3 ตัวนี้ต้องตรงกับ [2]-[3] ทุกจุด */

    /* --- บอก EXTI ว่า line นี้มาจาก Port ไหน --- */
    /* 🔧 [4] EXTICR[?] : index = เลข_line ÷ 4  (คำนวณจากเลขขา เพราะ line = เลขขาเสมอ) */
    SYSCFG->EXTICR[0] &= ~(0xFu << 12u);    /* 🔧 [5] เลข shift = (เลข_line mod 4) × 4 */
    SYSCFG->EXTICR[0] |=  (0x1u << 12u);    /* 🔧 [5] (ต่อ) shift ต้องตรงกับบรรทัดบน
                                                 🔧 [6] ค่า 0x1 -> รหัส PORT: A=0x0, B=0x1, C=0x2 */

    /* --- Trigger + Unmask --- */
    EXTI->FTSR |= (1u << 3u);               /* 🔧 [7] เลข 3 -> เลข_line (=เลขขา) */
    EXTI->IMR  |= (1u << 3u);               /* 🔧 [7] (ต่อ) เลข 3 ต้องตรงกับ FTSR ด้านบน */

    NVIC_EnableIRQ(EXTI3_IRQn);             /* 🔧 [8] ชื่อ IRQn ต้องเลือกตามเลข_line (ดูตารางข้างล่าง) */
}

void EXTI3_IRQHandler(void)                 /* 🔧 [9] ชื่อฟังก์ชันต้องตรงกับ [8] เป๊ะๆ (ดูตาราง) */
{
    if ((EXTI->PR & (1u << 3u)) != 0u)      /* 🔧 [10] เลข 3 -> เลข_line เหมือนเดิม */
    {
        EXTI->PR |= (1u << 3u);             /* 🔧 [10] (ต่อ) เลข 3 ต้องตรงกับบรรทัดบน */

        counter++;                          /* 🔧 [11] โค้ดที่อยากให้ทำงาน — เปลี่ยนเป็น logic จริงของโจทย์ */
    }
    else
    {
        /* No action */
    }
}
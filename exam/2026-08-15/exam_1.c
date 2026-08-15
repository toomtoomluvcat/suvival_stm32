#include <stdint.h>
#include "stm32f4xx.h"

/* --- BEGIN AUTO PROTOTYPES (tools/gen_prototypes.py) --- */
static void clock_init(void);
static void button_exti_init(void);
static void display_digit(uint8_t number);
/* --- END AUTO PROTOTYPES --- */


volatile uint8_t counter = 0u;

int main(void)
{
    clock_init();
    seven_pin_init();
    button_exti_init();
    while (1)
    {
        /* No action เพราะว่าสั่งผ่าน EXTI อย่างเดียว */
    }
    
    return 0;
}
static void clock_init(void)
{
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;  /* ตัวนี้ตายตัว ไม่ต้องแก้ (ใช้ EXTI ต้องเปิดเสมอ) */
}


void EXTI15_10_IRQHandler(void)                 /* 🔧 [9] ชื่อฟังก์ชันต้องตรงกับ [8] เป๊ะๆ (ดูตาราง) */
{
   if ((EXTI->PR & (1u << 10u)) != 0u)
    {

        EXTI->PR |= (1u << 10u);             /* 🔧 [10] (ต่อ) เลข 3 ต้องตรงกับบรรทัดบน */

        counter = (counter + 1u) % 10u; 
        display_digit(counter) ;  
    }
    else
    {
        /* No action */
    }
}

 void EXTI3_IRQHandler(void)                 /* 🔧 [9] ชื่อฟังก์ชันต้องตรงกับ [8] เป๊ะๆ (ดูตาราง) */
{
    if ((EXTI->PR & (1u << 3u)) != 0u)      /* 🔧 [10] เลข 3 -> เลข_line เหมือนเดิม */
    {
        EXTI->PR |= (1u << 3u);             /* 🔧 [10] (ต่อ) เลข 3 ต้องตรงกับบรรทัดบน */
        if (counter == 0u)
        {
            counter = 9u;
        }else
        {
            counter = counter - 1u;
        }
        display_digit(counter) ;                        /* 🔧 [11] โค้ดที่อยากให้ทำงาน — เปลี่ยนเป็น logic จริงของโจทย์ */
    }
    else
    {
        /* No action */
    }
}

 void EXTI9_5_IRQHandler(void)                 /* 🔧 [9] ชื่อฟังก์ชันต้องตรงกับ [8] เป๊ะๆ (ดูตาราง) */
{
    if ((EXTI->PR & (1u << 5u)) != 0u)      /* 🔧 [10] เลข 3 -> เลข_line เหมือนเดิม */
    {
        EXTI->PR |= (1u << 5u);             /* 🔧 [10] (ต่อ) เลข 3 ต้องตรงกับบรรทัดบน */

        counter = 0u; 
        display_digit(counter) ;                        /* 🔧 [11] โค้ดที่อยากให้ทำงาน — เปลี่ยนเป็น logic จริงของโจทย์ */
    }
    else
    {
        /* No action */
    }
}

static void button_exti_init(void)
{
    /* --- ตั้งขาเป็น Input + Pull-up --- */
    GPIOA->MODER &= ~(0x3u << (10u * 2u));  
    GPIOA->PUPDR &= ~(0x3u << (10u * 2u));  
    GPIOA->PUPDR |=  (0x1u << (10u * 2u));  

    GPIOB->MODER &= ~(0x3u << (3u * 2u));  
    GPIOB->PUPDR &= ~(0x3u << (3u * 2u));  
    GPIOB->PUPDR |=  (0x1u << (3u * 2u));  

    GPIOB->MODER &= ~(0x3u << (5u * 2u));  
    GPIOB->PUPDR &= ~(0x3u << (5u * 2u));  
    GPIOB->PUPDR |=  (0x1u << (5u * 2u));  

    /* --- บอก EXTI ว่า line นี้มาจาก Port ไหน --- */
    /* 🔧 [4] EXTICR[?] : index = เลข_line ÷ 4  (คำนวณจากเลขขา เพราะ line = เลขขาเสมอ) */
    SYSCFG->EXTICR[2] &= ~(0xFu << 8u);    /* 🔧 [5] เลข shift = (เลข_line mod 4) × 4 */
    SYSCFG->EXTICR[2] |=  (0x0u << 8u);    /* 🔧 [5] (ต่อ) shift ต้องตรงกับบรรทัดบน */

    SYSCFG->EXTICR[0] &= ~(0xFu << 12u);    /* 🔧 [5] เลข shift = (เลข_line mod 4) × 4 */
    SYSCFG->EXTICR[0] |=  (0x1u << 12u);    /* 🔧 [5] (ต่อ) shift ต้องตรงกับบรรทัดบน*/


    SYSCFG->EXTICR[1] &= ~(0xFu << 4u);    /* 🔧 [5] เลข shift = (เลข_line mod 4) × 4 */
    SYSCFG->EXTICR[1] |=  (0x1u << 4u);    /* 🔧 [5] (ต่อ) shift ต้องตรงกับบรรทัดบน
                                                 🔧 [6] ค่า 0x1 -> รหัส PORT: A=0x0, B=0x1, C=0x2 */

    EXTI->FTSR |= (1u << 5u);               /* 🔧 [7] เลข 3 -> เลข_line (=เลขขา) */
    EXTI->IMR  |= (1u << 5u);               /* 🔧 [7] (ต่อ) เลข 3 ต้องตรงกับ FTSR ด้านบน */
                                              
    
    EXTI->FTSR |= (1u << 3u);               /* 🔧 [7] เลข 3 -> เลข_line (=เลขขา) */
    EXTI->IMR  |= (1u << 3u);               /* 🔧 [7] (ต่อ) เลข 3 ต้องตรงกับ FTSR ด้านบน */

    /* --- Trigger + Unmask --- */
    EXTI->FTSR |= (1u << 10u);               /* 🔧 [7] เลข 3 -> เลข_line (=เลขขา) */
    EXTI->IMR  |= (1u << 10u);               /* 🔧 [7] (ต่อ) เลข 3 ต้องตรงกับ FTSR ด้านบน */

    NVIC_EnableIRQ(EXTI3_IRQn);   
    NVIC_EnableIRQ(EXTI9_5_IRQn);   

    NVIC_EnableIRQ(EXTI15_10_IRQn);             /* 🔧 [8] ชื่อ IRQn ต้องเลือกตามเลข_line (ดูตารางข้างล่าง) */
}

void seven_pin_init(void){
    GPIOC->MODER &= ~(0b11 << (7u * 2u));
    GPIOC->MODER |=  (0b01 << (7u * 2u));   /* PC7 = 2^0 */

    GPIOA->MODER &= ~(0b11 << (8u * 2u));
    GPIOA->MODER |=  (0b01 << (8u * 2u));   /* PA8 = 2^1 */

    GPIOB->MODER &= ~(0b11 << (10u * 2u));
    GPIOB->MODER |=  (0b01 << (10u * 2u));  /* PB10 = 2^2 */

    GPIOA->MODER &= ~(0b11 << (9u * 2u));
    GPIOA->MODER |=  (0b01 << (9u * 2u));   /* PA9 = 2^3 */
}


static void display_digit(uint8_t number)
{
    /* แยกแต่ละบิตของ number ออกมาด้วยการ shift ขวา แล้ว mask ด้วย & 1 */
    uint8_t bit0 = (number >> 0) & 1u;   /* บิต 2^0 -> PC7 */
    uint8_t bit1 = (number >> 1) & 1u;   /* บิต 2^1 -> PA8 */
    uint8_t bit2 = (number >> 2) & 1u;   /* บิต 2^2 -> PB10 */
    uint8_t bit3 = (number >> 3) & 1u;   /* บิต 2^3 -> PA9 */

    /* ตั้งค่าขาแต่ละขาตามบิตที่แยกออกมา ทีละขาอิสระต่อกัน */
    if (bit0 == 1u)
    {
        GPIOC->ODR |= (1 << 7);
    }
    else
    {
        GPIOC->ODR &= ~(1 << 7);
    }

    if (bit1 == 1u)
    {
        GPIOA->ODR |= (1 << 8);
    }
    else
    {
        GPIOA->ODR &= ~(1 << 8);
    }

    if (bit2 == 1u)
    {
        GPIOB->ODR |= (1 << 10);
    }
    else
    {
        GPIOB->ODR &= ~(1 << 10);
    }

    if (bit3 == 1u)
    {
        GPIOA->ODR |= (1 << 9);
    }
    else
    {
        GPIOA->ODR &= ~(1 << 9);
    }
}
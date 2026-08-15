#include <stdint.h>
#include "stm32f4xx.h"

volatile uint8_t counter = 0u;

static void clock_init(void);
static void seven_pin_init(void);
static void display_digit(uint8_t number);
static void delay_1s(void);


/*เขียนโปรแกรมควบคุม 7-segment ให้แสดงเลข 0-9 วนซ้ำอัตโนมัติทุก 1 วินาที
 โดยไม่ใช้ปุ่มกด (ใช้ delay function หรือ SysTick ก็ได้)
  เมื่อถึง 9 แล้วให้วนกลับไป 0 ใหม่*/
int your_code(void)
{
    clock_init();
    seven_pin_init();

    display_digit(0u);
    while(1){

        display_digit(counter);
        delay_1s();
        counter = (counter+1)%10;
    }
    return 0;
}

void clock_init(void)
{
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

}
void delay_1s(void)
{
    for (uint32_t i = 0u; i < 1333333u; i++)
    {
        /* รอ */
    }
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

void display_digit(uint8_t number)
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
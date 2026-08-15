/*
 * sim_test_7seg.c
 * คอมไพล์: gcc -Wall -Wextra -std=c99 sim_test_7seg.c -o sim_test && ./sim_test
 */
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx_sim.h"
#include "your_code.c"   /* ฟังก์ชันของผู้เรียน (ไม่มี main) */

static uint8_t g_fail_count = 0u;

static void check_pin(const char *name, const GPIO_TypeDef *port, uint8_t pin, uint8_t expected)
{
    uint8_t actual = (uint8_t)((port->ODR >> pin) & 1u);

    if (actual == expected)
    {
        printf("    [PASS] %-6s = %u\n", name, (unsigned int)actual);
    }
    else
    {
        printf("    [FAIL] %-6s = %u  (คาดว่าจะเป็น %u)\n", name, (unsigned int)actual, (unsigned int)expected);
        g_fail_count++;
    }
}

int main(void)
{
    uint8_t digit;

    clock_init();
    seven_pin_init();

    for (digit = 0u; digit <= 9u; digit++)
    {
        printf("--- ทดสอบเลข %u ---\n", (unsigned int)digit);
        display_digit(digit);

        /* ผัง: 2^0=PC7, 2^1=PA8, 2^2=PB10, 2^3=PA9 */
        check_pin("PC7",  GPIOC, 7u,  (uint8_t)((digit >> 0) & 1u));
        check_pin("PA8",  GPIOA, 8u,  (uint8_t)((digit >> 1) & 1u));
        check_pin("PB10", GPIOB, 10u, (uint8_t)((digit >> 2) & 1u));
        check_pin("PA9",  GPIOA, 9u,  (uint8_t)((digit >> 3) & 1u));
    }

    printf("\n================================\n");
    if (g_fail_count == 0u)
    {
        printf("ผลสรุป: PASS ทั้งหมด (0 จุดผิด) -> logic ถูกต้อง!\n");
    }
    else
    {
        printf("ผลสรุป: FAIL %u จุด -> กลับไปดู logic ในฟังก์ชัน display_digit()\n", (unsigned int)g_fail_count);
    }

    return 0;
}
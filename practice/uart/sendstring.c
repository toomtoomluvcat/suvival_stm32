#include <stdint.h>
#include "stm32f4xx.h"

/* ประกาศฟังก์ชันล่วงหน้า */
static void clock_init(void);
static void uart_init(void);
static void uart_send_data(uint8_t data);
static void uart_send_string(const char *str);
static void delay_1s(void);

int main(void)
{
    /* เปิด Clock ที่จำเป็นสำหรับ GPIOA และ USART2 */
    clock_init();

    /* ตั้งค่า GPIO และ USART2 สำหรับการส่งข้อมูล */
    uart_init();

    while (1)
    {
        /* ส่งข้อความ "Hello world" ออกทาง UART */
        /* \r = Carriage Return, \n = New Line */
        uart_send_string("Hello world\r\n");

        /* หน่วงเวลาประมาณ 1 วินาที */
        delay_1s();
    }

    return 0;
}

/*
 * เปิด Clock ที่จำเป็น
 *
 * GPIOA:
 * ใช้ PA2 เป็น USART2_TX
 *
 * USART2:
 * ใช้สำหรับส่งข้อมูล UART
 */
static void clock_init(void)
{
    /* เปิด Clock ของ GPIOA */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* เปิด Clock ของ USART2 */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
}

/*
 * ตั้งค่า USART2 และ PA2 สำหรับ UART TX
 */
static void uart_init(void)
{
    /*
     * ตั้ง PA2 เป็น Alternate Function
     *
     * MODER:
     * 00 = Input
     * 01 = Output
     * 10 = Alternate Function
     * 11 = Analog
     */
    GPIOA->MODER &= ~(0b11 << (2 * 2));
    GPIOA->MODER |=  (0b10 << (2 * 2));

    /*
     * เลือก Alternate Function ของ PA2
     *
     * PA2 ใช้ USART2_TX
     * USART2 ใช้ AF7
     *
     * AFR[0] ใช้สำหรับ Pin 0-7
     * แต่ละ Pin ใช้ 4 bits
     *
     * PA2:
     * 2 * 4 = 8
     *
     * ดังนั้น AF7 จะถูกใส่ที่ bits 11:8
     */
    GPIOA->AFR[0] &= ~(0xF << (2 * 4));
    GPIOA->AFR[0] |=  (7 << (2 * 4));

    /*
     * ตั้งค่า Baud Rate
     *
     * 139 = ค่าที่กำหนดไว้สำหรับ
     * 115200 baud เมื่อ USART Clock = 16 MHz
     */
    USART2->BRR = 139;

    /*
     * เปิด Transmitter
     *
     * TE = Transmitter Enable
     * ทำให้ USART2 สามารถส่งข้อมูลออกทาง TX ได้
     */
    USART2->CR1 |= USART_CR1_TE;

    /*
     * เปิดใช้งาน USART2
     *
     * UE = USART Enable
     */
    USART2->CR1 |= USART_CR1_UE;
}

/*
 * ส่งข้อมูล 1 byte ผ่าน USART2
 */
static void uart_send_data(uint8_t data)
{
    /*
     * รอจนกว่า Data Register จะพร้อมรับข้อมูลใหม่
     *
     * TXE = Transmit Data Register Empty
     *
     * TXE = 0 → ยังไม่พร้อม ต้องรอ
     * TXE = 1 → พร้อมส่งข้อมูลใหม่
     */
    while ((USART2->SR & USART_SR_TXE) == 0)
    {
        /* รอจนกว่า TXE จะเป็น 1 */
    }

    /*
     * ใส่ข้อมูลลง Data Register
     * USART จะนำข้อมูลนี้ไปส่งออกทาง TX
     */
    USART2->DR = data;
}

/*
 * ส่ง String ผ่าน UART
 *
 * str เป็น pointer ที่ชี้ไปยัง String
 *
 * String ในภาษา C จะจบด้วย '\0'
 */
static void uart_send_string(const char *str)
{
    /*
     * ส่งทีละตัวอักษร
     *
     * ทำจนกว่าจะเจอ '\0'
     */
    while (*str != '\0')
    {
        /* ส่งตัวอักษรปัจจุบัน */
        uart_send_data((uint8_t)*str);

        /* เลื่อนไปยังตัวอักษรถัดไป */
        str++;
    }
}

/*
 * Delay ประมาณ 1 วินาที
 *
 * เป็น Software Delay
 * CPU จะวน Loop ทิ้งไว้เฉย ๆ
 *
 * ค่า 1333333 ต้องปรับตาม Clock
 * และ Compiler/Optimization ของระบบ
 */
static void delay_1s(void)
{
    for (uint32_t i = 0u; i < 1333333u; i++)
    {
        /* รอ */
    }
}


/*
 * Interrupt Handler
 *
 * ยังไม่ได้ใช้งานในโปรแกรมนี้
 */
void EXTI3_IRQHandler(void)
{
}

void EXTI4_IRQHandler(void)
{
}

void ADC_IRQHandler(void)
{
}

/*
 * USART2 Interrupt Handler
 *
 * ยังไม่ได้ใช้ เพราะโปรแกรมนี้ใช้
 * Polling ผ่าน TXE แทน Interrupt
 */
void USART2_IRQHandler(void)
{
}
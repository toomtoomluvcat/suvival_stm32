/*******************************************************************************
 * File Name    : uart.c
 * Description  : รับ-ส่งข้อมูลผ่าน USART2 แบบ polling ล้วน ๆ (ไม่มี interrupt)
 *                ฟังก์ชันรับ (uart_receive_char) เป็นแบบ blocking คือ
 *                จะหยุดรอจนกว่าจะมีตัวอักษรเข้ามาจริง ๆ ทำให้เขียน main loop
 *                ได้ง่าย ไม่ต้องเช็ค flag เอง
 * Date         : 2026-08-16
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "uart.h"
#include "stm32f4xx.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* สมมติ clock ระบบเป็นค่า default หลัง reset คือ HSI 16MHz ไม่มี PLL/prescaler
 * ถ้าโปรเจกต์มีการตั้ง SystemClock_Config เปลี่ยนความถี่ ต้องแก้ค่านี้ตาม */
#define PCLK1_HZ            (16000000u)
#define UART_BAUD_RATE      (9600u)

#define ASCII_DIGIT_ZERO    (48u)  /* รหัส ASCII ของตัวอักษร '0' */
#define DECIMAL_BASE        (10u)
#define MAX_DIGIT_COUNT     (10u)  /* uint32_t สูงสุดมีได้ไม่เกิน 10 หลัก */

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/

/* Private user code ------------------------------------------------------------*/

/* Public functions ------------------------------------------------------------*/

/*
 * ตั้งขา PA2(TX)/PA3(RX) เป็น USART2, ตั้ง baud rate แล้วเปิดฝั่งส่ง-รับ-USART
 * เขียนทุกขั้นตอนไว้ในฟังก์ชันเดียวเรียงตามลำดับที่ทำจริง
 * เหตุผล: ต้องเปิดทั้ง TE (ส่ง) และ RE (รับ) เสมอ ถ้าลืม RE จะรับข้อมูล
 * จากผู้ใช้ไม่ได้เลยแม้ว่าส่งออกได้ปกติ
 */
void uart_init(void)
{
    /* PA2 = TX ตั้งเป็น Alternate Function AF7 (USART2) */
    GPIOA->MODER &= ~(0x3u << (2u * 2u));
    GPIOA->MODER |= (0x2u << (2u * 2u));
    GPIOA->AFR[0] &= ~(0xFu << (2u * 4u));
    GPIOA->AFR[0] |= (0x7u << (2u * 4u));

    /* PA3 = RX ตั้งเป็น Alternate Function AF7 (USART2) */
    GPIOA->MODER &= ~(0x3u << (3u * 2u));
    GPIOA->MODER |= (0x2u << (3u * 2u));
    GPIOA->AFR[0] &= ~(0xFu << (3u * 4u));
    GPIOA->AFR[0] |= (0x7u << (3u * 4u));

    /* BRR = PCLK / baudrate (สูตรอย่างง่าย ใช้ oversampling 16 ค่า default) */
    USART2->BRR = PCLK1_HZ / UART_BAUD_RATE;

    USART2->CR1 |= USART_CR1_TE;
    USART2->CR1 |= USART_CR1_RE;
    USART2->CR1 |= USART_CR1_UE;
}

/*
 * ส่งข้อมูล 1 ตัวอักษรออกทาง UART แบบ polling
 * เหตุผล: ต้องรอ TXE (TX buffer ว่าง) ก่อนเขียน DR เสมอ ไม่งั้นจะเขียน
 * ทับข้อมูลเก่าที่ยังส่งไม่เสร็จ ทำให้ข้อมูลหาย
 */
void uart_send_char(uint8_t data)
{
    while ((USART2->SR & USART_SR_TXE) == 0u)
    {
        /* รอจนกว่า TX buffer จะว่าง */
    }
    USART2->DR = data;
}

/*
 * ส่งข้อความทั้งสตริงออกทาง UART ทีละตัวอักษรจนเจอ '\0'
 * เหตุผล: ใช้ char ปกติแทน uint8_t เพราะ string literal ใน C เป็น char
 * โดยธรรมชาติ ทำให้เรียกใช้ตรง ๆ ได้ เช่น uart_send_string("LED OFF")
 */
void uart_send_string(const char * str)
{
    uint32_t i;

    i = 0u;
    /*เพราะในภาษา C String จะมีตัวจบที่เรียกว่า Null Terminator คือ '\0'*/
    while (str[i] != '\0')
    {
        uart_send_char((uint8_t)str[i]);
        i = i + 1u;
    }
}

/*
 * แปลงตัวเลข (uint32_t) เป็นสตริงแล้วส่งออกทาง UART โดยไม่ใช้ sprintf
 * เหตุผล: โจทย์ห้ามใช้ sprintf จึงต้องแตกหลักเองด้วยการหาร/มอด 10 ไปเรื่อย ๆ
 * ซึ่งจะได้หลักหน่วยออกมาก่อน จึงต้องเก็บใส่ buffer แล้วพิมพ์ย้อนกลับ
 */
void uart_send_number(uint32_t number)
{
    uint8_t digit_buffer[MAX_DIGIT_COUNT];
    uint8_t digit_count;
    uint32_t remaining;

    digit_count = 0u;
    remaining = number;

    if (remaining == 0u)
    {
        uart_send_char((uint8_t)ASCII_DIGIT_ZERO);
    }
    else
    {
        while (remaining > 0u)
        {
            digit_buffer[digit_count] = (uint8_t)((remaining % DECIMAL_BASE) + ASCII_DIGIT_ZERO);
            remaining = remaining / DECIMAL_BASE;
            digit_count = digit_count + 1u;
        }

        while (digit_count > 0u)
        {
            digit_count = digit_count - 1u;
            uart_send_char(digit_buffer[digit_count]);
        }
    }
}

/*
 * รอรับตัวอักษร 1 ตัวจาก UART แบบ blocking (ค้างรอจนกว่าจะมีข้อมูลเข้ามา)
 * เหตุผล: โปรแกรมนี้ไม่มีงานอื่นให้ทำระหว่างรอคำสั่ง จึง block รอได้เลย
 * ทำให้ main loop เขียนง่าย ไม่ต้องเช็ค flag ซับซ้อน
 */
uint8_t uart_receive_char(void)
{
    while ((USART2->SR & USART_SR_RXNE) == 0u)
    {
        /* รอจนกว่าจะมีข้อมูลเข้ามา */
    }
    return (uint8_t)USART2->DR;
}

/* Callback functions ------------------------------------------------------------*/

/* Private functions ------------------------------------------------------------*/

/*******************************************************************************
 * File Name    : uart.c
 * Description  : รับ-ส่งข้อมูลผ่าน USART2 แบบ polling ล้วน ๆ (ไม่มี interrupt)
 *                ฟังก์ชันรับ (uart_receive_char) เป็นแบบ blocking คือ
 *                จะหยุดรอจนกว่าจะมีตัวอักษรเข้ามาจริง ๆ ทำให้เขียน main loop
 *                ได้ง่าย ไม่ต้องเช็ค flag เอง
 * Date         : 2026-08-15
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "uart.h"
#include "stm32f4xx.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define UART_TX_PIN         (2u)   /* PA2 = USART2_TX */
#define UART_RX_PIN         (3u)   /* PA3 = USART2_RX */
#define UART_AF_USART2      (0x7u) /* USART2 อยู่บน AF7 */

#define GPIO_MODER_ALT_FUNC (0x2u)
#define GPIO_MODER_MASK     (0x3u)
#define GPIO_MODER_PIN_BITS (2u)

#define GPIO_AFR_MASK       (0xFu)
#define GPIO_AFR_PIN_BITS   (4u)

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
static void configure_uart_pins(void);

/* Private user code ------------------------------------------------------------*/

/* Public functions ------------------------------------------------------------*/

/*
 * ตั้งขา PA2/PA3 เป็น USART2 แล้วตั้ง baud rate, เปิดฝั่งส่ง-รับ-USART
 * เหตุผล: ต้องเปิดทั้ง TE (ส่ง) และ RE (รับ) เสมอ ถ้าลืม RE จะรับข้อมูล
 * จากผู้ใช้ไม่ได้เลยแม้ว่าส่งออกได้ปกติ
 */
void uart_init(void)
{
    configure_uart_pins();

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

/*
 * ตั้งขา PA2 (TX) และ PA3 (RX) ให้เป็น Alternate Function AF7 (USART2)
 * เหตุผล: ลืมตั้งขาใดขาหนึ่งเป็น AF7 = ขานั้นใช้งานไม่ได้เลย (พบบ่อยที่สุด
 * คือลืม RX แล้วส่งได้แต่รับข้อมูลจากผู้ใช้ไม่ได้)
 */
static void configure_uart_pins(void)
{
    GPIOA->MODER &= ~(GPIO_MODER_MASK << (UART_TX_PIN * GPIO_MODER_PIN_BITS));
    GPIOA->MODER |= (GPIO_MODER_ALT_FUNC << (UART_TX_PIN * GPIO_MODER_PIN_BITS));

    GPIOA->MODER &= ~(GPIO_MODER_MASK << (UART_RX_PIN * GPIO_MODER_PIN_BITS));
    GPIOA->MODER |= (GPIO_MODER_ALT_FUNC << (UART_RX_PIN * GPIO_MODER_PIN_BITS));

    GPIOA->AFR[0] &= ~(GPIO_AFR_MASK << (UART_TX_PIN * GPIO_AFR_PIN_BITS));
    GPIOA->AFR[0] |= (UART_AF_USART2 << (UART_TX_PIN * GPIO_AFR_PIN_BITS));

    GPIOA->AFR[0] &= ~(GPIO_AFR_MASK << (UART_RX_PIN * GPIO_AFR_PIN_BITS));
    GPIOA->AFR[0] |= (UART_AF_USART2 << (UART_RX_PIN * GPIO_AFR_PIN_BITS));
}

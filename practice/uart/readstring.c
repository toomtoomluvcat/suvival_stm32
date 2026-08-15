#include <stdint.h>
#include "stm32f4xx.h"

/* --- Function prototypes: ประกาศไว้ก่อนเสมอ ให้ compiler รู้จัก type ล่วงหน้า --- */
static void clock_init(void);
static void led_init(void);
static void led_set(uint8_t state);
static void uart_init(void);
static uint8_t uart_receive_data(void);

int main(void)
{
    clock_init();
    led_init();
    uart_init();

    while (1)
    {
        uint8_t data;

        /* รอรับตัวอักษรจาก UART (polling แบบ blocking) */
        data = uart_receive_data();

        /* ถ้าได้ตัวอักษร '1' ให้ LED ติด นอกนั้นดับ */
        if (data == '1')
        {
            led_set(1u);
        }
        else
        {
            led_set(0u);
        }
    }

    return 0;
}

/***********************************************************************
 * @fn      - clock_init
 * @brief   - เปิด clock ของ GPIOA (LED + UART pins) และ USART2 (อยู่บัส APB1)
 ************************************************************************/
static void clock_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
}

/***********************************************************************
 * @fn      - led_init
 * @brief   - ตั้ง PA5 เป็น output สำหรับ LED
 ************************************************************************/
static void led_init(void)
{
    GPIOA->MODER &= ~(0b11 << (5 * 2));
    GPIOA->MODER |=  (0b01 << (5 * 2));
}

/***********************************************************************
 * @fn      - led_set
 * @brief   - สั่ง LED ติด (state=1) หรือดับ (state=0)
 * @param   - state : 1 = ติด, 0 = ดับ
 ************************************************************************/
static void led_set(uint8_t state)
{
    if (state == 1u)
    {
        GPIOA->ODR |= (1 << 5);
    }
    else
    {
        GPIOA->ODR &= ~(1 << 5);
    }
}

/***********************************************************************
 * @fn      - uart_init
 * @brief   - ตั้งขา PA2/PA3 เป็น Alternate Function 7 (USART2)
 *            และตั้งค่า USART2: baud rate, เปิด TX+RX, เปิดใช้งาน
 ************************************************************************/
static void uart_init(void)
{
    /* PA2 -> Alternate Function mode (10) */
    GPIOA->MODER &= ~(0b11 << (2 * 2));
    GPIOA->MODER |=  (0b10 << (2 * 2));

    /* เลือก AF7 = USART2 ผ่าน AFR[0] (พิน 0-7 ใช้ AFR[0]) */
    GPIOA->AFR[0] &= ~(0xFu << (2 * 4));
    GPIOA->AFR[0] |=  (7u << (2 * 4));

    USART2->BRR = 139;                            /* baud 115200 @ 16MHz */
    USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE); /* เปิดทั้ง transmitter และ receiver */
    USART2->CR1 |= USART_CR1_UE;                  /* เปิดใช้งาน USART */
}

/***********************************************************************
 * @fn      - uart_receive_data
 * @brief   - รอจนมีข้อมูลเข้ามา (RXNE) แล้วอ่านค่าออกจาก DR
 * @return  - ตัวอักษรที่รับมา (uint8_t)
 ************************************************************************/
static uint8_t uart_receive_data(void)
{
    while ((USART2->SR & USART_SR_RXNE) == 0u)
    {
        /* wait */
    }

    return (uint8_t)USART2->DR;
}

void EXTI3_IRQHandler(void) { }
void EXTI4_IRQHandler(void) { }
void ADC_IRQHandler(void) { }
void USART2_IRQHandler(void) { }
/*******************************************************************************
 * File Name    : main.c
 * Description  : โปรแกรมหลัก Exam3 (16 ส.ค.) - รับคำสั่งทาง UART 1 ตัวอักษร
 *                '0' = ปิด LED ทั้ง 3 + พิมพ์ "OK", '1' = เปิด LED ทั้ง 3 +
 *                พิมพ์ "OK", 't' = พิมพ์อุณหภูมิ, 'l' = พิมพ์ความสว่าง,
 *                ตัวอื่น = พิมพ์ "Invalid Command"
 * Date         : 2026-08-16
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#define STM32F411xE         /* บอก CMSIS ว่าใช้ชิปรุ่น STM32F411xE (Nucleo-F411RE) */
#include "stm32f4xx.h"
#include "uart.h"
#include "led_control.h"
#include "sensor.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/
 
/* Private define ------------------------------------------------------------*/
#define CMD_LED_OFF   ((uint8_t)'0')
#define CMD_LED_ON    ((uint8_t)'1')
#define CMD_TEMP      ((uint8_t)'t')
#define CMD_LIGHT     ((uint8_t)'l')

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/
static void clock_init(void);
static void handle_command(uint8_t command);

/* Private user code ------------------------------------------------------------*/

/* main() ------------------------------------------------------------*/
int main(void)
{
    uint8_t received_char;

    clock_init();
    uart_init();
    led_control_init();
    sensor_init();

    received_char = 0u;

    while (1)
    {
        /* uart_receive_char() เป็นแบบ blocking จะหยุดรอตรงนี้จนกว่าจะมี
         * ตัวอักษรเข้ามาจริง ๆ ทำให้ไม่ต้องเช็ค flag เองใน main loop */
        received_char = uart_receive_char();
        handle_command(received_char);
    }

    return 0;
}

/* Callback functions ------------------------------------------------------------*/

/* Private functions ------------------------------------------------------------*/

/*
 * เปิด clock ให้ครบทุก peripheral ที่ใช้: GPIOA (LED + UART + เซนเซอร์),
 * USART2 (รับ-ส่งคำสั่ง), ADC1 (อ่านอุณหภูมิ/ความสว่าง)
 */
static void clock_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}

/*
 * ตีความตัวอักษรที่รับมา แล้วเรียกฟังก์ชันของ module ที่เกี่ยวข้องให้ทำงาน
 * เหตุผลที่ใช้ switch-case: อ่านง่าย เห็นชัดว่าตัวอักษรไหนตรงกับคำสั่งไหน
 * และ MISRA บังคับให้ทุก case มี break และมี default เสมอ
 */
static void handle_command(uint8_t command)
{
    switch (command)
    {
        case CMD_LED_OFF:
            led_all_off();
            uart_send_string("\nOK");
            break;

        case CMD_LED_ON:
            led_all_on();
            uart_send_string("\nOK");
            break;

        case CMD_TEMP:
            uart_send_string("\nTemperature = ");
            uart_send_number(sensor_read_temperature_millidegc());
            uart_send_string("\nmillidegree Celcius\n");
            break;

        case CMD_LIGHT:
            uart_send_string("\nLight intensity = ");
            uart_send_number(sensor_read_light_lux());
            uart_send_string("\nLux\n");
            break;

        default:
            uart_send_string("Invalid Command\n");
            break;
    }
}

#include <stdint.h>
#include "stm32f4xx.h"

/* --- BEGIN AUTO PROTOTYPES (tools/gen_prototypes.py) --- */
static void clock_init(void);
static void uint16_to_string(uint16_t value, char *out);
static void uart_send_data(uint8_t data);
static void uart_send_string(const char *str);
static void uart_init(void);
static void adc_init(void);
static uint16_t adc_read(void);
static void led_init(void);
static void led_toggle(void);
static void delay_1s(void);
/* --- END AUTO PROTOTYPES --- */

/*อ่านค่า ADC จาก PA1 ทุก 1 วินาที แล้วส่งค่าที่อ่านได้ออกทาง UART 
(PA2=TX, PA3=RX, 115200 baud) เป็นข้อความตัวเลข 
(ห้ามใช้ sprintf ใช้ uint16_to_string() แบบที่เคยทำ)
ในขณะเดียวกัน ให้คอยเช็ค UART RX (แบบ non-blocking, ห้ามค้างรอ)
 ถ้ามีตัวอักษร '1' ส่งเข้ามา ให้ toggle LED ที่ PA5*/
int main()
{
    clock_init();
    led_init();
    adc_init();
    uart_init();
    while (1)
    {
         uint16_t adc_value = adc_read();
         char num_str[6];
        char msg[20];
        uint8_t i;
        uint8_t j = 0u;

        uint16_to_string(adc_value, num_str);

        /* ประกอบ "ADC = " + ตัวเลข + "\r\n" เอง (ไม่ใช้ sprintf) */
        const char *prefix = "ADC = ";
        for (i = 0u; prefix[i] != '\0'; i++)
        {
            msg[j] = prefix[i];
            j++;
        }
        for (i = 0u; num_str[i] != '\0'; i++)
        {
            msg[j] = num_str[i];
            j++;
        }
        msg[j] = '\r'; j++;
        msg[j] = '\n'; j++;
        msg[j] = '\0';

        uart_send_string(msg);

           /* เช็ค RX แบบ non-blocking: ไม่รอ ถ้ายังไม่มีข้อมูลก็ข้ามไปเลย */
        if ((USART2->SR & USART_SR_RXNE) != 0u)
        {
            uint8_t received = (uint8_t)USART2->DR;

            if (received == (uint8_t)'1')
            {
                led_toggle();
            }
            else
            {
                /* No action */
            }
        }
        else
        {
            /* No action: ยังไม่มีข้อมูลเข้ามา */
        }

        delay_1s();

    }
    
    return 0;
}


static void clock_init(void)
{
    /* เปิด Clock ให้ GPIOA
       ใช้สำหรับขา GPIO ของ Port A เช่น PA1, PA2, PA6 */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* เปิด Clock ให้ USART2
       USART2 อยู่บนบัส APB1
       จำเป็นเมื่อจะใช้งาน UART/USART2 */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* เปิด Clock ให้ ADC1
       ADC1 อยู่บนบัส APB2
       จำเป็นเมื่อจะใช้งาน ADC1 */
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}

/* แปลง uint16_t เป็น string เอง (ไม่ใช้ sprintf) */
static void uint16_to_string(uint16_t value, char *out)
{
    char temp[6];
    uint8_t idx = 0u;
    uint8_t i;

    if (value == 0u)
    {
        out[0] = '0';
        out[1] = '\0';
    }
    else
    {
        while (value > 0u)
        {
            temp[idx] = (char)('0' + (value % 10u));
            value = value / 10u;
            idx++;
        }
        for (i = 0u; i < idx; i++)
        {
            out[i] = temp[idx - 1u - i];
        }
        out[idx] = '\0';
    }
}


static void uart_send_data(uint8_t data)
{
    while ((USART2->SR & USART_SR_TXE) == 0u)
    {
        /* wait */
    }
    USART2->DR = data;
}

static void uart_send_string(const char *str)
{
    while (*str != '\0')
    {
        uart_send_data((uint8_t)*str);
        str++;
    }
}

static void uart_init(void)
{
    GPIOA->MODER &= ~(0b11 << (2u * 2u));
    GPIOA->MODER |=  (0b10 << (2u * 2u));
    GPIOA->AFR[0] &= ~(0xFu << (2u * 4u));
    GPIOA->AFR[0] |=  (7u << (2u * 4));
    GPIOA->MODER &= ~(0b11u << (3u * 2u));
    GPIOA->MODER |=  (0b10u << (3u * 2u));
    GPIOA->AFR[0] &= ~(0xFu << (3u * 4u));
    GPIOA->AFR[0] |=  (7u << (3u * 4u));
    USART2->BRR = 139;
    USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE);
    USART2->CR1 |= USART_CR1_UE;
}

static void adc_init(void)
{
    GPIOA->MODER &= ~GPIO_MODER_MODER1;    /* แก้: เคลียร์ขา 0 ให้ตรงกับที่ตั้งค่า */
    GPIOA->MODER |= (0b11 << (1 * 2));

    ADC1->SMPR2 |= ADC_SMPR2_SMP1;         /* แก้: SMP0 ตรงกับ channel 0 */
    ADC1->SQR1  &= ~ADC_SQR1_L;
    ADC1->SQR3  &= ~ADC_SQR3_SQ1;
    ADC1->SQR3  |= (1 << ADC_SQR3_SQ1_Pos);

    ADC1->CR2 |= ADC_CR2_ADON;
}

static uint16_t adc_read(void)
{
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while ((ADC1->SR & ADC_SR_EOC) == 0u)
    {
        /* wait */
    }
    return (uint16_t)ADC1->DR;
}

static void led_init(void)
{
    GPIOA->MODER &= ~(0b11 << (5 * 2));
    GPIOA->MODER |=  (0b01 << (5 * 2));
}

static void led_toggle(void)
{
        GPIOA->ODR ^= (1 << 5);
}
static void delay_1s(void)
{
    for (uint32_t i = 0u; i < 1333333u; i++)
    {
        /* wait */
    }
}

/* ==========================================================
   Lab 4.2 - Reading Temperature with NTC Thermistor (STM32F411RE)
   -----------------------------------------------------------
   PA0        : NTC voltage divider output -> ADC1_IN0
   USART2 TX  : PA2  (AF7)  -> connect to ST-LINK Virtual COM Port
   USART2 RX  : PA3  (AF7)

   Prints:  "Temperature = 25742 millidegree Celcius\n"  every 1000 ms
   ========================================================== */
#include <stdint.h>
#include <stdio.h>
#include <math.h>
/* NOTE: STM32F411xE is already defined by the project's build settings
   (-DSTM32F411xE). Do NOT #define it again here or GCC will warn
   about macro redefinition. */
#include "stm32f4xx.h"

#define VREF        3.3f
#define VCC         3.3f
#define ADC_MAXRES  4095.0f
#define RX          10000.0f
#define R0          10000.0f
#define T0          298.15f
#define BETA        3950.0f

char stringOut[50];

void vdg_UART_TxString(char strOut[]) {
    uint32_t i = 0;
    while (strOut[i] != '\0') {
        while (!(USART2->SR & USART_SR_TXE));   // wait until Tx data register empty
        USART2->DR = strOut[i];
        i++;
    }
    while (!(USART2->SR & USART_SR_TC));        // wait until the full frame is sent
}

int main(void) {
    /* --- Setup peripheral clock --- */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    /* --- Setup GPIO PA2, PA3 for USART2 (Alternate Function 7) --- */
    GPIOA->MODER  &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
    GPIOA->MODER  |=  (0b10 << GPIO_MODER_MODER2_Pos) | (0b10 << GPIO_MODER_MODER3_Pos);
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);
    GPIOA->AFR[0] |=  (7 << GPIO_AFRL_AFSEL2_Pos) | (7 << GPIO_AFRL_AFSEL3_Pos);

    /* --- USART2 Setup: 115200 baud, 8N1 (PCLK1 assumed = 16 MHz HSI) ---
       USARTDIV = PCLK1 / (16 x baud) = 16,000,000 / (16 x 115200) = 8.68
       Mantissa = 8 (integer part)
       Fraction = 0.68 x 16 = ~11 (rounded, occupies the 4 low bits)
       BRR = (Mantissa << 4) | Fraction = (8 << 4) | 11 */
    USART2->BRR  = (8 << 4) | 11;
    USART2->CR1 |= USART_CR1_TE | USART_CR1_UE;

    /* --- ADC channel 0 setup (PA0) --- */
    GPIOA->MODER &= ~(GPIO_MODER_MODER0);
    GPIOA->MODER |= (0b11 << GPIO_MODER_MODER0_Pos);  // <-- Set PA0 mode to analog here
    ADC1->CR2    |= ADC_CR2_ADON;
    ADC1->SMPR2  |= ADC_SMPR2_SMP0;
    ADC1->SQR1   &= ~(ADC_SQR1_L);
    ADC1->SQR1   |= (0 << ADC_SQR1_L_Pos);
    ADC1->SQR3   &= ~(ADC_SQR3_SQ1);
    ADC1->SQR3   |= (0 << ADC_SQR3_SQ1_Pos);          // <-- Set conversion sequence to channel 0 here

    /* --- Enable FPU --- */
    SCB->CPACR |= (0b1111 << 20);
    __asm volatile("dsb");
    __asm volatile("isb");

    while (1) {
        ADC1->CR2 |= ADC_CR2_SWSTART;
        while ((ADC1->SR & ADC_SR_EOC) == 0);

        float adc_voltage = (ADC1->DR * VREF) / ADC_MAXRES;      // <-- volts on PA0
        float r_ntc       = RX * adc_voltage / (VCC - adc_voltage); // <-- NTC resistance
        float temperature = (BETA * T0) / (T0 * log(r_ntc / R0) + BETA) - 273.15f; // <-- Beta equation (Kelvin -> Celsius)

        /* --- USART2 String Tx --- */
        sprintf(stringOut, "Temperature = %ld millidegree Celcius\n",
                (int32_t)(temperature * 1000.0f));
        vdg_UART_TxString(stringOut);

        for (volatile uint32_t i = 0; i < 2000000; i++); // <- Put delay here (adjust for ~1000 ms based on actual clock)
    }
}
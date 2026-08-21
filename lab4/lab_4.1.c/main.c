/* ==========================================================
   Lab 4.1 - Reading Potentiometer (STM32F411RE)
   -----------------------------------------------------------
   PA4  : Potentiometer wiper  -> ADC1_IN4
   PA5  : Onboard LED (output)

   Behavior:
     - PA5 LED turns ON  when the potentiometer is rotated
       counter-clockwise (assumed = lower ADC reading)
     - PA5 LED turns OFF when rotated clockwise
   ========================================================== */
#include <stdint.h>
#define STM32F411xE
#include "stm32f4xx.h"

int main(void) {
    /* --- Setup peripheral clock --- */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;               // <-- Enable ADC1 clock

    /* --- Setup GPIO PA5 (LED) as general purpose output --- */
    GPIOA->MODER &= ~(GPIO_MODER_MODER5);
    GPIOA->MODER |=  (0b01 << GPIO_MODER_MODER5_Pos); // <- Put your code to setup PA5 LED here

    /* --- Step 1: Setup PA4 as Analog input (ADC1_4) --- */
    GPIOA->MODER &= ~(GPIO_MODER_MODER4);
    GPIOA->MODER |=  (0b11 << GPIO_MODER_MODER4_Pos); // <-- Set PA4 mode to analog here

    /* --- Step 2: Enable ADC peripheral --- */
    ADC1->CR2 |= ADC_CR2_ADON;                        // <-- Enable ADC peripheral here

    /* --- Step 3: Configure the sample time of channel 4 --- */
    ADC1->SMPR2 |= ADC_SMPR2_SMP4;                    // 111 = 480 cycles (maximum)

    /* --- Step 4.1: Configure the total number of conversions --- */
    ADC1->SQR1 &= ~(ADC_SQR1_L);
    ADC1->SQR1 |=  (0 << ADC_SQR1_L_Pos);             // <-- Set the total number of conversion here
                                                       //     (L = 0000b -> 1 conversion in sequence)

    /* --- Step 4.2: Configure the conversion sequences --- */
    ADC1->SQR3 &= ~(ADC_SQR3_SQ1);
    ADC1->SQR3 |=  (4 << ADC_SQR3_SQ1_Pos);           // <-- Set conversion sequence to channel 4 here

    while (1) {
        /* --- Step 5.1: Start ADC Conversion --- */
        ADC1->CR2 |= ADC_CR2_SWSTART;                 // <-- Write the code to start ADC conversion here

        /* --- Step 5.2: Wait until the conversion is completed --- */
        while ((ADC1->SR & ADC_SR_EOC) == 0);         // <-- Wait until the conversion is completed here

        /* --- Criteria: PA5 LED ON when pot is turned counter-clockwise ---
           NOTE: assumes CCW rotation -> lower ADC value.
           If your board behaves oppositely, change '<' to '>'. */
        if (ADC1->DR < 2048) {                        // <-- Write the criteria based on requirement here
            GPIOA->ODR |= GPIO_ODR_OD5;
        } else {
            GPIOA->ODR &= ~(GPIO_ODR_OD5);
        }

        for (volatile uint32_t i = 0; i < 200000; i++); // <- Put delay here (simple busy-wait)
    }
}
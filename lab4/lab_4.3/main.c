#include <stdint.h>
#include <stdio.h>
#include <math.h>
#define STM32F411xE
#include "stm32f4xx.h"

#define VREF                3.3f
#define VCC                 3.3f
#define ADC_MAXRES          4095.0f
#define RX                  10000.0f
#define SLOPE               -0.6875f
#define OFFSET              5.1276f

char stringOut[50];

void vdg_UART_TxString(char strOut[]) {
	uint8_t i = 0;
	while (strOut[i] != '\0') {
		while ((USART2->SR & USART_SR_TXE) == 0);
		USART2->DR = strOut[i];
		i++;
	}
}

int main(void) {
	/* --- Setup peripheral clock --- */
	RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR  |= RCC_APB1ENR_USART2EN;
	RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;

	/* --- Setup GPIO PA2, PA3 --- */
	GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
	GPIOA->MODER |= (0b10 << GPIO_MODER_MODER2_Pos) | (0b10 << GPIO_MODER_MODER3_Pos);
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);
	GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL2_Pos) | (7 << GPIO_AFRL_AFSEL3_Pos);

    /* --- Setup GPIO PA5 as output (LED) --- */
GPIOA->MODER &= ~(GPIO_MODER_MODER5);
GPIOA->MODER |= (0b01 << GPIO_MODER_MODER5_Pos);

	/* --- USART2 Setup --- */
	USART2->BRR = (8 << 4) | 11;
	USART2->CR1 |= USART_CR1_TE | USART_CR1_UE;

	/* --- ADC channel 0 setup --- */
	GPIOA->MODER &= ~(GPIO_MODER_MODER1);
	GPIOA->MODER |= (0b11 << GPIO_MODER_MODER1_Pos);
	ADC1->CR2 |= ADC_CR2_ADON;
	ADC1->SMPR2 |= ADC_SMPR2_SMP1;
	ADC1->SQR1 &= ~(ADC_SQR1_L);
	ADC1->SQR3 &= ~(ADC_SQR3_SQ1);
	ADC1->SQR3 |= (1 << ADC_SQR3_SQ1_Pos);

	/* --- Enable FPU --- */
	SCB->CPACR |= (0b1111 << 20);
	__asm volatile("dsb");
	__asm volatile("isb");

	while (1) {
		ADC1->CR2 |= ADC_CR2_SWSTART;
		while((ADC1->SR & ADC_SR_EOC) == 0);

		float adc_voltage = (ADC1->DR * VREF) / ADC_MAXRES;
		float r_ldr = RX * adc_voltage / (VREF - adc_voltage);
		float lightintensity = powf(10.0f, (log10f(r_ldr) - OFFSET) / SLOPE);   // <-- Put your code to calculate light intensity using the formula

		/* --- USART2 String Tx --- */
		sprintf(stringOut, "Light intensity = %d Lux\n", (uint16_t)lightintensity);   // <-- Put your formatted string here
		vdg_UART_TxString(stringOut);

		if (lightintensity < 60) {                                  // <-- Write the criteria based on requirement here
			GPIOA->ODR |= GPIO_ODR_OD5;
		} else {
			GPIOA->ODR &= ~(GPIO_ODR_OD5);
		}

		for (volatile uint32_t i = 0; i < 4000000; i++);            // <- Put delay here
	}
}
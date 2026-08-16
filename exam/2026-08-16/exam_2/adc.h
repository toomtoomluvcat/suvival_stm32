/*******************************************************************************
 * File Name    : adc.h
 * Description  : Header สำหรับโมดูลอ่านค่า potentiometer ผ่าน ADC1 (polling)
 * Date         : 2026-08-16
 ******************************************************************************/
#ifndef ADC_H
#define ADC_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported typedef/enum/struct/union -----------------------------------------*/

/* Exported define/macro/constants --------------------------------------------*/
#define ADC_MAX_VALUE   (4095u)   /* ค่าสูงสุดของ ADC 12 บิต (0-4095) */

/* Exported variables -----------------------------------------------------------*/

/* Exported function prototypes ------------------------------------------------*/
void adc_init(void);
uint16_t adc_read(void);

#endif /* ADC_H */

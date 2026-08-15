/*******************************************************************************
 * File Name    : adc.h
 * Description  : Header สำหรับโมดูลอ่านค่า potentiometer ผ่าน ADC1
 *                แบบ continuous conversion + EOC interrupt
 * Date         : 2026-08-15
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
uint8_t adc_new_data_available(void);
uint16_t adc_get_latest_value(void);

#endif /* ADC_H */

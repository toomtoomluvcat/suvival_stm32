/*******************************************************************************
 * File Name    : led_pair.h
 * Description  : Header สำหรับโมดูลควบคุม LED เป็นคู่ (คู่กลาง / คู่นอก)
 * Date         : 2026-08-16
 ******************************************************************************/
#ifndef LED_PAIR_H
#define LED_PAIR_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported typedef/enum/struct/union -----------------------------------------*/

/* Exported define/macro/constants --------------------------------------------*/

/* Exported variables -----------------------------------------------------------*/

/* Exported function prototypes ------------------------------------------------*/
void led_pair_init(void);
void led_pair_update(uint16_t adc_value);

#endif /* LED_PAIR_H */

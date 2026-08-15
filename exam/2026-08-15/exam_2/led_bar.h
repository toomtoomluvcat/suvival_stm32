/*******************************************************************************
 * File Name    : led_bar.h
 * Description  : Header สำหรับโมดูลควบคุม LED ทั้ง 4 ดวงแบบ "บาร์" ติดทีละดวง
 * Date         : 2026-08-15
 ******************************************************************************/
#ifndef LED_BAR_H
#define LED_BAR_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported typedef/enum/struct/union -----------------------------------------*/

/* Exported define/macro/constants --------------------------------------------*/

/* Exported variables -----------------------------------------------------------*/

/* Exported function prototypes ------------------------------------------------*/
void led_bar_init(void);
void led_bar_update(uint16_t adc_value);

#endif /* LED_BAR_H */

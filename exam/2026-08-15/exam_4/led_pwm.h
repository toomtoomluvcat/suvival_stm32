/*******************************************************************************
 * File Name    : led_pwm.h
 * Description  : Header สำหรับโมดูลควบคุมความสว่าง LED ทั้ง 4 ดวงด้วย PWM
 * Date         : 2026-08-15
 ******************************************************************************/
#ifndef LED_PWM_H
#define LED_PWM_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported typedef/enum/struct/union -----------------------------------------*/

/* Exported define/macro/constants --------------------------------------------*/

/* Exported variables -----------------------------------------------------------*/

/* Exported function prototypes ------------------------------------------------*/
void led_pwm_init(void);
void led_pwm_update(uint16_t adc_value);

#endif /* LED_PWM_H */

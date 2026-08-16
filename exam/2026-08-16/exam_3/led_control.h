/*******************************************************************************
 * File Name    : led_control.h
 * Description  : Header สำหรับโมดูลควบคุม LED สีน้ำเงิน/แดง/เหลือง (เปิด-ปิดพร้อมกัน)
 * Date         : 2026-08-16
 ******************************************************************************/
#ifndef LED_CONTROL_H
#define LED_CONTROL_H

/* Includes ------------------------------------------------------------------*/

/* Exported typedef/enum/struct/union -----------------------------------------*/

/* Exported define/macro/constants --------------------------------------------*/

/* Exported variables -----------------------------------------------------------*/

/* Exported function prototypes ------------------------------------------------*/
void led_control_init(void);
void led_all_on(void);
void led_all_off(void);

#endif /* LED_CONTROL_H */

/*******************************************************************************
 * File Name    : button.h
 * Description  : Header สำหรับโมดูลอ่านปุ่มกดแบบ polling ธรรมดา (เช็คสถานะ
 *                ปัจจุบันของขาโดยตรง ไม่ทำ edge-detect เพราะโจทย์ต้องการ
 *                ให้กดค้างแล้วค่าขยับต่อเนื่อง)
 * Date         : 2026-08-16
 ******************************************************************************/
#ifndef BUTTON_H
#define BUTTON_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported typedef/enum/struct/union -----------------------------------------*/

/* Exported define/macro/constants --------------------------------------------*/

/* Exported variables -----------------------------------------------------------*/

/* Exported function prototypes ------------------------------------------------*/
void button_init(void);
uint8_t button_increase_is_pressed(void);
uint8_t button_decrease_is_pressed(void);
uint8_t button_reset_is_pressed(void);

#endif /* BUTTON_H */

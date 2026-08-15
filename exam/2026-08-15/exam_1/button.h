/*******************************************************************************
 * File Name    : button.h
 * Description  : Header สำหรับโมดูลอ่านปุ่มกดแบบ polling + edge-detect
 * Date         : 2026-08-15
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
uint8_t button_increase_pressed(void);
uint8_t button_decrease_pressed(void);
uint8_t button_reset_pressed(void);

#endif /* BUTTON_H */

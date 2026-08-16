/*******************************************************************************
 * File Name    : seven_seg.h
 * Description  : Header สำหรับโมดูลควบคุม 7-segment display (BCD 4 บิต)
 * Date         : 2026-08-16
 ******************************************************************************/
#ifndef SEVEN_SEG_H
#define SEVEN_SEG_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported typedef/enum/struct/union -----------------------------------------*/

/* Exported define/macro/constants --------------------------------------------*/

/* Exported variables -----------------------------------------------------------*/

/* Exported function prototypes ------------------------------------------------*/
void seven_seg_init(void);
void seven_seg_display(uint8_t digit);

#endif /* SEVEN_SEG_H */

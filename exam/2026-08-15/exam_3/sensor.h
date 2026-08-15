/*******************************************************************************
 * File Name    : sensor.h
 * Description  : Header สำหรับโมดูลอ่านค่าอุณหภูมิและความสว่างผ่าน ADC1
 * Date         : 2026-08-15
 ******************************************************************************/
#ifndef SENSOR_H
#define SENSOR_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported typedef/enum/struct/union -----------------------------------------*/

/* Exported define/macro/constants --------------------------------------------*/

/* Exported variables -----------------------------------------------------------*/

/* Exported function prototypes ------------------------------------------------*/
void sensor_init(void);
uint32_t sensor_read_temperature_millidegc(void);
uint32_t sensor_read_light_lux(void);

#endif /* SENSOR_H */

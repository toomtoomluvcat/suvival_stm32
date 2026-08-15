#ifndef LED_THRESHOLD_H
#define LED_THRESHOLD_H

#include <stdint.h>

void led_init(void);
void led_set(uint8_t pin, uint8_t state);

#endif /* LED_THRESHOLD_H */
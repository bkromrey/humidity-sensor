#ifndef __LED_ARRAY_H_
#define __LED_ARRAY_H_

// Pico SDK
#include "pico/stdlib.h"
#include <stdint.h>

// Shared Functions
void LED_Array_Init(const uint32_t *led_pins, uint32_t pin_number);
void Display_LED_Array(uint32_t value);

#endif
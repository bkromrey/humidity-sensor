#ifndef __LED_ARRAY_H_
#define __LED_ARRAY_H_

// Pico SDK
#include "pico/stdlib.h"

void LED_Array_Init(const uint *led_pins, uint pin_number);
void Display_LED_Array(uint value);

#endif
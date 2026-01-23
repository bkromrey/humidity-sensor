#ifndef __BUTTONS_H_
#define __BUTTONS_H_

// Standard Library
#include "stdbool.h"

// Pico SDK
#include "pico/stdlib.h"

// Global Data Types
typedef struct {
    uint32_t button_pin;
    volatile uint32_t disabled_count;
    uint32_t reset_value;
    volatile bool flag;
} Button;

void Button_Init(Button *button_array, uint32_t num_buttons);
void GPIO_Interrupt_Init(void(*handler)(uint, uint32_t));

#endif
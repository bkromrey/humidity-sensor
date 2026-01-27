#ifndef __BUTTONS_H__
#define __BUTTONS_H__

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
    void(*button_handler)(void);
} Button;

void Button_Init(Button *button_array, uint32_t num_buttons);
void GPIO_Interrupt_Init(void(*handler)(uint, uint32_t));

#endif
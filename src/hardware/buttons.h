#ifndef __BUTTONS_H_
#define __BUTTONS_H_

#include "pico/stdlib.h"

// Global Data Types
typedef struct {
    uint button_pin;
    uint disabled_count;
    uint reset_value;
    bool flag;
} Button;

void Button_Init(Button *button_array, uint num_buttons);
void GPIO_Interrupt_Init(void(*handler)(uint, uint32_t));

#endif
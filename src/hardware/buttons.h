#ifndef __BUTTONS_H_
#define __BUTTONS_H_

#include "pico/stdlib.h"

void Button_Init(uint *buttons, uint button_number);
void GPIO_Interrupt_Init(void(*handler)(uint, uint32_t));

#endif
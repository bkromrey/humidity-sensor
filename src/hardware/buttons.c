#include "buttons.h"
#include "hardware/gpio.h"

// Globals
static Button *Button_Array_Local;
static uint Num_Buttons_Local = 0;

// initialize each gpio pin as a pull-up switch
void GPIO_Init(uint button_pin){
    gpio_init(button_pin);
    gpio_set_dir(button_pin, GPIO_IN);
    gpio_pull_up(button_pin);
}

void Button_Init(Button *button_array, uint num_buttons){
    // set Globals
    Button_Array_Local = button_array;
    Num_Buttons_Local = num_buttons;

    for(Button *btn = Button_Array_Local; btn < Button_Array_Local + Num_Buttons_Local ;btn++){
        GPIO_Init(btn->button_pin);
    }
}

void GPIO_Interrupt_Init(void(*handler)(uint, uint32_t)){
    for(Button *btn = Button_Array_Local; btn < Button_Array_Local + Num_Buttons_Local ;btn++){
        if (btn == Button_Array_Local)
            gpio_set_irq_enabled_with_callback(btn->button_pin, GPIO_IRQ_EDGE_FALL, true, handler);
        else
            gpio_set_irq_enabled(btn->button_pin, GPIO_IRQ_EDGE_FALL, true);
    }
}
#include "buttons.h"
#include "hardware/gpio.h"

// Globals
static const Button *Button_Array_Local;
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

    for (uint i = 0; i < Num_Buttons_Local; i++){
        GPIO_Init(Button_Array_Local[i].button_pin);
    }
}

void GPIO_Interrupt_Init(void(*handler)(uint, uint32_t)){
    for (uint i = 0; i < Num_Buttons_Local; i ++){
        if (i == 0)
            gpio_set_irq_enabled_with_callback(Button_Array_Local[i].button_pin, GPIO_IRQ_EDGE_FALL, true, handler);
        else
            gpio_set_irq_enabled(Button_Array_Local[i].button_pin, GPIO_IRQ_EDGE_FALL, true);
    }
}
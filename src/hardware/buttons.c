#include "buttons.h"
#include "hardware/gpio.h"

// Globals
static const uint *Buttons;
static uint Button_Number = 0;

// initialize each gpio pin as a pull-up switch
void GPIO_Init(uint button_pin){
    gpio_init(button_pin);
    gpio_set_dir(button_pin, GPIO_IN);
    gpio_pull_up(button_pin);
}

void Button_Init(uint *buttons, uint button_number){
    // set Globals
    Buttons = buttons;
    Button_Number = button_number;

    for (uint i = 0; i < Button_Number; i++){
        GPIO_Init(Buttons[i]);
    }
}

void GPIO_Interrupt_Init(void(*handler)(uint, uint32_t)){
    for (uint i = 0; i < Button_Number; i ++){
        if (i == 0)
            gpio_set_irq_enabled_with_callback(Buttons[i], GPIO_IRQ_EDGE_FALL, true, handler);
        else
            gpio_set_irq_enabled(Buttons[i], GPIO_IRQ_EDGE_FALL, true);
    }
}
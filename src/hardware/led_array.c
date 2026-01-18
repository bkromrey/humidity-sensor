
#include "led_array.h"
static uint Pin_Number = 0;
static const uint *LED_Pins;

void LED_Array_Init(const uint *led_pins, uint pin_number){
  
  // Initialize Globals
  LED_Pins = led_pins;
  Pin_Number = pin_number;

  // Initialize Pins using Pico SDK
  for (uint i = 0; i < Pin_Number; i++){
     gpio_init(LED_Pins[i]);
     gpio_set_dir(LED_Pins[i], GPIO_OUT);
  }
}


// Write value [0, Pin_Number] to LED Array
void Display_LED_Array(uint end_index){

  // led on mask
  uint32_t led_on_mask = 0;
  uint32_t led_off_mask = 0;

  for(uint i = 0; i < Pin_Number; i++){
    if (i <= end_index)
      led_on_mask |= (1u << LED_Pins[i]);
    else
      led_off_mask |= (1u << LED_Pins[i]);
  }

  gpio_set_mask(led_on_mask);
  gpio_clr_mask(led_off_mask);
}
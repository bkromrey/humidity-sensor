#include "led_array.h"

// File scope globals
static uint32_t Pin_Number = 0;
static const uint32_t *LED_Pins;

/**
 * Initializes GPIO pins for LED array
 */
void LED_Array_Init(const uint32_t *led_pins, uint32_t pin_number){
  
  // Initialize Globals
  LED_Pins = led_pins;
  Pin_Number = pin_number;

  // Initialize Pins using Pico SDK
  for (uint32_t i = 0; i < Pin_Number; i++){
     gpio_init(LED_Pins[i]);
     gpio_set_dir(LED_Pins[i], GPIO_OUT);
  }
}

/**
 * Writes [0, Pin_Number] to LED Array
 * 0 - no LEDS
 * Pin_Number, all LEDs
 */
void Display_LED_Array(uint32_t end_index){

  // led on mask
  uint32_t led_on_mask = 0;
  uint32_t led_off_mask = 0;

  for(uint32_t i = 0; i < Pin_Number; i++){
    if (i < end_index)
      led_on_mask |= (1u << LED_Pins[i]);
    else
      led_off_mask |= (1u << LED_Pins[i]);
  }

  gpio_set_mask(led_on_mask);
  gpio_clr_mask(led_off_mask);
}

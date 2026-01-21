// Standard Library
#include <stdbool.h>
#include <stdio.h>

// Pico SDK
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/gpio.h"
#include "pico/time.h"

// User Modules
#include "hardware/led_array.h"
#include "hardware/buttons.h"

// Debug Mode - enable or disable bc printf to UART is slow
#define DEBUG 0

// Buttons
#define BUTTON_DEBOUNCE 100 // ms
#define NUM_BUTTONS 3
#define BUTTON_1 16
#define BUTTON_2 17
#define BUTTON_3 18

Button Button_Array[NUM_BUTTONS] = {
  {BUTTON_1, 0, BUTTON_DEBOUNCE, false},
  {BUTTON_2, 0, BUTTON_DEBOUNCE, false},
  {BUTTON_3, 0, BUTTON_DEBOUNCE, false},
};

// LED Array
#define LED_LENGTH 6
#define LED_PIN_0 10
#define LED_PIN_1 11
#define LED_PIN_2 12
#define LED_PIN_3 13
#define LED_PIN_4 14
#define LED_PIN_5 15
uint Led_Pins[LED_LENGTH] = {LED_PIN_0, LED_PIN_1, LED_PIN_2, LED_PIN_3, LED_PIN_4, LED_PIN_5};

// LCD I2C
#define LCD_I2C_SDA 0
#define LCD_I2C_SCL 2

// Humidity Sensor I2C
#define SENSOR_I2C_SDA 4
#define SENSOR_I2C_SCL 5

// ADC Pin
#define PHOTORESISTOR_ADC 26

// Test Globals
uint LED_Value;

bool system_timer_callback(struct repeating_timer *t){
  // decrement buttons disabled count
  for(Button *btn = Button_Array; btn < Button_Array + NUM_BUTTONS ;btn++){
    if(btn->disabled_count)
      btn->disabled_count--;
  }
  return true;
}

void GPIO_Handler(uint gpio, uint32_t event_mask){
  for(Button *btn = Button_Array; btn < Button_Array + NUM_BUTTONS ;btn++){
    if(btn->button_pin == gpio && btn->disabled_count == 0){
      btn->flag = true;
      btn->disabled_count = btn->reset_value;
    }
  }
}

void Button_Logic(void){
  for(Button *btn = Button_Array; btn < Button_Array + NUM_BUTTONS ;btn++){
    if (btn->flag){
      switch (btn->button_pin){
        case BUTTON_1:
          if (LED_Value > 0)
            LED_Value--;
          break;
        case BUTTON_2:
          if (LED_Value < LED_LENGTH)
            LED_Value++;
          break;
        case BUTTON_3:
          LED_Value = 0;
          break;
      }
      btn->flag = false;
    }
  }
}

int main() {
  // Needed for picotool
  stdio_init_all();

  // System Timer
  struct repeating_timer timer;
  add_repeating_timer_ms(-1, system_timer_callback, NULL, &timer);

  // Buttons
  Button_Init(Button_Array, NUM_BUTTONS);
  GPIO_Interrupt_Init(GPIO_Handler);

  // LED Array
  LED_Array_Init(Led_Pins, LED_LENGTH);

  while (true) {
    Button_Logic();

    // printf for UART debugging only if debug mode enabled
    #if DEBUG
      printf("LED_Value is: %d\r\n", LED_Value);
    #endif

    Display_LED_Array(LED_Value);
  }

  return 0;
}

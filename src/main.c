// Standard Library
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

// Pico SDK
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/time.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"

// User Modules
#include "hardware/led_array.h"
#include "hardware/buttons.h"
#include "data_flow/data_flow.h" // data types shared between main and core1
#include "core1/core1.h"

// Debug Mode - enable or disable bc printf to UART is slow
#define DEBUG 0

// Buttons
#define BUTTON_DEBOUNCE 100 // ms
#define NUM_BUTTONS 3
#define BUTTON_1 16
#define BUTTON_2 17
#define BUTTON_3 18

// Button Handler Prtotypes
void Button_1_Handler(void);
void Button_2_Handler(void);
void Button_3_Handler(void);

Button Button_Array[NUM_BUTTONS] = {
  {BUTTON_1, 0, BUTTON_DEBOUNCE, false, Button_1_Handler},
  {BUTTON_2, 0, BUTTON_DEBOUNCE, false, Button_2_Handler},
  {BUTTON_3, 0, BUTTON_DEBOUNCE, false, Button_3_Handler},
};

// LED Array
#define LED_LENGTH 6
#define LED_PIN_0 10
#define LED_PIN_1 11
#define LED_PIN_2 12
#define LED_PIN_3 13
#define LED_PIN_4 14
#define LED_PIN_5 15
uint32_t Led_Pins[LED_LENGTH] = {LED_PIN_0, LED_PIN_1, LED_PIN_2, LED_PIN_3, LED_PIN_4, LED_PIN_5};

// ADC Conversion
#define ADC_MAX 3200
#define ADC_MIN 100

// LCD I2C
#define LCD_I2C_SDA 0
#define LCD_I2C_SCL 2

// Humidity Sensor I2C
#define SENSOR_I2C_SDA 4
#define SENSOR_I2C_SCL 5

// Test Globals
uint32_t LED_Value = 0;

void Button_1_Handler(void){
  if(LED_Value > 0)
    LED_Value--;
}

void Button_2_Handler(void){
  if(LED_Value < LED_LENGTH)
    LED_Value++;  
}

void Button_3_Handler(void){
  LED_Value = 0;  
}


bool system_timer_callback(struct repeating_timer *t){
  // protect critical section
  uint32_t status = save_and_disable_interrupts();
  
  // decrement buttons disabled count
  for(Button *btn = Button_Array; btn < Button_Array + NUM_BUTTONS ;btn++){
    if(btn->disabled_count)
      btn->disabled_count--;
  }

  restore_interrupts(status);
  return true;
}

void GPIO_Handler(uint gpio, uint32_t event_mask){
  for(Button *btn = Button_Array; btn < Button_Array + NUM_BUTTONS ;btn++){
    if(btn->button_pin == gpio && btn->disabled_count == 0){  // if this button is the pin that's been pressed and it's not disabled
      btn->flag = true;                                       // set flag to true, pressed
      btn->disabled_count = btn->reset_value;                 // resetd disabled counter 
    }
  }
}

void Button_Logic(void){
  for(Button *btn = Button_Array; btn < Button_Array + NUM_BUTTONS ;btn++){
    // handle race condition
    uint32_t status = save_and_disable_interrupts();

    // Save State
    bool flag_local = btn->flag;
    // Consume Flag
    btn->flag = false; // set flag to not pressed, system_timer_callback handles the delay decrements, GPIO_Handler resets this value
    restore_interrupts(status);

    // button logic
    if (flag_local)
      btn->button_handler();
  }
}

void Process_Data(void){
  while(Data_Ring_Buffer.head != Data_Ring_Buffer.tail){
    Payload_Data data_copy = Data_Ring_Buffer.buffer[Data_Ring_Buffer.tail]; // index the buffer with the tail value

    // Increment tail
    Data_Ring_Buffer.tail = (Data_Ring_Buffer.tail + 1) % DATA_BUFFER_SIZE;

    // Do something with the data
    printf("Index: %d\r\n",Data_Ring_Buffer.tail);
    printf("ADC Data: %d\r\n",data_copy.ADC_Data);
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

  // Launch Core 1
  multicore_launch_core1(Core_1_Entry);

  while (true) {
    Button_Logic();

    // printf for UART debugging only if debug mode enabled
    #if DEBUG
      static uint32_t led_value_old = 0;
      if (LED_Value != led_value_old){
        printf("LED_Value is: %d\r\n", LED_Value);
        led_value_old = LED_Value;
      }
    #endif
    
    Process_Data();

    Display_LED_Array(LED_Value);
  }

  return 0;
}

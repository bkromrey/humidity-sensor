// Standard Library
#include <stdbool.h>
#include <stdio.h>

// Pico SDK
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/gpio.h"

// User Modules
#include "hardware/led_array.h"
#include "hardware/buttons.h"

// Debug Mode - enable or disable bc printf to UART is slow
#define DEBUG 0

// Buttons
#define BUTTON_DEBOUNCE 100 // Units
#define BUTTONS_LENGTH 3
#define BUTTON_1 16
#define BUTTON_2 17
#define BUTTON_3 18
uint Buttons[BUTTONS_LENGTH] = {BUTTON_1, BUTTON_2, BUTTON_3};

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

// Buttons
volatile uint Button_1_Disabled = 0;
volatile uint Button_2_Disabled = 0;
volatile uint Button_3_Disabled = 0;

// Output Flag
volatile uint Output_Flag = 0;
const uint Output_Screens = 2;

// Test 
uint LED_Value = 0;
volatile bool Increment = false;
volatile bool Decrement = false;
volatile bool Set_Zero = false;

void GPIO_Handler(uint gpio, uint32_t event_mask){
  if (gpio == BUTTON_1 && !Button_1_Disabled){
    Decrement = true;
    Button_1_Disabled = BUTTON_DEBOUNCE;
  }
  if (gpio == BUTTON_2 && !Button_2_Disabled){
    Increment = true;
    Button_2_Disabled = BUTTON_DEBOUNCE;
  }
  if (gpio == BUTTON_3 && !Button_3_Disabled){
    Set_Zero = true;
    Button_3_Disabled = BUTTON_DEBOUNCE;
  }
}

int main() {
  // Needed for picotool
  stdio_init_all();

  // System Timer

  // Buttons
  Button_Init(Buttons, BUTTONS_LENGTH);
  GPIO_Interrupt_Init(GPIO_Handler);

  // LED Array
  LED_Array_Init(Led_Pins, LED_LENGTH);

  while (true) {
    if (Increment && LED_Value < LED_LENGTH){
      LED_Value++;
      Increment = false;
    }
    if (Decrement && LED_Value > 0){
      LED_Value--;
      Decrement = false;
    }
    if (Set_Zero){
      LED_Value = 0;
      Set_Zero = false;
    }
  
    // printf for UART debugging only if debug mode enabled
    #if DEBUG
      printf("LED_Value is: %d\r\n", LED_Value);
    #endif

    Display_LED_Array(LED_Value);
  }

  return 0;
}

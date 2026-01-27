// Standard Library
#include <stdbool.h>
#include <stdio.h>

// Pico SDK
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/gpio.h"

// User Modules
#include "hardware/led_array.h"
#include "hardware/dht20_sensor.h"

// Debug Mode - enable or disable bc printf to UART is slow
#define DEBUG false

// Buttons
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

// Humidity Sensor I2C - identify based on GPIO pin #, not actual pin #
#define SENSOR_I2C_SDA 4
#define SENSOR_I2C_SCL 5

// ADC Pin
#define PHOTORESISTOR_ADC 26

// Output Flag
volatile uint Output_Flag = 0;
const uint Output_Screens = 2;

// Test 
uint LED_Value = 0;
volatile bool Increment = false;
volatile bool Decrement = false;
volatile bool Set_Zero = false;

void GPIO_Handler(uint gpio, uint32_t event_mask){
  switch (gpio){
    case BUTTON_1:
      Decrement = true;
      break;
    case BUTTON_2:
      Increment = true;
      break;
    case BUTTON_3:
      Set_Zero = true;
      break;
  }
}

void Button_Init(uint button_pin){
  gpio_init(button_pin);
  gpio_set_dir(button_pin, GPIO_IN);
  gpio_pull_up(button_pin);
}

int main() {
  // Needed for picotool
  stdio_init_all();

  // Buttons
  for (uint i = 0; i < BUTTONS_LENGTH; i++){
    Button_Init(Buttons[i]);
  }
  gpio_set_irq_enabled_with_callback(BUTTON_1, GPIO_IRQ_EDGE_FALL, true, &GPIO_Handler);
  gpio_set_irq_enabled(BUTTON_2, GPIO_IRQ_EDGE_FALL, true);
  gpio_set_irq_enabled(BUTTON_3, GPIO_IRQ_EDGE_FALL, true);

  // LED Array
  LED_Array_Init(Led_Pins, LED_LENGTH);

  // initialize dht20_sensor
  if (!setup_sensor(SENSOR_I2C_SDA, SENSOR_I2C_SCL)){
    printf("ERROR INITIALIZING DHT20 SENSOR\r\n");
  }
  

  // demo sensor
  for (int i=0; i < 5; i++){ 
    take_measurement();
    sleep_ms(5000);
  }

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
    if (DEBUG) {
      printf("LED_Value is: %d\r\n", LED_Value);
    }

    Display_LED_Array(LED_Value);
  }

  return 0;
}

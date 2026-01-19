// Pico SDK
#include "pico/stdlib.h"
#include "pico/bootrom.h"

// User Modules
#include "hardware/led_array.h"

// Buttons
#define BUTTON_1 24
#define BUTTON_2 22
#define BUTTON_3 16

// LED Array
#define LED_PINS_LENGTH 6
#define LED_PIN_0 10
#define LED_PIN_1 11
#define LED_PIN_2 12
#define LED_PIN_3 13
#define LED_PIN_4 14
#define LED_PIN_5 15
uint Led_Pins[LED_PINS_LENGTH] = {LED_PIN_0, LED_PIN_1, LED_PIN_2, LED_PIN_3, LED_PIN_4, LED_PIN_5};

// LCD I2C
#define LCD_I2C_SDA 0
#define LCD_I2C_SCL 2

// Sensor I2C
#define SENSOR_I2C_SDA 4
#define SENSOR_I2C_SCL 5

// ADC Pin
#define PHOTORESISTOR_ADC 26

// Output Flag
volatile uint Output_Flag = 0;
const uint Output_Screens = 2;

int main() {
  // Needed for reboot
  stdio_init_all();

  // LED Array
  LED_Array_Init(Led_Pins, LED_PINS_LENGTH);

  uint led_value = 0;
  while (true) {
    Display_LED_Array(led_value++);
    if(led_value > LED_PINS_LENGTH)
      led_value = 0;
    sleep_ms(2000);
  }

  return 0;
}
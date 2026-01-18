// Pico SDK
#include "pico/stdlib.h"
#include "pico/bootrom.h"

// User Modules
#include "hardware/led_array.h"

// Reset
#define RESET_PIN 2

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
#define LCD_I2C_SDA 26
#define LCD_I2C_SCL 27

// Sensor I2C
#define SENSOR_I2C_SDA 20
#define SENSOR_I2C_SCL 21

void Reset_Init(uint pin_number){
  gpio_init(pin_number);
  gpio_set_dir(pin_number, GPIO_IN);
  gpio_pull_up(pin_number);
}

int main() {
  // Needed for reboot
  stdio_init_all();

  // Reset
  Reset_Init(RESET_PIN);

  // LED Array
  LED_Array_Init(Led_Pins, LED_PINS_LENGTH);

  uint led_value = 0;
  while (true) {
      if (!gpio_get(RESET_PIN))
    {
      reset_usb_boot(0,0);      
    }

    sleep_ms(500);
    Display_LED_Array(led_value++);
    if(led_value > LED_PINS_LENGTH)
      led_value = 0;
    sleep_ms(2000);
  }

  return 0;
}
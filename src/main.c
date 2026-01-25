// Standard Library
#include <stdbool.h>
#include <stdio.h>

// Pico SDK
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

// User Modules
#include "hardware/led_array.h"
#include "hardware/lcd.h"

// Configuration file; all hardware constants and settings here
#include "config.h"

// Debug Mode - enable or disable bc printf to UART is slow
#define DEBUG true

// Output Flag
volatile uint Output_Flag = 0;
const uint Output_Screens = 2;

// Test
uint LED_Value = 0;
volatile bool Increment = false;
volatile bool Decrement = false;
volatile bool Set_Zero = false;

void GPIO_Handler(uint gpio, uint32_t event_mask)
{
  switch (gpio)
  {
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

void Button_Init(uint button_pin)
{
  gpio_init(button_pin);
  gpio_set_dir(button_pin, GPIO_IN);
  gpio_pull_up(button_pin);
}

int main()
{
  // Needed for picotool
  stdio_init_all();

  // Buttons
  for (uint i = 0; i < BUTTONS_LENGTH; i++)
  {
    Button_Init(Buttons[i]);
  }
  gpio_set_irq_enabled_with_callback(BUTTON_1, GPIO_IRQ_EDGE_FALL, true, &GPIO_Handler);
  gpio_set_irq_enabled(BUTTON_2, GPIO_IRQ_EDGE_FALL, true);
  gpio_set_irq_enabled(BUTTON_3, GPIO_IRQ_EDGE_FALL, true);

  // LED Array
  LED_Array_Init(Led_Pins, LED_LENGTH);

  // LCD I2C init (I2C0)
  i2c_init(i2c0, 100 * 1000);
  gpio_set_function(LCD_I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(LCD_I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(LCD_I2C_SDA);
  gpio_pull_up(LCD_I2C_SCL);

  // Initialize LCD
  lcd_init();
  lcd_set_cursor(0, 0);
  lcd_string("Hello");
  lcd_set_cursor(1, 0);
  lcd_string("World");

  while (true)
  {
    if (Increment && LED_Value < LED_LENGTH)
    {
      LED_Value++;
      Increment = false;
    }
    if (Decrement && LED_Value > 0)
    {
      LED_Value--;
      Decrement = false;
    }
    if (Set_Zero)
    {
      LED_Value = 0;
      Set_Zero = false;
    }

    // printf for UART debugging only if debug mode enabled
    if (DEBUG)
    {
      printf("LED_Value is: %d\r\n", LED_Value);
    }

    Display_LED_Array(LED_Value);
  }

  return 0;
}

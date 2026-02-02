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
#include "hardware/i2c.h"

// config
#include "config.h"

// lcd ui
#include "ui/lcd_ui.h"
#include "hardware/lcd.h"
// User Modules
#include "hardware/led_array.h"
#include "hardware/buttons.h"
#include "data_flow/data_flow.h" // data types shared between main and core1
#include "core1/core1.h"

// Debug Mode - enable or disable bc printf to UART is slow
#define DEBUG 0

// System Interrupt Speed
#define SYS_TIMER 20 // ms

// Buttons
#define BUTTON_DEBOUNCE 1 // 20 ms
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
// #define LED_LENGTH 6
// #define LED_PIN_0 10
// #define LED_PIN_1 11
// #define LED_PIN_2 12
// #define LED_PIN_3 13
// #define LED_PIN_4 14
// #define LED_PIN_5 15
// uint32_t Led_Pins[LED_LENGTH] = {LED_PIN_0, LED_PIN_1, LED_PIN_2, LED_PIN_3, LED_PIN_4, LED_PIN_5};

// ADC Conversion
#define ADC_MAX 3200
#define ADC_MIN 100

// Humidity Sensor I2C
#define SENSOR_I2C_SDA 4
#define SENSOR_I2C_SCL 5

static lcd_env_data_t ui = {0};
// static char lcd_line1[17];
// static char lcd_line2[17];

// refresh rate limiters
// LCD should only repaint when *values changed*, and not more often than 1Hz.
static absolute_time_t lcd_next_update;
static absolute_time_t uart_next_update;

// last rendered raw sensor values (avoid float compare noise)
static bool lcd_has_last = false;
static uint32_t lcd_last_temp_raw = 0;
static uint32_t lcd_last_hum_raw = 0;

// Test Globals
uint32_t LED_Value = 0;

void Button_1_Handler(void)
{
  if (LED_Value > 0)
    LED_Value--;
}

void Button_2_Handler(void)
{
  if (LED_Value < LED_LENGTH)
    LED_Value++;
}

void Button_3_Handler(void)
{
  LED_Value = 0;
}

bool system_timer_callback(struct repeating_timer *t)
{
  // protect critical section
  uint32_t status = save_and_disable_interrupts();

  // decrement buttons disabled count
  for (Button *btn = Button_Array; btn < Button_Array + NUM_BUTTONS; btn++)
  {
    if (btn->disabled_count)
      btn->disabled_count--;
  }

  restore_interrupts(status);
  return true;
}

void GPIO_Handler(uint gpio, uint32_t event_mask)
{
  for (Button *btn = Button_Array; btn < Button_Array + NUM_BUTTONS; btn++)
  {
    if (btn->button_pin == gpio && btn->disabled_count == 0)
    {                                         // if this button is the pin that's been pressed and it's not disabled
      btn->flag = true;                       // set flag to true, pressed
      btn->disabled_count = btn->reset_value; // resetd disabled counter
    }
  }
}

void Button_Logic(void)
{
  for (Button *btn = Button_Array; btn < Button_Array + NUM_BUTTONS; btn++)
  {
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

void Process_Data(void)
{
  // Drain the buffer; keep only the newest sample for LCD.
  bool have_sample = false;
  Payload_Data newest = {0};

  while (Data_Ring_Buffer.head != Data_Ring_Buffer.tail)
  {
    Payload_Data data = Data_Ring_Buffer.buffer[Data_Ring_Buffer.tail];
    __dmb();
    Data_Ring_Buffer.tail = (Data_Ring_Buffer.tail + 1) % DATA_BUFFER_SIZE;

    newest = data;
    have_sample = true;

    // --- UART update @ 5Hz (оставляем!) ---
    if (absolute_time_diff_us(get_absolute_time(), uart_next_update) <= 0)
    {
      uart_next_update = delayed_by_ms(get_absolute_time(), 200);

      printf("IDX:%u ADC:%u T:%lu H:%lu\r\n",
             (unsigned)Data_Ring_Buffer.tail,
             (unsigned)data.ADC_Data,
             (unsigned long)data.Temperature_Data,
             (unsigned long)data.Humidity_Data);
    }
  }

  if (!have_sample)
    return;

  // --- LCD update ---
  // Rules:
  // 1) If values didn't change -> do NOT repaint.
  // 2) If values changed -> repaint, but no more often than 1Hz.
  const uint32_t temp_raw = newest.Temperature_Data;
  const uint32_t hum_raw = newest.Humidity_Data;

  const bool changed = (!lcd_has_last) || (temp_raw != lcd_last_temp_raw) || (hum_raw != lcd_last_hum_raw);
  if (!changed)
    return;

  if (absolute_time_diff_us(get_absolute_time(), lcd_next_update) > 0)
    return; // too early for next allowed repaint

  lcd_next_update = delayed_by_ms(get_absolute_time(), 1000);
  lcd_has_last = true;
  lcd_last_temp_raw = temp_raw;
  lcd_last_hum_raw = hum_raw;

  ui.mode = LCD_MODE_NORMAL;
  ui.view_mode = LCD_VIEW_ENV;

  ui.has_temp = true;
  ui.temp_unit = TEMP_C;
  ui.temp_value = (float)temp_raw / 100.0f;

  ui.has_humidity = true;
  ui.humidity_percent = (float)hum_raw / 100.0f;

  lcd_env_render(&ui);
}

int main()
{
  // Needed for picotool
  stdio_init_all();

  // --- i2c rpotocol init init (per README) ---
  i2c_init(LCD_I2C_PORT, 100 * 1000);

  // set up lcd i2c pins
  gpio_set_function(LCD_I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(LCD_I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(LCD_I2C_SDA);
  gpio_pull_up(LCD_I2C_SCL);

  // lcd init
  lcd_init();

  // Init rate limiters (so we don't spam LCD/UART right after boot)
  absolute_time_t now = get_absolute_time();
  uart_next_update = delayed_by_ms(now, 200);
  lcd_next_update = delayed_by_ms(now, 1000);

  // System Timer
  struct repeating_timer timer;
  add_repeating_timer_ms(SYS_TIMER, system_timer_callback, NULL, &timer);

  // Buttons
  Button_Init(Button_Array, NUM_BUTTONS);
  GPIO_Interrupt_Init(GPIO_Handler);

  // LED Array
  LED_Array_Init(Led_Pins, LED_LENGTH);

  // Launch Core 1
  multicore_launch_core1(Core_1_Entry);

  while (true)
  {
    Button_Logic();

// printf for UART debugging only if debug mode enabled
#if DEBUG
    static uint32_t led_value_old = 0;
    if (LED_Value != led_value_old)
    {
      printf("LED_Value is: %d\r\n", LED_Value);
      led_value_old = LED_Value;
    }
#endif

    Process_Data();

    Display_LED_Array(LED_Value);
  }

  return 0;
}
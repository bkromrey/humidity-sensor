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



// Button Handler Prtotypes
void Button_1_Handler(void);
void Button_2_Handler(void);
void Button_3_Handler(void);

Button Button_Array[NUM_BUTTONS] = {
    {BUTTON_1, 0, BUTTON_DEBOUNCE, false, Button_1_Handler},
    {BUTTON_2, 0, BUTTON_DEBOUNCE, false, Button_2_Handler},
    {BUTTON_3, 0, BUTTON_DEBOUNCE, false, Button_3_Handler},
};




static lcd_env_data_t ui = {0};
// static char lcd_line1[17];
// static char lcd_line2[17];

// refresh rate lcd limiter
static absolute_time_t lcd_next_update;

// LCD screen toggle (Button 3): ENV <-> PHOTO
static volatile lcd_view_t g_view_mode = LCD_VIEW_ENV;
static volatile bool lcd_force_redraw = false;

static absolute_time_t uart_next_update;

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
  // Toggle LCD view: ENV <-> PHOTO
  g_view_mode = (g_view_mode == LCD_VIEW_ENV) ? LCD_VIEW_PHOTO : LCD_VIEW_ENV;
  lcd_force_redraw = true;
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
  // Drain ring buffer; keep only the most recent sample for LCD (prevents rapid redraw)
  bool have_sample = false;
  Payload_Data newest = {0};

  while (Data_Ring_Buffer.head != Data_Ring_Buffer.tail)
  {

    Payload_Data data = Data_Ring_Buffer.buffer[Data_Ring_Buffer.tail];
    __dmb();
    Data_Ring_Buffer.tail =
        (Data_Ring_Buffer.tail + 1) % DATA_BUFFER_SIZE;

    newest = data;
    have_sample = true;

    // --- UART update @ 5Hz (оставляем!) ---
    if (absolute_time_diff_us(get_absolute_time(), uart_next_update) <= 0)
    {
      uart_next_update = delayed_by_ms(get_absolute_time(), 200);

      printf("IDX:%u ADC:%u T:%lu H:%lu",
             (unsigned)Data_Ring_Buffer.tail,
             (unsigned)data.ADC_Data,
             (unsigned long)data.Temperature_Data,
             (unsigned long)data.Humidity_Data);
    }
  }

  if (!have_sample)
    return;

  // --- LCD update: только при изменении и не чаще 1Hz ---
  static bool lcd_has_last = false;
  static uint32_t last_adc = 0;
  static uint32_t last_t = 0;
  static uint32_t last_h = 0;

  bool env_changed = (!lcd_has_last) ||
                     (newest.Temperature_Data != last_t) ||
                     (newest.Humidity_Data != last_h);

  // Noise threshold for ADC (photo screen) using existing ADC_MIN/ADC_MAX scaling
  uint32_t adc_now = newest.ADC_Data;
  bool photo_changed = (!lcd_has_last) ||
                       ((adc_now > last_adc ? adc_now - last_adc : last_adc - adc_now) >= PHOTO_NOISE_THR);

  // Determine if we should render now
  bool time_ok = (absolute_time_diff_us(get_absolute_time(), lcd_next_update) <= 0);
  bool want_render = false;

  if (lcd_force_redraw)
  {
    want_render = true;
  }
  else if (time_ok)
  {
    if (g_view_mode == LCD_VIEW_ENV)
      want_render = env_changed;
    else if (g_view_mode == LCD_VIEW_PHOTO)
      want_render = photo_changed;
  }

  if (!want_render)
    return;

  // Rate limit next LCD update to 1 Hz
  lcd_next_update = delayed_by_ms(get_absolute_time(), 1000);
  lcd_force_redraw = false;

  // Fill UI struct
  ui.mode = LCD_MODE_NORMAL;
  ui.view_mode = g_view_mode;

  // ENV
  ui.has_temp = true;
  ui.temp_unit = TEMP_C;
  ui.temp_value = (float)newest.Temperature_Data / 100.0f;

  ui.has_humidity = true;
  ui.humidity_percent = (float)newest.Humidity_Data / 100.0f;

  // PHOTO (reuse newest.ADC_Data)
  ui.has_photo = true;
  ui.photo_raw = (uint16_t)adc_now;

  // Normalize to percent using ADC_MIN/ADC_MAX (from this file)
  float pct = 0.0f;
  if (ADC_MAX > ADC_MIN)
  {
    int32_t clamped = (int32_t)adc_now;
    if (clamped < ADC_MIN)
      clamped = ADC_MIN;
    if (clamped > ADC_MAX)
      clamped = ADC_MAX;
    pct = (float)(clamped - ADC_MIN) * 100.0f / (float)(ADC_MAX - ADC_MIN);
  }
  ui.photo_percent = pct;

  lcd_env_render(&ui);

  // Update "last" values after a successful render
  last_adc = adc_now;
  last_t = newest.Temperature_Data;
  last_h = newest.Humidity_Data;
  lcd_has_last = true;
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
// Standard Library
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "config.h"
#include "ui/lcd_screens.h"

// test ci
//  Pico SDK
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/time.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"

// User Modules
#include "hardware/buttons.h"
#include "hardware/led_array.h"
#include "hardware/photores.h"
#include "hardware/dht20_sensor.h"
#include "data_flow/data_flow.h" // data types shared between main and core1
#include "core1/core1.h"

// Debug Mode - enable or disable bc printf to UART is slow

// System Interrupt Speed

// LED Array
// #define LED_LENGTH 6
// #define LED_PIN_0 10
// #define LED_PIN_1 11
// #define LED_PIN_2 12
// #define LED_PIN_3 13
// #define LED_PIN_4 14
// #define LED_PIN_5 15
// uint32_t Led_Pins[LED_LENGTH] = {LED_PIN_0, LED_PIN_1, LED_PIN_2, LED_PIN_3, LED_PIN_4, LED_PIN_5};



// Global Button Array
Button Button_Array[NUM_BUTTONS] = {
    {BUTTON_1, 0, BUTTON_DEBOUNCE, false},
    {BUTTON_2, 0, BUTTON_DEBOUNCE, false},
    {BUTTON_3, 0, BUTTON_DEBOUNCE, false},
};




// The stuff below here may need to be removed; it was a placeholder originally

// ADC Conversion





// Function Prototypes
void Refresh_Data(void);
void GPIO_Handler(uint gpio, uint32_t event_mask);
bool system_timer_callback(struct repeating_timer *t);
void Clear_Button_Flags(void);

// ********** State Machine **********

// Enum for function states
typedef enum
{
  Init,
  Loading,
  Normal_F,
  Normal_C,
  Photores,
} State;

// Function Prototypes
State Init_State(void);
State Loading_State(void);
State Normal_F_State(void);
State Normal_C_State(void);
State Photores_State(void);

typedef State (*stateHandler)(void); // function pointer

stateHandler StateTable[] = {
    Init_State,
    Loading_State,
    Normal_F_State,
    Normal_C_State,
    Photores_State};

State Get_Corresponding_Screen(State *screens);

// Global Values
volatile Payload_Data Sensor_Data_Copy;
volatile bool Data_Ready_Flag = false;

/*********** Main **********/
int main(void)
{
  State current = Init;
  while (1)
  {
    current = StateTable[current]();
  }
}

/*********** Initial State **********/
State Init_State(void)
{
  stdio_init_all();

  //
  ui_lcd_init();
  ui_show_loading();

  // all code below added just for testing screens - will be removed later
  sleep_ms(2000);
  // ui_show_dht20_f(NULL);
  // sleep_ms(2000);
  // ui_show_dht20_c(NULL);
  // sleep_ms(2000);
  // ui_show_photores(NULL);
  // sleep_ms(2000);
  // ui_show_custom("Custom Line 1", "Custom Line 2");
  // sleep_ms(2000);
  // ui_show_error("ERROR: NO DATA", "DHT20 / ADC");

#if DEBUG
  sleep_ms(2000);
  printf("Current State is: Init\r\n");
#endif

  // System Timer
  static struct repeating_timer timer;
  add_repeating_timer_ms(SYS_TIMER, system_timer_callback, NULL, &timer);

  // Photoresistor
  Photoresistor_Init(PHOTORES_GPIO_PIN);

  // Buttons
  Button_Init(Button_Array, NUM_BUTTONS);
  GPIO_Interrupt_Init(GPIO_Handler);

  // LED Array
  LED_Array_Init(Led_Pins, LED_LENGTH);

  // initialize dht20_sensor
  if (setup_sensor(SENSOR_I2C_SDA, SENSOR_I2C_SCL, SENSOR_I2C_CHANNEL))
  {
// TODO: error handling here
#if DEBUG
    printf("ERROR INITIALIZING DHT20 SENSOR\r\n");
#endif
  }
  // Launch Core 1
  multicore_launch_core1(Core_1_Entry);

  return Loading;
}

/*********** Loading **********/
State Loading_State(void)
{
#if DEBUG
  printf("Current State is: Loading\r\n");
  sleep_ms(2000);
#endif

  // need to print a loading screen here

  while (!Data_Ready_Flag) // Spin until a packet is received
    Refresh_Data();

  return Normal_F;
}

/*********** Normal_F **********/
State Normal_F_State(void)
{
#if DEBUG
  printf("Current State is: Normal_F\r\n");
  sleep_ms(2000);
#endif
  Refresh_Data();

  if (Data_Ready_Flag)
  {

#if DEBUG
    printf("DHT20 Sensor Data Validity: %d\tTemp (F) is: %f\r\n", Sensor_Data_Copy.DHT20_Data_Valid, Sensor_Data_Copy.DHT20_Data.temperature_f);
#endif
    if (Data_Ready_Flag)
    {
      ui_show_dht20_f((const Payload_Data *)&Sensor_Data_Copy);
      Data_Ready_Flag = false;
    }

    // Display Flag
    Data_Ready_Flag = false;
  }

  // [0] - default
  // [1] - button 0
  // [2] - button 1
  // [3] - button 2
  State return_vals[NUM_BUTTONS + 1] = {
      Normal_F,
      Photores,
      Photores,
      Normal_C,
  };
  State return_val = Get_Corresponding_Screen(return_vals);

  Clear_Button_Flags();
  if (return_val != return_vals[0])
    Data_Ready_Flag = true; // allow next state to render on entry

  return return_val;
}

/*********** Normal_C **********/
State Normal_C_State(void)
{
#if DEBUG
  printf("Current State is: Normal_C\r\n");
  sleep_ms(2000);
#endif
  Refresh_Data();

  if (Data_Ready_Flag)
  {
    // Display LCD Data
    if (Data_Ready_Flag)
    {
      ui_show_dht20_c((const Payload_Data *)&Sensor_Data_Copy);
      Data_Ready_Flag = false;
    }

    // Display Flag
    Data_Ready_Flag = false;
  }

  // [0] - default
  // [1] - button 0
  // [2] - button 1
  // [3] - button 2
  State return_vals[NUM_BUTTONS + 1] = {
      Normal_C,
      Photores,
      Photores,
      Normal_F,
  };
  State return_val = Get_Corresponding_Screen(return_vals);

  Clear_Button_Flags();
  if (return_val != return_vals[0])
    Data_Ready_Flag = true; // allow next state to render on entry

  return return_val;
}

/*********** Photoresistor **********/
State Photores_State(void)
{
#if DEBUG
  printf("Current State is: Photores\r\n");
  sleep_ms(2000);
#endif
  Refresh_Data();

  if (Data_Ready_Flag)
  {
    // Display LCD Data
    if (Data_Ready_Flag)
    {
      ui_show_photores((const Payload_Data *)&Sensor_Data_Copy);
      Data_Ready_Flag = false;
    }

    // Display Flag
    Data_Ready_Flag = false;
  }

  // [0] - default
  // [1] - button 0
  // [2] - button 1
  // [3] - button 2
  State return_vals[NUM_BUTTONS + 1] = {
      Photores,
      Normal_F,
      Normal_F,
      Photores,
  };
  State return_val = Get_Corresponding_Screen(return_vals);

  Clear_Button_Flags();
  if (return_val != return_vals[0])
    Data_Ready_Flag = true; // allow next state to render on entry

  return return_val;
}

/**
 * System timer callback for button debouncing
 * Loops through the global button array and will decrement the button's disabled counter if it is greater than zero
 * debounces button input
 */
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

/**
 * GPIO Handler for every falling edge button press
 * Will loop through button array and set its counter if the button is enabled
 */
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

/**
 * Sends packet acknowledgement to Core1
 */
void Ack_Successful(void)
{
  multicore_fifo_push_blocking(true);
}

/**
 * Sets Data_Ready_Flag letting other states know to read Sensor_Data
 *
 */
void Refresh_Data(void)
{
  bool data_ready = multicore_fifo_rvalid();
  if (!data_ready)
    return;

  Payload_Data *ptr = (Payload_Data *)multicore_fifo_pop_blocking(); // get pointer to data from Core1
  Sensor_Data_Copy = *ptr;                                           // copy data from Core1
  Ack_Successful();                                                  // let Core 0 continue
  Data_Ready_Flag = true;                                            // set Data_Ready_Flag indicating we have new data to display
}

/**
 * Takes an array of state returns and selects the corresponding button
 * Array must be State screen[NUM_BUTTON + 1] = [DEFAULT, etc]
 */
State Get_Corresponding_Screen(State *screens)
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    uint32_t status = save_and_disable_interrupts();
    bool pressed = Button_Array[i].flag;
    restore_interrupts(status);

    if (pressed)
      return screens[i + 1];
  }
  return screens[0];
}


/**
 * Clear all flag of buttons
 */
void Clear_Button_Flags(void)
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    uint32_t status = save_and_disable_interrupts();
    Button_Array[i].flag = false; // critical section due to shared memory
    restore_interrupts(status);
  }
}

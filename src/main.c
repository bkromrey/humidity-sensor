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

// System Interrupt Speed
#define SYS_TIMER 20 // ms

// Buttons
#define BUTTON_DEBOUNCE 1 // 20 ms
#define NUM_BUTTONS 3
#define BUTTON_1 16
#define BUTTON_2 17
#define BUTTON_3 18

// Button Handler Prototypes
void Button_1_Handler(void);
void Button_2_Handler(void);
void Button_3_Handler(void);

// Global Button Array
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

// Function Prototypes
void Refresh_Data(void);
void GPIO_Handler(uint gpio, uint32_t event_mask);
bool system_timer_callback(struct repeating_timer *t);

// ********** State Machine **********

// Enum for function states
typedef enum {
  Init,
  Loading,
  Error,
  Normal_F,
  Normal_C,
  PhotoRes,
} State;

// Function Prototypes
State Init_State(void);
State Loading_State(void);
State Error_State(void);
State Normal_F_State(void);
State Normal_C_State(void);
State Photores_State(void);

typedef State (*stateHandler) (void); // function pointer

stateHandler StateTable[] = {
  Init_State,
  Loading_State,
  Error_State,
  Normal_F_State,
  Normal_C_State,
  Photores_State
};

// Global Values
volatile Payload_Data Sensor_Data;
volatile bool Data_Ready_Flag;

/*********** Main **********/ 
int main(void){
  State current = Init;
  while(1){
    current = StateTable[current]();
  }
}

/*********** Initial State **********/
State Init_State(void){
  #if DEBUG
    printf("Current State is: Init")
  #endif
  // Needed for picotool
  stdio_init_all();

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

  return Loading;
}

/*********** Loading **********/
State Loading_State(void){
  #if DEBUG
    printf("Current State is: Loading")
  #endif

  while (!Data_Ready_Flag) // Block until a packet is received
    Refresh_Data();

  return Normal_F_State;
}

/*********** Normal_F **********/
State Normal_F_State(void){
  #if DEBUG
    printf("Current State is: Normal_F")
  #endif
  
  // fetch data
  Refresh_Data();
  if (!Data_Ready_Flag)
    return Normal_F_State;

    LED_Value(0);

  return Normal_F_State;
}

/*********** Normal_C **********/
State Normal_C_State(void){
  #if DEBUG
    printf("Current State is: Normal_F")
  #endif
  
  // fetch data
  Refresh_Data();
  if (!Data_Ready_Flag)
    return Normal_C_State;

    LED_Value(0);

  return Normal_C_State;
}

/*********** Photoresistor **********/
State Photores_State(void){
  #if DEBUG
    printf("Current State is: Normal_F")
  #endif
  
  // fetch data
  Refresh_Data();
  if (!Data_Ready_Flag)
    return Photores_State;

    LED_Value(0);

  return Photores_State
}

/**
 * System timer callback for button debouncing
 * Loops through the global button array and will decrement the button's disabled counter if it is greater than zero
 * debounces button input
 */
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

/**
 * GPIO Handler for every falling edge button press
 * Will loop through button array and set its counter if the button is enabled
 */
void GPIO_Handler(uint gpio, uint32_t event_mask){
  for(Button *btn = Button_Array; btn < Button_Array + NUM_BUTTONS ;btn++){
    if(btn->button_pin == gpio && btn->disabled_count == 0){  // if this button is the pin that's been pressed and it's not disabled
      btn->flag = true;                                       // set flag to true, pressed
      btn->disabled_count = btn->reset_value;                 // resetd disabled counter 
    }
  }
}

/**
 * Sends packet acknowledgement to Core1
 */
void Ack_Successful(void){
  multicore_fifo_push_blocking(true);
}

/**
 * Sets Data_Ready_Flag letting other states know to read Sensor_Data
 * 
 */
void Refresh_Data(void){
  bool data_ready = multicore_fifo_rvalid();
  if (!data_ready)
    return ;

  Payload_Data *ptr = (Payload_Data *) multicore_fifo_pop_blocking(); // get pointer to data from Core1
  Sensor_Data = *ptr; // copy data from Core 0
  Ack_Successful(); // let Core 0 continue
}
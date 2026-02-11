#include "core1.h"

#define CORE1_TIMER 1000

#define PHOTORES_GPIO_PIN 26 // this needs to be put into config.h

// File Scope Datatypes
typedef struct {
    volatile uint32_t disabled_count;
    uint32_t reset_value;
    void (*flag_handler)(void); // call back when flag triggers
} System_Flag;

// System Flag Handling
#define NUM_SYSTEM_FLAGS 1
#define SYSTEM_RELOAD 1 // 1000 ms

// Prototypes
void Produce_Data(void);

// Globals
Payload_Data Sensor_Data; // for exchanging data to Core0

// Core_1 System Flag Array
System_Flag Core_1_Flags[NUM_SYSTEM_FLAGS] = {
 {0, SYSTEM_RELOAD, Produce_Data}, // Sample Data Flag
};

/**
 * Samples data, packs it into the global payload struct
 * Sends data to core0, blocks until data acknowledgement is received
 */
void Produce_Data(void){
    // Write to Global
    Payload_Data *data= &Sensor_Data;

    // Take Measurement from DHT20 sensor (temperature & humidity)  
    DHT20_Reading dht20_reading;        
    int dht20_valid = !take_measurement(&dht20_reading);      // invert validity boolean because take_measurement returns 0 for success, 1 for error
    data->DHT20_Data = dht20_reading;   
    data->DHT20_Data_Valid = dht20_valid;
  
    // Take Measurement from photoresistor
    data->ADC_Data = Get_Photo_Resistor_Data(PHOTORES_GPIO_PIN);
  
    // Logic Checking Here

    // If Data is Valid
    multicore_fifo_push_blocking((uint32_t) data);
    bool response = multicore_fifo_pop_blocking(); // block until the packet is read, not used for anything yet
}

/**
 * Iterates through Core_1_Flag structs on timer execution; decrements value until 0
 */
bool Core_1_Timer_Callback(struct repeating_timer *t){
    // protect critical section
    uint32_t status = save_and_disable_interrupts();
    
    // Decrement Flag
    for(System_Flag *flg = Core_1_Flags; flg < Core_1_Flags + NUM_SYSTEM_FLAGS ; flg++){
        if(flg->disabled_count){
            flg->disabled_count--;
        }
    }
    restore_interrupts(status); // advance head to show it has new data
    return true;
}

/**
 * Iterates through Core_1_Flags and runs processes if valid
 * If System flag is 0, function pointer in System_Flag Array is executed
 */
void System_Flag_Logic(void){  
    for(System_Flag *flg = Core_1_Flags; flg < Core_1_Flags + NUM_SYSTEM_FLAGS; flg++){
        // protected section
        uint32_t status = save_and_disable_interrupts();
        uint32_t flag_timer_local = flg->disabled_count; // save  locally
        restore_interrupts(status);

        // Flag Logic
        if (flag_timer_local == 0){
            flg->flag_handler();
            // Protected section
            uint32_t status = save_and_disable_interrupts();
            flg->disabled_count = flg->reset_value;
            restore_interrupts(status);
        }
    }
}

/**
 * Core1 process called from Core0
 */
void Core_1_Entry(void){

    // Core 1 Timer
    struct repeating_timer timer;
    add_repeating_timer_ms(CORE1_TIMER, Core_1_Timer_Callback, NULL, & timer);

    while (true){
        // handle the flag here
        System_Flag_Logic();
    }
}
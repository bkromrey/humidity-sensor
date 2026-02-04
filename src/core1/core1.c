#include "core1.h"

#define CORE1_TIMER 1000

// ADC Pin
#define PHOTORESISTOR_ADC 26

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
Payload_Data Sensor_Data;

System_Flag Core_1_Flags[NUM_SYSTEM_FLAGS] = {
 {0, SYSTEM_RELOAD, Produce_Data}, // Sample Data Flag
};

void Produce_Data(void){
    // Write to Global
    Payload_Data *data= &Sensor_Data;
    data->ADC_Data = adc_read();

    // Logic Checking Here

    // If Data is Valid
    multicore_fifo_push_blocking((uint32_t) data);
    bool response = multicore_fifo_pop_blocking();
}

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

void ADC_Init(){
    adc_init();
    adc_gpio_init(PHOTORESISTOR_ADC);
    adc_select_input(PHOTORESISTOR_ADC - 26); // Pins 26-29 are ADC pins on the pico
}

void Core_1_Entry(void){
    // ADC for the photoresistor
    ADC_Init();

    // Core 1 Timer
    struct repeating_timer timer;
    add_repeating_timer_ms(CORE1_TIMER, Core_1_Timer_Callback, NULL, & timer);

    while (true){
        // handle the flag here
        System_Flag_Logic();
    }
}
#include "core1.h"

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
#define SYSTEM_RELOAD 1000 // ms

// Flags
void Sample_Data(void);

System_Flag Core_1_Flags[NUM_SYSTEM_FLAGS] = {
 {0, SYSTEM_RELOAD, Sample_Data}, // Sample Data Flag
};

void Sample_Data(void){
    uint16_t raw = adc_read();
    printf("raw is: %d\r\n", raw);
}

bool Core_1_Timer_Callback(struct repeating_timer *t){
    // protect critical section
    uint32_t status = save_and_disable_interrupts();
    
    // decrement buttons disabled count
    for(System_Flag *flg = Core_1_Flags; flg < Core_1_Flags + NUM_SYSTEM_FLAGS ; flg++){
        if(flg->disabled_count){
            flg->disabled_count--;
        }
    }
    restore_interrupts(status);
    return true;
}

void System_Flag_Logic(void){  
    for(System_Flag *flg = Core_1_Flags; flg < Core_1_Flags + NUM_SYSTEM_FLAGS; flg++){
        // protected section
        uint32_t status = save_and_disable_interrupts();
        uint32_t flag_timer_local = flg->disabled_count; // save  locally
        // resume
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
    add_repeating_timer_ms(-1, Core_1_Timer_Callback, NULL, & timer);

    while (true){
        // handle the flag here
        System_Flag_Logic();
    }
}
#include "photores.h"

#define ADC_MAX 29
#define ADC_MIN 26
#define ADC_OFFSET 26 // pins 26 - 29 are ADC pins on the pico
// ADC 0 - gpio 26
// ADC 1 - gpio 27
// ADC 2 - gpio 28
// ADC 3 - gpio 29

/**
 * Initializes photoresistor pin for ADC sampling
 */
void Photoresistor_Init(uint gpio_pin){
    // Initialize hardware
    adc_init();
    adc_gpio_init(gpio_pin);
    adc_select_input(gpio_pin - ADC_OFFSET);
}

/**
 * Takes gpio pin as input
 * returns the current ADC reading for that pin
 */
uint16_t Get_Photo_Resistor_Data(uint gpio_pin){

    // Validate input
    if (gpio_pin < ADC_MIN || gpio_pin > ADC_MAX)
        return 0;

    // Select pin if needed
    uint current_input = adc_get_selected_input();
    if(current_input != gpio_pin - ADC_OFFSET)
        adc_select_input(gpio_pin - ADC_OFFSET);

    // Return read of photoresistor
    return adc_read();
}
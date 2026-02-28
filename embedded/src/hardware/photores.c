#include "photores.h"
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
    adc_select_input(gpio_pin - ADC_PIN_OFFSET);
}

/**
 * Takes gpio pin as input
 * returns the current ADC reading for that pin
 */
uint16_t Get_Photoresistor_Data(uint gpio_pin){

    // Validate input
    if (gpio_pin < ADC_PIN_MIN || gpio_pin > ADC_PIN_MAX)
        return 0;

    // Select pin if needed
    uint current_input = adc_get_selected_input();
    if(current_input != gpio_pin - ADC_PIN_OFFSET)
        adc_select_input(gpio_pin - ADC_PIN_OFFSET);

    // Return read of photoresistor
    return adc_read();
}

/**
 * Takes adc read and converts to percentage
 */
float Convert_Photoresistor_Percent(uint16_t adc_raw){
    float result = 0;
    result = ((float) adc_raw - ADC_MIN) / (ADC_MAX - ADC_MIN);
    result *= 100.0f;                                           // scale to percentage
    return result;
}
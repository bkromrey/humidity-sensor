#ifndef __PHOTORES_H_
#define __PHOTORES_H_

// Standard Library
#include <stdint.h>

// Pico SDK
#include "hardware/adc.h"

/**
 * Initializes ADC pin for ADC sampling
 */
void Photoresistor_Init(uint gpio_pin);


/**
 * Takes gpio pin as input
 * returns the current ADC reading for that pin
 */
uint16_t Get_Photo_Resistor_Data(uint gpio_pin);

#endif
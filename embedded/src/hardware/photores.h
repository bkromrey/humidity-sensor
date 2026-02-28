#ifndef __PHOTORES_H_
#define __PHOTORES_H_

// Standard Library
#include <stdint.h>

// Pico SDK
#include "hardware/adc.h"

// User Module
#include "../config.h"

/**
 * Initializes ADC pin for ADC sampling
 */
void Photoresistor_Init(uint gpio_pin);


/**
 * Takes gpio pin as input
 * returns the current ADC reading for that pin
 */
uint16_t Get_Photoresistor_Data(uint gpio_pin);

/**
 * Converts adc raw data to a percentage value
 */
float Convert_Photoresistor_Percent(uint16_t adc_raw);

#endif
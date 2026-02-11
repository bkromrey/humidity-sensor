#ifndef CONFIG_H
#define CONFIG_H

#include "pico/stdlib.h"
#include <stdint.h>
#include "hardware/i2c.h"
#include <stdbool.h>

// Debug Mode - enable or disable bc printf to UART is slow
#define DEBUG 0
#define PHOTO_NOISE_THR 15

// System Interrupt Speed
#define SYS_TIMER 20 // ms

// ADC Conversion
#define ADC_MAX 3200
#define ADC_MIN 100

// Buttons
#define BUTTON_DEBOUNCE 1 // 20 ms
#define NUM_BUTTONS 3
#define BUTTON_1 16
#define BUTTON_2 17
#define BUTTON_3 18
// ===== BUTTONS =====
#define BUTTONS_LENGTH 3
#define BUTTON_1 16
#define BUTTON_2 17
#define BUTTON_3 18

static const uint Buttons[BUTTONS_LENGTH] = {
    BUTTON_1,
    BUTTON_2,
    BUTTON_3
};

// ===== LED ARRAY =====
#define LED_LENGTH 6
#define LED_PIN_0 10
#define LED_PIN_1 11
#define LED_PIN_2 12
#define LED_PIN_3 13
#define LED_PIN_4 14
#define LED_PIN_5 15

static const uint32_t Led_Pins[LED_LENGTH] = {
    LED_PIN_0,
    LED_PIN_1,
    LED_PIN_2,
    LED_PIN_3,
    LED_PIN_4,
    LED_PIN_5
};

// LCD on I2C1 (GP2/GP3)
#define LCD_I2C_ADDR 0x27
#define LCD_I2C_PORT i2c1
#define LCD_I2C_SDA  2
#define LCD_I2C_SCL  3

// DHT20 on I2C0 (GP4/GP5)
#define SENSOR_I2C_PORT i2c0
#define SENSOR_I2C_SDA  4
#define SENSOR_I2C_SCL  5
#define SENSOR_I2C_CHANNEL SENSOR_I2C_PORT


// ===== ADC =====
#define PHOTORESISTOR_ADC 26

#define ADC_MAX 3200
#define ADC_MIN 100

#define PHOTORES_GPIO_PIN 26



#endif // CONFIG_H

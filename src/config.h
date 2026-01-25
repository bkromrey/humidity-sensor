#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include "hardware/i2c.h"

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

static const uint Led_Pins[LED_LENGTH] = {
    LED_PIN_0,
    LED_PIN_1,
    LED_PIN_2,
    LED_PIN_3,
    LED_PIN_4,
    LED_PIN_5
};

// ===== LCD (I2C0) =====
#define LCD_I2C_PORT     i2c0
#define LCD_I2C_SDA  0   // GP0
#define LCD_I2C_SCL  1   // GP1
#define LCD_I2C_ADDR     0x27

// ===== HUMIDITY SENSOR (I2C1) =====
#define SENSOR_I2C_PORT     i2c1
#define SENSOR_I2C_SDA  4
#define SENSOR_I2C_SCL  5

// ===== ADC =====
#define PHOTORESISTOR_ADC 26

#endif // CONFIG_H

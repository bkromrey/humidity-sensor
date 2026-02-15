#include "ui/lcd_screens.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "hardware/lcd_i2c.h"
#include "data_flow/data_flow.h"

/**
 * UI screens for a 16x2 I2C LCD.
 * This module formats sensor data (temperature, humidity, light) and common messages 
 * into fixed 16-character lines and sends them to the LCD driver.
 */
static lcd_i2c_t g_lcd;

/**
 * Copies a string into a 16-character buffer
 */
static void pad16(char *dst, const char *src) {
   
    size_t n = strlen(src);
    if (n > 16) n = 16;
    memcpy(dst, src, n);
    for (size_t i = n; i < 16; i++) dst[i] = ' ';
    dst[16] = '\0';
}

/**
 * Writes 2 16-character lines to the LCD
 */
static void write_2lines(const char *l1, const char *l2) {
    char a[17], b[17];
    pad16(a, l1);
    pad16(b, l2);

    //lcd_i2c
    lcd_i2c_set_cursor(&g_lcd, 0, 0);
    lcd_i2c_write_str(&g_lcd, a);
    lcd_i2c_set_cursor(&g_lcd, 0, 1);
    lcd_i2c_write_str(&g_lcd, b);
}

/**
 * Initializes the I2C bus and the LCD module, loads custom characters and shows the loading screen
 */
void ui_lcd_init(void) {
    // I2C0 for LCD
    i2c_init(LCD_I2C_PORT, 100 * 1000);

    gpio_set_function(LCD_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(LCD_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(LCD_I2C_SDA);
    gpio_pull_up(LCD_I2C_SCL);

    lcd_i2c_init(&g_lcd, LCD_I2C_PORT, LCD_I2C_ADDR, 16, 2);
    lcd_create_char(&g_lcd,1, degree_symbol);
    ui_show_loading();
}

/**
 * Shows a loading screen while the system is starting.
 */
void ui_show_loading(void) {
    write_2lines("Humidity Sensor", "Loading...");
}

/**
 * Shows two custom text lines on the LCD
 */
void ui_show_custom(const char *line1, const char *line2) {
    write_2lines(line1 ? line1 : "", line2 ? line2 : "");
}

/**
 * Displays DHT20 temperature in Celsius and humidity on the LCD
 * If data is not valid, shows placeholder
 */
void ui_show_dht20_c(const Payload_Data *p) {
    char l1[32], l2[32];

    if (!p || !p->DHT20_Data_Valid) {
        snprintf(l1, sizeof(l1), "Temp: --.-%cC",LCD_CHAR_DEGREE);
        snprintf(l2, sizeof(l2), "Humidity: --%%");
    } else {
        snprintf(l1, sizeof(l1), "Temp: %2.1f%cC", (double)p->DHT20_Data.temperature_c,LCD_CHAR_DEGREE);
        snprintf(l2, sizeof(l2), "Humidity: %2.1f%%", (double)p->DHT20_Data.humidity);
    }

    write_2lines(l1, l2);
}

/**
 * Displays DHT20 temperature in Fahrenheit and humidity on the LCD
 * If data is not valid, shows placeholder
 */
void ui_show_dht20_f(const Payload_Data *p) {
    char l1[32], l2[32];

    if (!p || !p->DHT20_Data_Valid) {
        snprintf(l1, sizeof(l1), "Temp: --.-%cF",LCD_CHAR_DEGREE);
        snprintf(l2, sizeof(l2), "Humidity: --%%");
    } else {
        snprintf(l1, sizeof(l1), "Temp: %2.1f%cF", (double)p->DHT20_Data.temperature_f,LCD_CHAR_DEGREE);
        snprintf(l2, sizeof(l2), "Humidity: %2.1f%%", (double)p->DHT20_Data.humidity);
    }

    write_2lines(l1, l2);
}

/**
 * Displays the photoresistor reading on the LCD
 * If data is not valid, shows placeholder
 */
void ui_show_photores(const Payload_Data *p) {
    char l1[32], l2[32];

    if (!p) {
        snprintf(l1, sizeof(l1), "Light");
        snprintf(l2, sizeof(l2), "ADC: ----");
    } else {
        snprintf(l1, sizeof(l1), "Light");
        snprintf(l2, sizeof(l2), "ADC: %4u", (unsigned)p->ADC_Data);
    }

    write_2lines(l1, l2);
}

/**
 * Shows an error message on the LCD.
 */
void ui_show_error(const char *line1, const char *line2) {

    if (!line1) line1 = "ERROR";
    if (!line2) line2 = "";

    write_2lines(line1, line2);
}

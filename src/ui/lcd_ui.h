#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdint.h>

/**
 * LCD display operation modes.
 * Define the overall state of the screen.
 */
typedef enum
{
    LCD_MODE_NORMAL = 0,  // Normal data display mode
    LCD_MODE_LOADING,     // Loading/waiting mode
    LCD_MODE_ERROR        // Error display mode
} lcd_mode_t;

/**
 * Data presentation modes on the screen.
 * Switch between different information screens.
 */
typedef enum
{
    LCD_VIEW_ENV = 0,  // Display environmental data (temperature, humidity)
    LCD_VIEW_TEXT,     // Display text messages
    LCD_VIEW_PHOTO     // Display photoresistor / ADC data
} lcd_view_t;

/**
 * Temperature measurement units.
 */
typedef enum
{
    TEMP_C = 0,  // Degrees Celsius
    TEMP_F       // Degrees Fahrenheit
} temp_unit_t;

/**
 * Data structure for displaying environmental information.
 * Contains all parameters needed for LCD rendering.
 */
typedef struct
{
    lcd_mode_t mode;         // Current display operation mode
    lcd_view_t view_mode;    // Current data presentation mode

    bool has_temp;           // Flag indicating temperature data availability
    float temp_value;        // Temperature value
    temp_unit_t temp_unit;   // Temperature measurement unit

    bool has_humidity;       // Flag indicating humidity data availability
    float humidity_percent;  // Humidity percentage

    bool has_photo;          // Flag indicating photo/ADC data availability
    uint16_t photo_raw;       // Raw ADC value (0..4095 or project scale)
    float photo_percent;      // Normalized light level in percent (0..100)


    const char *line1;       // Text for the first display line
    const char *line2;       // Text for the second display line
} lcd_env_data_t;

/**
 * Renders environmental data on the LCD display.
 * 
 * @param data Pointer to the data structure for display.
 *             Depending on mode and view_mode, displays
 *             temperature, humidity, or text messages.
 */
void lcd_env_render(const lcd_env_data_t *data);

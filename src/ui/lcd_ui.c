#include "ui/lcd_ui.h"
#include "hardware/lcd.h"
#include <stdio.h>

/**
 * Prints a string to the LCD with padding to fill 16 characters.
 * If the string is shorter than 16 characters, fills the rest with spaces.
 * If longer, truncates at 16 characters.
 * 
 * @param s The string to print. Can be NULL (will print all spaces).
 */
static void print_padded(const char *s)
{
    int i = 0;
    if (s)
    {
        for (; s[i] && i < 16; ++i)
        {
            char c[2] = {s[i], 0};
            lcd_string(c);
        }
    }
    for (; i < 16; ++i)
        lcd_string(" ");
}

/**
 * Renders environmental data on the LCD display.
 * Handles different display modes:
 * - LCD_MODE_LOADING: Shows "Loading..." message
 * - LCD_MODE_ERROR: Shows error message from line1/line2
 * - LCD_MODE_NORMAL: 
 *   - LCD_VIEW_TEXT: Displays custom text from line1/line2
 *   - LCD_VIEW_ENV: Displays temperature and humidity data
 * 
 * For environmental view, temperature is shown on line 0 with unit (C/F),
 * and humidity percentage is shown on line 1. Missing data is displayed as "--.-".
 * 
 * @param d Pointer to lcd_env_data_t structure containing display data.
 *          If NULL, only clears the display.
 */
void lcd_env_render(const lcd_env_data_t *d)
{
    lcd_clear();

    if (!d)
        return;

    // Loading mode: display loading message
    if (d->mode == LCD_MODE_LOADING)
    {
        lcd_set_cursor(0, 0);
        print_padded("Loading...");
        lcd_set_cursor(1, 0);
        print_padded("");
        return;
    }

    // Error mode: display error message
    if (d->mode == LCD_MODE_ERROR)
    {
        lcd_set_cursor(0, 0);
        print_padded(d->line1 ? d->line1 : "ERROR");
        lcd_set_cursor(1, 0);
        print_padded(d->line2 ? d->line2 : "");
        return;
    }

    // NORMAL mode - text view: display custom text
    if (d->view_mode == LCD_VIEW_TEXT)
    {
        lcd_set_cursor(0, 0);
        print_padded(d->line1 ? d->line1 : "");
        lcd_set_cursor(1, 0);
        print_padded(d->line2 ? d->line2 : "");
        return;
    }

    // NORMAL mode - environmental view: display sensor data
    char line0[17] = {0};
    char line1[17] = {0};

    // Format temperature for line 0
    if (d->has_temp)
    {
        if (d->temp_unit == TEMP_F)
        {
            snprintf(line0, sizeof(line0), "Temp.: %4.1fF", d->temp_value);
        }
        else
        {
            snprintf(line0, sizeof(line0), "Temp.: %4.1fC", d->temp_value);
        }
    }
    else
    {
        snprintf(line0, sizeof(line0), "Temp.: --.-");
    }

    // Format humidity for line 1
    if (d->has_humidity)
    {
        snprintf(line1, sizeof(line1), "Humidity: %4.1f%%", d->humidity_percent);
    }
    else
    {
        snprintf(line1, sizeof(line1), "Humidity: --.-%%");
    }

    // Display both lines
    lcd_set_cursor(0, 0);
    print_padded(line0);
    lcd_set_cursor(1, 0);
    print_padded(line1);
}

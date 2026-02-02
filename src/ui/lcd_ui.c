#include "ui/lcd_ui.h"
#include "hardware/lcd.h"
#include <stdio.h>
#include <string.h>

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
    // Frame cache to avoid unnecessary redraws (prevents visible flicker)
    static char last0[17] = {0};
    static char last1[17] = {0};

    char line0[17] = {0};
    char line1[17] = {0};

    if (!d)
        return;

    // Loading mode: display loading message
    if (d->mode == LCD_MODE_LOADING)
    {
        snprintf(line0, sizeof(line0), "Loading...");
        snprintf(line1, sizeof(line1), "");
    }
    // Error mode: display error message
    else if (d->mode == LCD_MODE_ERROR)
    {
        snprintf(line0, sizeof(line0), "%s", d->line1 ? d->line1 : "ERROR");
        snprintf(line1, sizeof(line1), "%s", d->line2 ? d->line2 : "");
    }
    // NORMAL mode - text view: display custom text
    else if (d->view_mode == LCD_VIEW_TEXT)
    {
        snprintf(line0, sizeof(line0), "%s", d->line1 ? d->line1 : "");
        snprintf(line1, sizeof(line1), "%s", d->line2 ? d->line2 : "");
    }
    // NORMAL mode - photoresistor / ADC view
    else if (d->view_mode == LCD_VIEW_PHOTO)
    {
        if (d->has_photo)
        {
            // Example: "Light:  45.0%"
            snprintf(line0, sizeof(line0), "Light: %5.1f%%", d->photo_percent);
            // Example: "ADC: 1234"
            snprintf(line1, sizeof(line1), "ADC: %4u", (unsigned)d->photo_raw);
        }
        else
        {
            snprintf(line0, sizeof(line0), "Light:  --.-%%");
            snprintf(line1, sizeof(line1), "ADC: ----");
        }
    }
    // NORMAL mode - environmental view: display sensor data (default)
    else
    {
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
    }

    // If nothing changed, do not touch LCD (prevents blinking)
    if (strncmp(line0, last0, 16) == 0 && strncmp(line1, last1, 16) == 0)
        return;

    // Draw (no lcd_clear()!)
    lcd_set_cursor(0, 0);
    print_padded(line0);
    lcd_set_cursor(1, 0);
    print_padded(line1);

    // Update cache
    strncpy(last0, line0, 16);
    last0[16] = 0;
    strncpy(last1, line1, 16);
    last1[16] = 0;
}

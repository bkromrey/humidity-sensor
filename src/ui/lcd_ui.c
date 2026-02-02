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
    // Calling lcd_clear() every frame causes visible flicker on HD44780 LCDs.
    // Strategy:
    // 1) Never clear on normal renders; we overwrite both lines with padding.
    // 2) Keep a tiny "last frame" buffer; if nothing changed, do nothing.

    static char last0[17] = {0};
    static char last1[17] = {0};

    if (!d)
    {
        // Explicit clear request
        lcd_clear();
        last0[0] = 0;
        last1[0] = 0;
        return;
    }

    char line0[17] = {0};
    char line1[17] = {0};

    // Decide what to render into line0/line1
    if (d->mode == LCD_MODE_LOADING)
    {
        snprintf(line0, sizeof(line0), "Loading...");
        line1[0] = '\0';
    }
    else if (d->mode == LCD_MODE_ERROR)
    {
        snprintf(line0, sizeof(line0), "%.*s", 16, d->line1 ? d->line1 : "ERROR");
        snprintf(line1, sizeof(line1), "%.*s", 16, d->line2 ? d->line2 : "");
    }
    else if (d->view_mode == LCD_VIEW_TEXT)
    {
        snprintf(line0, sizeof(line0), "%.*s", 16, d->line1 ? d->line1 : "");
        snprintf(line1, sizeof(line1), "%.*s", 16, d->line2 ? d->line2 : "");
    }
    else
    {
        // NORMAL mode - environmental view
        if (d->has_temp)
        {
            if (d->temp_unit == TEMP_F)
                snprintf(line0, sizeof(line0), "Temp.: %4.1fF", d->temp_value);
            else
                snprintf(line0, sizeof(line0), "Temp.: %4.1fC", d->temp_value);
        }
        else
        {
            snprintf(line0, sizeof(line0), "Temp.: --.-");
        }

        if (d->has_humidity)
            snprintf(line1, sizeof(line1), "Humidity: %4.1f%%", d->humidity_percent);
        else
            snprintf(line1, sizeof(line1), "Humidity: --.-%%");
    }

    // If content didn't change (up to 16 chars), skip I2C writes completely
    if (strncmp(last0, line0, 16) == 0 && strncmp(last1, line1, 16) == 0)
        return;

    // Save "frame" (truncate to 16 chars)
    snprintf(last0, sizeof(last0), "%.*s", 16, line0);
    snprintf(last1, sizeof(last1), "%.*s", 16, line1);

    // Overwrite both lines (no lcd_clear -> no flicker)
    lcd_set_cursor(0, 0);
    print_padded(line0);
    lcd_set_cursor(1, 0);
    print_padded(line1);
}

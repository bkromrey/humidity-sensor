#include "ui/lcd_ui.h"
#include "hardware/lcd.h"
#include <stdio.h>

static void print_padded(const char *s)
{
    // печать максимум 16 символов + добивка пробелами
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

void lcd_env_render(const lcd_env_data_t *d)
{
    lcd_clear();

    if (!d)
        return;

    if (d->mode == LCD_MODE_LOADING)
    {
        lcd_set_cursor(0, 0);
        print_padded("Loading...");
        lcd_set_cursor(1, 0);
        print_padded("");
        return;
    }

    if (d->mode == LCD_MODE_ERROR)
    {
        lcd_set_cursor(0, 0);
        print_padded(d->line1 ? d->line1 : "ERROR");
        lcd_set_cursor(1, 0);
        print_padded(d->line2 ? d->line2 : "");
        return;
    }

    // NORMAL
    if (d->view_mode == LCD_VIEW_TEXT)
    {
        lcd_set_cursor(0, 0);
        print_padded(d->line1 ? d->line1 : "");
        lcd_set_cursor(1, 0);
        print_padded(d->line2 ? d->line2 : "");
        return;
    }

    // ENV
    char line0[17] = {0};
    char line1[17] = {0};

    // TEMP -> line0
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

    // HUMIDITY -> line1
    if (d->has_humidity)
    {
        snprintf(line1, sizeof(line1), "Humidity: %4.1f%%", d->humidity_percent);
    }
    else
    {
        snprintf(line1, sizeof(line1), "Humidity: --.-%%");
    }

    lcd_set_cursor(0, 0);
    print_padded(line0);
    lcd_set_cursor(1, 0);
    print_padded(line1);
}

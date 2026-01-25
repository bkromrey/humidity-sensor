#include "pico/stdlib.h"
#include <stdbool.h>
#include "ui/lcd_ui.h" //  lcd_env_data_t + lcd_env_render

// 10 pair of test values
static const float kTempsC[10] = {18.2f, 19.0f, 20.5f, 21.1f, 22.8f, 23.4f, 24.0f, 25.3f, 26.1f, 27.0f};
static const float kHumPct[10] = {35.0f, 38.5f, 41.0f, 44.2f, 47.8f, 50.0f, 52.3f, 55.5f, 58.0f, 60.0f};

void mock_ui_run_10s(void)
{
    // 12 шагов * ~833ms ≈ 10 секунд
    const uint32_t step_ms = 833;

    lcd_env_data_t ui = {0};

    for (int step = 0; step < 12; ++step)
    {
        if (step == 0)
        {
            // TEXT
            ui.mode = LCD_MODE_NORMAL;
            ui.view_mode = LCD_VIEW_TEXT;
            ui.line1 = "custom message";
            ui.line2 = "test message";
            lcd_env_render(&ui);
        }
        else if (step == 1)
        {
            // ERROR
            ui.mode = LCD_MODE_ERROR;
            ui.view_mode = LCD_VIEW_TEXT;
            ui.line1 = "System message";
            ui.line2 = "some error";
            lcd_env_render(&ui);
        }
        else
        {
            // ENV (10 значений)
            int idx = step - 2; // 0..9

            ui.mode = LCD_MODE_NORMAL;
            ui.view_mode = LCD_VIEW_ENV;

            ui.has_temp = true;
            ui.temp_unit = TEMP_C;
            ui.temp_value = kTempsC[idx];

            ui.has_humidity = true;
            ui.humidity_percent = kHumPct[idx];

            // clear text lines
            ui.line1 = NULL;
            ui.line2 = NULL;

            lcd_env_render(&ui);
        }

        sleep_ms(step_ms);
    }

    static const float kTemp = 24.5f;
    static const float kHum = 52.0f;
    // ---------- 1) No temp ----------
    ui.mode = LCD_MODE_NORMAL;
    ui.view_mode = LCD_VIEW_ENV;

    ui.has_temp = false; // ← No temp
    ui.has_humidity = true;
    ui.humidity_percent = kHum;

    lcd_env_render(&ui);
    sleep_ms(step_ms);

    // ---------- 2) No humidity ----------
    ui.has_temp = true;
    ui.temp_unit = TEMP_C;
    ui.temp_value = kTemp;

    ui.has_humidity = false; // ← No humidity

    lcd_env_render(&ui);
    sleep_ms(step_ms);

    // ---------- 3) no both ----------
    ui.has_temp = false;
    ui.has_humidity = false;

    lcd_env_render(&ui);
    sleep_ms(step_ms);

    // ---------- 4) back to normal) ----------
    ui.has_temp = true;
    ui.temp_value = kTemp;

    ui.has_humidity = true;
    ui.humidity_percent = kHum;

    lcd_env_render(&ui);
    sleep_ms(step_ms);
}
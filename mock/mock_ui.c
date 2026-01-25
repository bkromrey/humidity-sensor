/**
 * Mock UI test runner for LCD display.
 * Demonstrates all display modes and views by cycling through test data.
 * Total runtime: approximately 16 seconds.
 */

#include "pico/stdlib.h"
#include <stdbool.h>
#include "ui/lcd_ui.h" //  lcd_env_data_t + lcd_env_render

// Test data: 10 temperature values in Celsius
static const float kTempsC[10] = {18.2f, 19.0f, 20.5f, 21.1f, 22.8f, 23.4f, 24.0f, 25.3f, 26.1f, 27.0f};

// Test data: 10 humidity percentage values
static const float kHumPct[10] = {35.0f, 38.5f, 41.0f, 44.2f, 47.8f, 50.0f, 52.3f, 55.5f, 58.0f, 60.0f};

/**
 * Runs a demonstration of all LCD display modes.
 * 
 * Test sequence (16 steps total, ~833ms each):
 * Step 0: TEXT mode - displays custom text messages
 * Step 1: ERROR mode - displays error message
 * Steps 2-11: ENV mode - cycles through 10 temperature/humidity pairs
 * Step 12: ENV mode - temperature only (no humidity data)
 * Step 13: ENV mode - humidity only (no temperature data)
 * Step 14: ENV mode - no sensor data (all missing)
 * Step 15: ENV mode - both sensors back to normal
 * 
 * This function tests:
 * - All display modes (NORMAL, ERROR)
 * - All view modes (TEXT, ENV)
 * - Missing sensor data handling
 * - Data formatting and display updates
 */
void mock_ui_run_lcd(void)
{
    // 12 steps * ~833ms ≈ 10 seconds (initial sequence)
    const uint32_t step_ms = 833;

    lcd_env_data_t ui = {0};

    // Main test sequence: TEXT, ERROR, and 10 ENV readings
    for (int step = 0; step < 12; ++step)
    {
        if (step == 0)
        {
            // TEXT mode test
            ui.mode = LCD_MODE_NORMAL;
            ui.view_mode = LCD_VIEW_TEXT;
            ui.line1 = "custom message";
            ui.line2 = "test message";
            lcd_env_render(&ui);
        }
        else if (step == 1)
        {
            // ERROR mode test
            ui.mode = LCD_MODE_ERROR;
            ui.view_mode = LCD_VIEW_TEXT;
            ui.line1 = "System message";
            ui.line2 = "some error";
            lcd_env_render(&ui);
        }
        else
        {
            // ENV mode: cycle through 10 sensor readings
            int idx = step - 2; // 0..9

            ui.mode = LCD_MODE_NORMAL;
            ui.view_mode = LCD_VIEW_ENV;

            ui.has_temp = true;
            ui.temp_unit = TEMP_C;
            ui.temp_value = kTempsC[idx];

            ui.has_humidity = true;
            ui.humidity_percent = kHumPct[idx];

            // Clear text lines (not used in ENV mode)
            ui.line1 = NULL;
            ui.line2 = NULL;

            lcd_env_render(&ui);
        }

        sleep_ms(step_ms);
    }

    // Test constants for missing data scenarios
    static const float kTemp = 24.5f;
    static const float kHum = 52.0f;

    // ---------- Test 1: Missing temperature ----------
    ui.mode = LCD_MODE_NORMAL;
    ui.view_mode = LCD_VIEW_ENV;

    ui.has_temp = false; // ← No temp
    ui.has_humidity = true;
    ui.humidity_percent = kHum;

    lcd_env_render(&ui);
    sleep_ms(step_ms);

    // ---------- Test 2: Missing humidity ----------
    ui.has_temp = true;
    ui.temp_unit = TEMP_C;
    ui.temp_value = kTemp;

    ui.has_humidity = false; // ← No humidity

    lcd_env_render(&ui);
    sleep_ms(step_ms);

    // ---------- Test 3: Both sensors missing ----------
    ui.has_temp = false;
    ui.has_humidity = false;

    lcd_env_render(&ui);
    sleep_ms(step_ms);

    // ---------- Test 4: Return to normal (both sensors present) ----------
    ui.has_temp = true;
    ui.temp_value = kTemp;

    ui.has_humidity = true;
    ui.humidity_percent = kHum;

    lcd_env_render(&ui);
    sleep_ms(step_ms);
}

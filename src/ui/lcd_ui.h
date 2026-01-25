#pragma once
#include <stdbool.h>

typedef enum
{
    LCD_MODE_NORMAL = 0,
    LCD_MODE_LOADING,
    LCD_MODE_ERROR
} lcd_mode_t;

typedef enum
{
    LCD_VIEW_ENV = 0,
    LCD_VIEW_TEXT
} lcd_view_t;

typedef enum
{
    TEMP_C = 0,
    TEMP_F
} temp_unit_t;

typedef struct
{
    lcd_mode_t mode;
    lcd_view_t view_mode;

    bool has_temp;
    float temp_value;
    temp_unit_t temp_unit;

    bool has_humidity;
    float humidity_percent;

    const char *line1;
    const char *line2;
} lcd_env_data_t;

void lcd_env_render(const lcd_env_data_t *data);
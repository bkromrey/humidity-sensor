#pragma once
#include <stdint.h>
#include <stdbool.h>
#define LCD_CHAR_DEGREE 1
typedef struct
{
    void *i2c; 
    uint8_t addr;
    bool backlight;
    uint8_t cols;
    uint8_t rows;
} lcd_i2c_t;

void lcd_i2c_init(lcd_i2c_t *lcd, void *i2c_inst, uint8_t addr, uint8_t cols, uint8_t rows);
void lcd_i2c_set_backlight(lcd_i2c_t *lcd, bool on);
void lcd_i2c_clear(lcd_i2c_t *lcd);
void lcd_i2c_home(lcd_i2c_t *lcd);
void lcd_i2c_set_cursor(lcd_i2c_t *lcd, uint8_t col, uint8_t row);
void lcd_i2c_write_str(lcd_i2c_t *lcd, const char *s);
void lcd_i2c_write_char(lcd_i2c_t *lcd, char c);
void lcd_create_char( lcd_i2c_t *lcd, uint8_t location, uint8_t *charmap);
extern uint8_t degree_symbol[8];
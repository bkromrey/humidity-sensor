#include "lcd.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "../config.h"

// commands
static const int LCD_CLEARDISPLAY = 0x01;
static const int LCD_ENTRYMODESET = 0x04;
static const int LCD_DISPLAYCONTROL = 0x08;
static const int LCD_FUNCTIONSET = 0x20;

static const int LCD_ENTRYLEFT = 0x02;
static const int LCD_DISPLAYON = 0x04;
static const int LCD_2LINE = 0x08;
static const int LCD_BACKLIGHT = 0x08;
static const int LCD_ENABLE_BIT = 0x04;

#define LCD_CHARACTER  1
#define LCD_COMMAND    0

static int addr = LCD_I2C_ADDR;

static void i2c_write_byte(uint8_t val) {
    i2c_write_blocking(i2c0, addr, &val, 1, false);
}

static void lcd_toggle_enable(uint8_t val) {
    sleep_us(600);
    i2c_write_byte(val | LCD_ENABLE_BIT);
    sleep_us(600);
    i2c_write_byte(val & ~LCD_ENABLE_BIT);
    sleep_us(600);
}

static void lcd_send_byte(uint8_t val, int mode) {
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low  = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    i2c_write_byte(high);
    lcd_toggle_enable(high);
    i2c_write_byte(low);
    lcd_toggle_enable(low);
}

void lcd_clear(void) {
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

void lcd_set_cursor(int line, int position) {
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte(val, LCD_COMMAND);
}

void lcd_string(const char *s) {
    while (*s) {
        lcd_send_byte(*s++, LCD_CHARACTER);
    }
}

void lcd_init(void) {
    sleep_ms(50);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x02, LCD_COMMAND);

    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    lcd_clear();
}

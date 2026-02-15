#include "hardware/lcd_i2c.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <string.h>

/**
 * I2C driver for LCD display, which provides 4-bit write operations, basic commands, 
 * and helper functions for printing text and custom characters.
 */

#define PIN_RS 0x01
#define PIN_RW 0x02
#define PIN_EN 0x04
#define PIN_BL 0x08

/**
 * Returns the backlight control bit based on the current backlight state
 */
static inline uint8_t bl_mask(const lcd_i2c_t *lcd) { return lcd->backlight ? PIN_BL : 0; }

/**
 * Sends a single byte to the LCD over the configured I2C bus
 */
static void i2c_write_byte(lcd_i2c_t *lcd, uint8_t v)
{
    i2c_write_blocking((i2c_inst_t *)lcd->i2c, lcd->addr, &v, 1, false);
}

/**
 * Generates an enable pulse for the LCD to latch the current data, then, the LCD reads the data
 */
static void pulse_enable(lcd_i2c_t *lcd, uint8_t data)
{
    i2c_write_byte(lcd, data | PIN_EN);
    sleep_us(1);
    i2c_write_byte(lcd, data & ~PIN_EN);
    sleep_us(50);
}

/**
 * Writes a 4-bit value to the LCD data bus
 */
static void write4(lcd_i2c_t *lcd, uint8_t nibble, uint8_t rs)
{
    uint8_t data = (nibble & 0xF0) | bl_mask(lcd) | (rs ? PIN_RS : 0);
    i2c_write_byte(lcd, data);
    pulse_enable(lcd, data);
}

/**
 * Sends a 8-bit value to the LCD using two 4-bit transfers
 * RS selects whether this is a command (0) or data (1)
 */
static void lcd_send(lcd_i2c_t *lcd, uint8_t value, uint8_t rs)
{
    write4(lcd, value & 0xF0, rs);
    write4(lcd, (value << 4) & 0xF0, rs);
}

/**
 * Sends a command byte to the LCD
 */
static void lcd_cmd(lcd_i2c_t *lcd, uint8_t cmd)
{
    lcd_send(lcd, cmd, 0);
}

/**
 * Sends a character to the LCD.
 */
static void lcd_data(lcd_i2c_t *lcd, uint8_t d)
{
    lcd_send(lcd, d, 1);
}

/**
 * Turns the LCD backlight on or off
 */
void lcd_i2c_set_backlight(lcd_i2c_t *lcd, bool on)
{
    lcd->backlight = on;
    i2c_write_byte(lcd, bl_mask(lcd));
}

/**
 * Initializes the LCD over I2C in 4-bit mode.
 *
 * @param lcd     Pointer to lcd_i2c_t
 * @param i2c_inst I2C instance used for communication
 * @param addr    I2C address of the LCD backpack
 * @param cols    Number of columns
 * @param rows    Number of rows
 */
void lcd_i2c_init(lcd_i2c_t *lcd, void *i2c_inst, uint8_t addr, uint8_t cols, uint8_t rows)
{
    lcd->i2c = i2c_inst;
    lcd->addr = addr;
    lcd->cols = cols;
    lcd->rows = rows;
    lcd->backlight = true;

    sleep_ms(50);

    // 4-bit init sequence
    write4(lcd, 0x30, 0);
    sleep_ms(5);
    write4(lcd, 0x30, 0);
    sleep_us(150);
    write4(lcd, 0x30, 0);
    sleep_us(150);
    write4(lcd, 0x20, 0);
    sleep_us(150); // 4-bit

    // Function set: 4-bit, 2-line, 5x8
    lcd_cmd(lcd, 0x28);
    // Display off
    lcd_cmd(lcd, 0x08);
    // Clear
    lcd_cmd(lcd, 0x01);
    sleep_ms(2);
    // Entry mode
    lcd_cmd(lcd, 0x06);
    // Display on, cursor off, blink off
    lcd_cmd(lcd, 0x0C);

    lcd_i2c_set_backlight(lcd, true);
}

/**
 * Clears the entire display and returns the cursor to the home position
 */
void lcd_i2c_clear(lcd_i2c_t *lcd)
{
    lcd_cmd(lcd, 0x01);
    sleep_ms(2);
}

/**
 * Moves the cursor to home position, not clearing the display
 */
void lcd_i2c_home(lcd_i2c_t *lcd)
{
    lcd_cmd(lcd, 0x02);
    sleep_ms(2);
}

/**
 * Sets the cursor position to the given column and row
 */
void lcd_i2c_set_cursor(lcd_i2c_t *lcd, uint8_t col, uint8_t row)
{
    static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row >= lcd->rows)
        row = 0;
    lcd_cmd(lcd, 0x80 | (col + row_offsets[row]));
}

/**
 * Writes a single character at the current cursor position
 */
void lcd_i2c_write_char(lcd_i2c_t *lcd, char c)
{
    lcd_data(lcd, (uint8_t)c);
}

/**
 * Writes a string starting at the current cursor position
 */
void lcd_i2c_write_str(lcd_i2c_t *lcd, const char *s)
{
    while (*s)
        lcd_data(lcd, (uint8_t)*s++);
}

/**
 * 5x8 pixel bitmap for a degree symbol
 */
uint8_t degree_symbol[8] = {
    0b00110,
    0b01001,
    0b01001,
    0b00110,
    0b00000,
    0b00000,
    0b00000,
    0b00000

};

/**
 * Stores a custom character bitmap in the LCD CGRAM
 *
 * @param location CGRAM slot (0–7)
 * @param charmap  Pointer to an array representing the character
 */
void lcd_create_char( lcd_i2c_t *lcd, uint8_t location, uint8_t *charmap){
    location &= 0x7;
    lcd_cmd(lcd, 0x40 | (location << 3));
    for (int i=0; i<8;i++)
        lcd_i2c_write_char(lcd, charmap[i]);
}
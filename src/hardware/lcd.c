/**
 * Low-level LCD functions for 16x2 LCD with I2C backpack.
 * Implements HD44780-compatible LCD driver using I2C communication.
 * Uses 4-bit mode to minimize I2C traffic.
 */

#include "lcd.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "../config.h"

// LCD command codes
static const int LCD_CLEARDISPLAY = 0x01;   // Clear display command
static const int LCD_ENTRYMODESET = 0x04;   // Set entry mode command
static const int LCD_DISPLAYCONTROL = 0x08; // Display control command
static const int LCD_FUNCTIONSET = 0x20;    // Function set command

// LCD control flags
static const int LCD_ENTRYLEFT = 0x02;      // Entry mode: left-to-right text
static const int LCD_DISPLAYON = 0x04;      // Display on flag
static const int LCD_2LINE = 0x08;          // 2-line display mode
static const int LCD_BACKLIGHT = 0x08;      // Backlight control bit
static const int LCD_ENABLE_BIT = 0x04;     // Enable bit for LCD commands

// Mode flags for lcd_send_byte
#define LCD_CHARACTER  1  // Data mode (character data)
#define LCD_COMMAND    0  // Command mode (instruction)

/**
 * Writes a single byte to the I2C LCD device.
 * 
 * @param val The byte value to write
 */
static void i2c_write_byte(uint8_t val) {
    i2c_write_blocking(LCD_I2C_PORT, LCD_I2C_ADDR, &val, 1, false);
}

/**
 * Toggles the enable bit to latch data/command into the LCD.
 * The LCD reads data on the falling edge of the enable bit.
 * Includes required timing delays for LCD processing.
 * 
 * @param val The data byte with enable bit to toggle
 */
static void lcd_toggle_enable(uint8_t val) {
    sleep_us(600);
    i2c_write_byte(val | LCD_ENABLE_BIT);
    sleep_us(600);
    i2c_write_byte(val & ~LCD_ENABLE_BIT);
    sleep_us(600);
}

/**
 * Sends a byte to the LCD in 4-bit mode.
 * Splits the byte into high and low nibbles and sends them sequentially.
 * Keeps backlight enabled during transmission.
 * 
 * @param val The byte value to send (command or character)
 * @param mode LCD_CHARACTER for data, LCD_COMMAND for instructions
 */
static void lcd_send_byte(uint8_t val, int mode) {
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low  = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    i2c_write_byte(high);
    lcd_toggle_enable(high);
    i2c_write_byte(low);
    lcd_toggle_enable(low);
}

/**
 * Clears the entire LCD display.
 * Removes all characters and resets cursor to home position (0,0).
 */
void lcd_clear(void) {
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

/**
 * Sets the cursor position on the LCD display.
 * Line 0 starts at DDRAM address 0x00 (command 0x80).
 * Line 1 starts at DDRAM address 0x40 (command 0xC0).
 * 
 * @param line Line number (0 for first line, 1 for second line)
 * @param position Character position on the line (0-15)
 */
void lcd_set_cursor(int line, int position) {
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte(val, LCD_COMMAND);
}

/**
 * Writes a null-terminated string to the LCD at the current cursor position
 * and does not automatically wrap to the next line.
 * 
 * @param s Null-terminated string to display
 */
void lcd_string(const char *s) {
    while (*s) {
        lcd_send_byte(*s++, LCD_CHARACTER);
    }
}

/**
 * Initializes the LCD display in 4-bit mode.
 * Performs the initialization sequence required by HD44780-compatible displays:
 * 1. Waits 50ms for power stabilization
 * 2. Sends reset sequence (0x03 three times)
 * 3. Switches to 4-bit mode (0x02)
 * 4. Configures display settings (2-line mode, display on, left-to-right entry)
 * 5. Clears the display
 * 
 * Must be called before any other LCD functions.
 */
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

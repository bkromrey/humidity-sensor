#ifndef LCD_H
#define LCD_H

#include <stdint.h>

// Maximum number of lines on the LCD display
#define LCD_MAX_LINES 2

// Maximum number of characters per line
#define LCD_MAX_CHARS 16

/**
 * Initializes the LCD display hardware.
 * Must be called before any other LCD functions.
 * Configures GPIO pins and sets up the display in 4-bit mode.
 */
void lcd_init(void);

/**
 * Clears the entire LCD display.
 * Removes all characters and resets cursor to home position (0,0).
 */
void lcd_clear(void);

/**
 * Sets the cursor position on the LCD display.
 * 
 * @param line Line number (0 for first line, 1 for second line)
 * @param position Character position on the line (0-15)
 */
void lcd_set_cursor(int line, int position);

/**
 * Writes a string to the LCD at the current cursor position.
 * Does not automatically wrap to the next line or add padding.
 * 
 * @param s Null-terminated string to display
 */
void lcd_string(const char *s);

#endif

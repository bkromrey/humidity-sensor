#ifndef LCD_H
#define LCD_H

#include <stdint.h>

#define LCD_MAX_LINES 2
#define LCD_MAX_CHARS 16

void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(int line, int position);
void lcd_string(const char *s);

#endif

#ifndef LCD_H
#define LCD_H

#include <stdbool.h>
#include <stdint.h>

void lcd_init(void);
void lcd_clear(void);
void lcd_putc(uint8_t position, char c, bool decimal_point);

/* Decimal-point bit OR-ed into the 14-segment code before lcd_put. */
#define LCD_DP 0x0008u

#endif

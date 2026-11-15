#include "lcd.h"
#include "regs.h"

/*
 * Segment table for the 14-segment glass of the STM8L-DISCOVERY (MB915).
 * The 16 bit code is four nibbles, MSB first:
 *   {E,D,P,J} {M,C,COL,H} {B,A,K,N} {G,F,Q,DP}
 * Tables taken from the ST STM8L-DISCOVERY glass driver.
 */
static const uint16_t number_map[10] = {
    0x5F00, 0x4200, 0xF500, 0x6700, 0xEA00,
    0xAF00, 0xBF00, 0x4600, 0xFF00, 0xEF00
};

static const uint16_t letter_map[26] = {
    0xFE00, 0x6711, 0x1D00, 0x4711, 0x9D00, 0x9C00, 0x3F00, 0xFA00, 0x0011,
    0x5300, 0x9844, 0x1900, 0x5A42, 0x5A06, 0x5F00, 0xFC00, 0x5F04, 0xFC04,
    0xAF00, 0x0411, 0x5B00, 0x18C0, 0x5A84, 0x00C6, 0x0052, 0x05C0
};

static uint16_t char_code(char c)
{
    if (c >= '0' && c <= '9')
        return number_map[c - '0'];
    if (c >= 'A' && c <= 'Z')
        return letter_map[c - 'A'];
    if (c >= 'a' && c <= 'z')
        return letter_map[c - 'a'];
    if (c == '-')
        return 0xA000;
    return 0x0000;
}

void lcd_put(uint8_t position, uint16_t ch)
{
    uint8_t digit[4];
    uint8_t i, j;

    for (i = 12, j = 0; j < 4; i -= 4, j++)
        digit[j] = (uint8_t)((ch >> i) & 0x0F);

    switch (position) {
    case 1:
        LCD_RAM(0) = (uint8_t)((LCD_RAM(0) & 0xFC) | (digit[0] & 0x03));
        LCD_RAM(2) = (uint8_t)((LCD_RAM(2) & 0x3F) | ((digit[0] << 4) & 0xC0));
        LCD_RAM(3) = (uint8_t)((LCD_RAM(3) & 0xCF) | ((digit[1] << 4) & 0x30));
        LCD_RAM(6) = (uint8_t)((LCD_RAM(6) & 0xF3) | (digit[1] & 0x0C));
        LCD_RAM(7) = (uint8_t)((LCD_RAM(7) & 0xFC) | (digit[2] & 0x03));
        LCD_RAM(9) = (uint8_t)((LCD_RAM(9) & 0x3F) | ((digit[2] << 4) & 0xC0));
        LCD_RAM(10) = (uint8_t)((LCD_RAM(10) & 0xCF) | ((digit[3] << 2) & 0x30));
        LCD_RAM(13) = (uint8_t)((LCD_RAM(13) & 0xF3) | ((digit[3] << 2) & 0x0C));
        break;
    case 2:
        LCD_RAM(0) = (uint8_t)((LCD_RAM(0) & 0xF3) | ((digit[0] << 2) & 0x0C));
        LCD_RAM(2) = (uint8_t)((LCD_RAM(2) & 0xCF) | ((digit[0] << 2) & 0x30));
        LCD_RAM(3) = (uint8_t)((LCD_RAM(3) & 0x3F) | ((digit[1] << 6) & 0xC0));
        LCD_RAM(6) = (uint8_t)((LCD_RAM(6) & 0xFC) | ((digit[1] >> 2) & 0x03));
        LCD_RAM(7) = (uint8_t)((LCD_RAM(7) & 0xF3) | ((digit[2] << 2) & 0x0C));
        LCD_RAM(9) = (uint8_t)((LCD_RAM(9) & 0xCF) | ((digit[2] << 2) & 0x30));
        LCD_RAM(10) = (uint8_t)((LCD_RAM(10) & 0x3F) | ((digit[3] << 4) & 0xC0));
        LCD_RAM(13) = (uint8_t)((LCD_RAM(13) & 0xFC) | (digit[3] & 0x03));
        break;
    case 3:
        LCD_RAM(0) = (uint8_t)((LCD_RAM(0) & 0xCF) | ((digit[0] << 4) & 0x30));
        LCD_RAM(2) = (uint8_t)((LCD_RAM(2) & 0xF3) | (digit[0] & 0x0C));
        LCD_RAM(4) = (uint8_t)((LCD_RAM(4) & 0xFC) | (digit[1] & 0x03));
        LCD_RAM(5) = (uint8_t)((LCD_RAM(5) & 0x3F) | ((digit[1] << 4) & 0xC0));
        LCD_RAM(7) = (uint8_t)((LCD_RAM(7) & 0xCF) | ((digit[2] << 4) & 0x30));
        LCD_RAM(9) = (uint8_t)((LCD_RAM(9) & 0xF3) | (digit[2] & 0x0C));
        LCD_RAM(11) = (uint8_t)((LCD_RAM(11) & 0xFC) | ((digit[3] >> 2) & 0x03));
        LCD_RAM(12) = (uint8_t)((LCD_RAM(12) & 0x3F) | ((digit[3] << 6) & 0xC0));
        break;
    case 4:
        LCD_RAM(0) = (uint8_t)((LCD_RAM(0) & 0x3F) | ((digit[0] << 6) & 0xC0));
        LCD_RAM(2) = (uint8_t)((LCD_RAM(2) & 0xFC) | ((digit[0] >> 2) & 0x03));
        LCD_RAM(4) = (uint8_t)((LCD_RAM(4) & 0xF3) | ((digit[1] << 2) & 0x0C));
        LCD_RAM(5) = (uint8_t)((LCD_RAM(5) & 0xCF) | ((digit[1] << 2) & 0x30));
        LCD_RAM(7) = (uint8_t)((LCD_RAM(7) & 0x3F) | ((digit[2] << 6) & 0xC0));
        LCD_RAM(9) = (uint8_t)((LCD_RAM(9) & 0xFC) | ((digit[2] >> 2) & 0x03));
        LCD_RAM(11) = (uint8_t)((LCD_RAM(11) & 0xF3) | (digit[3] & 0x0C));
        LCD_RAM(12) = (uint8_t)((LCD_RAM(12) & 0xCF) | ((digit[3] << 4) & 0x30));
        break;
    case 5:
        LCD_RAM(1) = (uint8_t)((LCD_RAM(1) & 0xFC) | (digit[0] & 0x03));
        LCD_RAM(1) = (uint8_t)((LCD_RAM(1) & 0x3F) | ((digit[0] << 4) & 0xC0));
        LCD_RAM(4) = (uint8_t)((LCD_RAM(4) & 0xCF) | ((digit[1] << 4) & 0x30));
        LCD_RAM(5) = (uint8_t)((LCD_RAM(5) & 0xF3) | (digit[1] & 0x0C));
        LCD_RAM(8) = (uint8_t)((LCD_RAM(8) & 0xFE) | (digit[2] & 0x01));
        LCD_RAM(8) = (uint8_t)((LCD_RAM(8) & 0x3F) | ((digit[2] << 4) & 0xC0));
        LCD_RAM(11) = (uint8_t)((LCD_RAM(11) & 0xEF) | ((digit[3] << 2) & 0x10));
        LCD_RAM(12) = (uint8_t)((LCD_RAM(12) & 0xF3) | ((digit[3] << 2) & 0x0C));
        break;
    case 6:
        LCD_RAM(1) = (uint8_t)((LCD_RAM(1) & 0xF3) | ((digit[0] << 2) & 0x0C));
        LCD_RAM(1) = (uint8_t)((LCD_RAM(1) & 0xCF) | ((digit[0] << 2) & 0x30));
        LCD_RAM(4) = (uint8_t)((LCD_RAM(4) & 0x3F) | ((digit[1] << 6) & 0xC0));
        LCD_RAM(5) = (uint8_t)((LCD_RAM(5) & 0xFC) | ((digit[1] >> 2) & 0x03));
        LCD_RAM(8) = (uint8_t)((LCD_RAM(8) & 0xFB) | ((digit[2] << 2) & 0x04));
        LCD_RAM(8) = (uint8_t)((LCD_RAM(8) & 0xCF) | ((digit[2] << 2) & 0x30));
        LCD_RAM(11) = (uint8_t)((LCD_RAM(11) & 0xBF) | ((digit[3] << 4) & 0x40));
        LCD_RAM(12) = (uint8_t)((LCD_RAM(12) & 0xFC) | (digit[3] & 0x03));
        break;
    default:
        break;
    }
}

void lcd_putc(uint8_t position, char c, bool decimal_point)
{
    uint16_t code = char_code(c);

    if (decimal_point)
        code |= LCD_DP;
    lcd_put(position, code);
}

void lcd_clear(void)
{
    uint8_t i;

    for (i = 0; i < 22; i++)
        LCD_RAM(i) = 0;
}

void lcd_init(void)
{
    /* LCD is clocked from the RTC clock (LSE), 1/4 duty, 1/3 bias. */
    CLK_PCKENR2 |= 0x08;    /* LCD clock gate */
    LCD_FRQ = 0x0F;     /* prescaler 1, divider 31 */
    LCD_CR1 = 0x06;     /* 1/4 duty */
    LCD_CR2 = 0x28;     /* pulse on duration 1, contrast 3.0 V, internal source */
    LCD_CR3 = 0x00;     /* no dead time */
    LCD_PM0 = 0xFF;     /* segments 0..23 are on the glass */
    LCD_PM1 = 0xFF;
    LCD_PM2 = 0xFF;
    lcd_clear();
    LCD_CR3 |= 0x40;    /* enable */
}

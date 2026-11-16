#include <stdbool.h>
#include <stdint.h>

#include "lcd.h"
#include "regs.h"
#include "rtc.h"
#include "temp.h"
#include "time_conv.h"

/*
 * Mailbox: 8 bytes of RAM at 0x0000, left untouched by the SDCC startup
 * because the firmware is linked with DATA at 0x0008.
 *
 *   0   0xA5
 *   1   0x5A
 *   2   Unix time, little endian, 4 bytes
 *   6   CRC8 (poly 0x07, init 0x00) over bytes 0..5
 *   7   0xFF, reserved
 */
#define MB_BASE   ((volatile uint8_t *)0x0000u)
#define MB_MAGIC0 0xA5
#define MB_MAGIC1 0x5A

static uint8_t crc8(const uint8_t *p, uint8_t len)
{
    uint8_t crc = 0x00;
    uint8_t i;

    while (len--) {
        crc ^= *p++;
        for (i = 0; i < 8; i++)
            crc = (uint8_t)((crc & 0x80) ? ((crc << 1) ^ 0x07) : (crc << 1));
    }
    return crc;
}

static void apply_mailbox(void)
{
    uint32_t utime;

    if (MB_BASE[0] != MB_MAGIC0 || MB_BASE[1] != MB_MAGIC1)
        return;

    if (crc8((const uint8_t *)MB_BASE, 6) == MB_BASE[6]) {
        utime = (uint32_t)MB_BASE[2]
              | ((uint32_t)MB_BASE[3] << 8)
              | ((uint32_t)MB_BASE[4] << 16)
              | ((uint32_t)MB_BASE[5] << 24);
        rtc_set_unixtime(utime);
    }

    /* Consume the mailbox so a plain reset keeps the clock running. */
    MB_BASE[0] = 0x00;
    MB_BASE[1] = 0x00;
}

static void show_time(const time_struct_t *t)
{
    lcd_clear();
    lcd_putc(1, (char)('0' + t->hour / 10), false);
    lcd_putc(2, (char)('0' + t->hour % 10), true);
    lcd_putc(3, (char)('0' + t->minute / 10), false);
    lcd_putc(4, (char)('0' + t->minute % 10), true);
    lcd_putc(5, (char)('0' + t->second / 10), false);
    lcd_putc(6, (char)('0' + t->second % 10), false);
}

static void show_temp(int16_t tenths)
{
    bool negative = (tenths < 0);
    uint8_t intg;
    uint8_t frac;

    if (negative)
        tenths = (int16_t)-tenths;
    if (tenths > 999)
        tenths = 999;

    intg = (uint8_t)(tenths / 10);
    frac = (uint8_t)(tenths % 10);

    /*
     * Fixed layout: the decimal point always sits on position 3, so the dot
     * never moves between the time and temperature screens.  Position 5 holds
     * the "C" (positions 5 and 6 have no dot segment on this glass).
     */
    lcd_clear();
    if (negative)
        lcd_putc(1, '-', false);
    if (intg >= 10)
        lcd_putc(2, (char)('0' + intg / 10), false);
    lcd_putc(3, (char)('0' + intg % 10), true);
    lcd_putc(4, (char)('0' + frac), false);
    lcd_putc(5, 'C', false);
}

void main(void)
{
    time_struct_t t;
    int16_t temp = 0;
    uint8_t last_sec = 0xFF;

    rtc_init();
    lcd_init();
    temp_init();
    apply_mailbox();

    for (;;) {
        uint8_t sec = rtc_seconds();

        if (sec == last_sec)
            continue;
        last_sec = sec;

        rtc_get(&t);

        if ((uint8_t)(sec % 6) < 3) {
            /* first 3 s of each 6 s cycle: time */
            show_time(&t);
        } else {
            /* remaining 3 s: temperature */
            temp = temp_read_tenths();
            show_temp(temp);
        }
    }
}

#include "rtc.h"
#include "regs.h"

#define RTC_ISR1_INIT   0x80
#define RTC_ISR1_INITF  0x40

static uint8_t bin2bcd(uint8_t v)
{
    return (uint8_t)(((v / 10) << 4) | (v % 10));
}

static uint8_t bcd2bin(uint8_t v)
{
    return (uint8_t)(((v >> 4) * 10) + (v & 0x0F));
}

static void unlock(void)
{
    RTC_WPR = 0xCA;
    RTC_WPR = 0x53;
}

static void lock(void)
{
    RTC_WPR = 0xFF;
}

static void init_mode(bool on)
{
    if (on) {
        RTC_ISR1 |= RTC_ISR1_INIT;
        while (!(RTC_ISR1 & RTC_ISR1_INITF))
            ;
    } else {
        RTC_ISR1 &= (uint8_t)~RTC_ISR1_INIT;
        while (RTC_ISR1 & RTC_ISR1_INITF)
            ;
    }
}

/*
 * Enable the 32.768 kHz crystal and select it as RTC/LCD clock source.
 * Returns true when LSE started, false when the LSI fallback is used.
 */
static bool clock_setup(void)
{
    uint32_t i;

    CLK_PCKENR2 |= 0x04;            /* RTC clock gate */
    CLK_ECKCR |= 0x04;              /* LSE on */

    for (i = 0; i < 1000000UL; i++) {
        if (CLK_ECKCR & 0x08)       /* LSERDY */
            break;
    }

    if (CLK_ECKCR & 0x08) {
        CLK_CRTCR = 0x10;           /* LSE, divider 1 */
        return true;
    }

    /* Fallback: internal 38 kHz RC, approximately 148 * 256 division. */
    CLK_ICKCR |= 0x04;              /* LSI on */
    while (!(CLK_ICKCR & 0x08))
        ;
    CLK_CRTCR = 0x04;               /* LSI, divider 1 */
    return false;
}

void rtc_init(void)
{
    bool lse = clock_setup();
    time_struct_t t;

    t.year = 2027;
    t.month = 1;
    t.day = 1;
    t.hour = 0;
    t.minute = 0;
    t.second = 0;

    unlock();
    init_mode(true);

    if (lse) {
        RTC_APRER = 0x7F;           /* 128 */
        RTC_SPRERL = 0xFF;          /* 256 -> exactly 32768 */
    } else {
        RTC_APRER = 0x94;           /* 149 */
        RTC_SPRERL = 0xFF;          /* 256 -> about 38144 Hz */
    }
    RTC_SPRERH = 0x00;

    RTC_CR1 = 0x10;                 /* bypass shadow registers */

    RTC_TR1 = bin2bcd(t.second);
    RTC_TR2 = bin2bcd(t.minute);
    RTC_TR3 = bin2bcd(t.hour);
    RTC_DR1 = bin2bcd(t.day);
    RTC_DR2 = bin2bcd(t.month);
    RTC_DR3 = bin2bcd((uint8_t)(t.year % 100));

    init_mode(false);
    lock();
}

void rtc_get(time_struct_t *t)
{
    t->second = bcd2bin((uint8_t)(RTC_TR1 & 0x7F));
    t->minute = bcd2bin((uint8_t)(RTC_TR2 & 0x7F));
    t->hour = bcd2bin((uint8_t)(RTC_TR3 & 0x3F));
    t->day = bcd2bin((uint8_t)(RTC_DR1 & 0x3F));
    t->month = bcd2bin((uint8_t)(RTC_DR2 & 0x1F));
    t->year = (uint16_t)(2000 + bcd2bin(RTC_DR3));
}

uint8_t rtc_seconds(void)
{
    return bcd2bin((uint8_t)(RTC_TR1 & 0x7F));
}

void rtc_set_unixtime(uint32_t utime)
{
    time_struct_t t;

    unixtime_to_time(utime, &t);

    unlock();
    init_mode(true);

    RTC_TR1 = bin2bcd(t.second);
    RTC_TR2 = bin2bcd(t.minute);
    RTC_TR3 = bin2bcd(t.hour);
    RTC_DR1 = bin2bcd(t.day);
    RTC_DR2 = bin2bcd(t.month);
    RTC_DR3 = bin2bcd((uint8_t)(t.year % 100));

    init_mode(false);
    lock();
}

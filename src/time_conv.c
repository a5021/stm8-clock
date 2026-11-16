#include "time_conv.h"

#define JD0 2440588u

/* Unix time -> Gregorian calendar, Fliegel–Van Flandern day-number algorithm. */
void unixtime_to_time(uint32_t utime, time_struct_t *t)
{
    uint32_t ace, b, d, m;

    ace = (utime / 86400) + 32044 + JD0;
    b = (4 * ace + 3) / 146097;
    ace = ace - ((146097 * b) / 4);
    d = (4 * ace + 3) / 1461;
    ace = ace - ((1461 * d) / 4);
    m = (5 * ace + 2) / 153;
    t->day = (uint8_t)(ace - ((153 * m + 2) / 5) + 1);
    t->month = (uint8_t)(m + 3 - (12 * (m / 10)));
    t->year = (uint16_t)(100 * b + d - 4800 + (m / 10));
    t->hour = (uint8_t)((utime / 3600) % 24);
    t->minute = (uint8_t)((utime / 60) % 60);
    t->second = (uint8_t)(utime % 60);
}

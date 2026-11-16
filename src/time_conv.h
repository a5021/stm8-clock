#ifndef TIME_CONV_H
#define TIME_CONV_H

#include <stdint.h>

typedef struct {
    uint16_t year;       /* full year, e.g. 2027 */
    uint8_t month;       /* 1..12 */
    uint8_t day;         /* 1..31 */
    uint8_t hour;        /* 0..23 */
    uint8_t minute;      /* 0..59 */
    uint8_t second;      /* 0..59 */
} time_struct_t;

/* Gregorian calendar <-> Unix time (days since 1970-01-01 00:00:00 UTC). */
uint32_t time_to_unixtime(const time_struct_t *t);
void unixtime_to_time(uint32_t utime, time_struct_t *t);

#endif

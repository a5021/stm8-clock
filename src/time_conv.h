#ifndef TIME_CONV_H
#define TIME_CONV_H

#include <stdint.h>

typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} time_struct_t;

uint32_t time_to_unixtime(const time_struct_t *t);
void unixtime_to_time(uint32_t utime, time_struct_t *t);

#endif

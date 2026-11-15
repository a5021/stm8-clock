#ifndef RTC_H
#define RTC_H

#include <stdbool.h>
#include <stdint.h>
#include "time_conv.h"

bool rtc_init(void);
void rtc_get(time_struct_t *t);
uint8_t rtc_seconds(void);
void rtc_set_unixtime(uint32_t utime);

#endif

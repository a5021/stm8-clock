#ifndef RTC_H
#define RTC_H

#include <stdbool.h>
#include <stdint.h>
#include "time_conv.h"

void rtc_init(void);
void rtc_get(time_struct_t *t);      /* read current time from RTC registers */
uint8_t rtc_seconds(void);    /* seconds field only, for polling */
void rtc_set_unixtime(uint32_t utime);   /* write calendar from Unix time */

#endif

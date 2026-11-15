#ifndef TEMP_H
#define TEMP_H

#include <stdint.h>

/*
 * Calibration of the internal sensor: V25 and V25_TENTHS in temp.c hold the
 * single-point calibration, TEMP_OFFSET_TENTHS trims the final reading.
 */
#define TEMP_OFFSET_TENTHS  0

void temp_init(void);
int16_t temp_read_tenths(void);

#endif

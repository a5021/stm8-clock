#include "temp.h"
#include "regs.h"

/* Internal reference voltage, mV (typical). */
#define VREFINT_MV   1225
/*
 * Sensor voltage at 25.5 C, measured on this part (ADC counts and Vrefint).
 * The datasheet V25 spread is far too wide here, so the value was calibrated
 * against a reference thermometer instead.
 */
#define V25_MV       492
#define V25_TENTHS   255
/* Sensor slope, -4.3 mV/C, in mV per 10 C. */
#define SLOPE_MV10   43

/*
 * ADC1 fast channels: bit 4 in SQR1 is Vrefint, bit 5 is the temperature
 * sensor.  Both are sampled against VDDA, so Vrefint gives VDDA back.
 */
#define CH_VREFINT   0x10
#define CH_TEMP      0x20

static uint16_t adc_sample(uint8_t channel)
{
    ADC_SQR0 = channel;
    ADC_SR = 0xFE;                  /* clear EOC (write zero to it) */
    ADC_CR1 |= 0x02;                /* START, single conversion */
    while (!(ADC_SR & 0x01))
        ;
    return (uint16_t)((((uint16_t)ADC_DRH & 0x0F) << 8) | ADC_DRL);
}

void temp_init(void)
{
    uint16_t i;

    CLK_PCKENR2 |= 0x01;            /* ADC1 clock gate */
    ADC_CR1 = 0x00;
    ADC_CR2 = 0x80;                 /* single, 12 bit, prescaler 2 */
    ADC_CR3 = 0xFF;                 /* fast channels, 384 cycle sampling */
    ADC_SR = 0xFE;
    ADC_CR1 = 0x01;                 /* ADON */
    ADC_TRIGR1 = 0x30;              /* Vrefint and temperature sensor on */

    for (i = 0; i < 4000; i++)      /* sensor settling time */
        ;
}

int16_t temp_read_tenths(void)
{
    uint32_t vref = 0;
    uint32_t ts = 0;
    uint32_t mv;
    int32_t tenths;
    uint8_t i;

    for (i = 0; i < 8; i++) {
        vref += adc_sample(CH_VREFINT);
        ts += adc_sample(CH_TEMP);
    }
    vref >>= 3;
    ts >>= 3;

    if (vref == 0)
        return 0;

    /* Sensor voltage in mV: ts/4096 * VDDA, with VDDA = 1225*4096/vref. */
    mv = (ts * VREFINT_MV) / vref;

    /* T = 25.5 C + (V25 - V) / slope, tenths: 4.3 mV/C = 43 mV per 10 C. */
    tenths = V25_TENTHS + ((int32_t)V25_MV - (int32_t)mv) * 100L / SLOPE_MV10;

    return (int16_t)(tenths + TEMP_OFFSET_TENTHS);
}

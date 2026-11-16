#ifndef REGS_H
#define REGS_H

#include <stdint.h>

#define REG8(a) (*(volatile uint8_t *)(a))

/* Clock controller, RM0031 CLK registers at 0x50C0. */
#define CLK_CRTCR    REG8(0x50C1)
#define CLK_ICKCR    REG8(0x50C2)
#define CLK_PCKENR2  REG8(0x50C4)
#define CLK_ECKCR    REG8(0x50C6)

/* Real time clock, base 0x5140. */
#define RTC_TR1      REG8(0x5140)
#define RTC_TR2      REG8(0x5141)
#define RTC_TR3      REG8(0x5142)
#define RTC_DR1      REG8(0x5144)
#define RTC_DR2      REG8(0x5145)
#define RTC_DR3      REG8(0x5146)
#define RTC_CR1      REG8(0x5148)
#define RTC_ISR1     REG8(0x514C)
#define RTC_SPRERH   REG8(0x5150)
#define RTC_SPRERL   REG8(0x5151)
#define RTC_APRER    REG8(0x5152)
#define RTC_WPR      REG8(0x5159)

/* LCD controller, base 0x5400. */
#define LCD_CR1      REG8(0x5400)
#define LCD_CR2      REG8(0x5401)
#define LCD_CR3      REG8(0x5402)
#define LCD_FRQ      REG8(0x5403)
#define LCD_PM0      REG8(0x5404)
#define LCD_PM1      REG8(0x5405)
#define LCD_PM2      REG8(0x5406)
#define LCD_RAM_BASE 0x540C
#define LCD_RAM(i)   REG8(LCD_RAM_BASE + (i))

/* ADC1, base 0x5340. */
#define ADC_CR1      REG8(0x5340)
#define ADC_CR2      REG8(0x5341)
#define ADC_CR3      REG8(0x5342)
#define ADC_SR       REG8(0x5343)
#define ADC_DRH      REG8(0x5344)
#define ADC_DRL      REG8(0x5345)
#define ADC_SQR0     REG8(0x534A)
#define ADC_TRIGR1   REG8(0x534E)

#endif

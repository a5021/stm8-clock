# projectmem - STM8_CLOCK

_Last updated: 2026-11-16_

## Project purpose
Bare-metal firmware for the STM8L-DISCOVERY board (MB915B, STM8L152C6T6) that turns it into a desk clock with a thermometer: the 14-segment glass shows the time for 3 s, then the internal chip temperature for 3 s, alternating. The time is set from a PC over SWIM by a small Python script — no…

## Recent issues
- No issues logged yet.

## Decisions
- Refactor: refactor: drop dead code and simplify build and display [src/main.c]

## Notes
- chore: build rules for SDCC and ignore generated output
- feat(time): unix time to calendar conversion
- feat(lcd): 14-segment glass driver for the MB915B board
- feat(rtc): calendar on the 32.768 kHz crystal with LSI fallback
- feat(temp): internal sensor read through Vrefint, calibrated in place
- feat(main): alternate time and temperature, set time from a SWIM mailbox
- feat(tools): set the clock from a PC over SWIM
- docs: describe hardware, flashing, time mailbox and calibration

## Key files
- `.gitignore`
- `Makefile`
- `src/regs.h`
- `src/time_conv.c`
- `src/time_conv.h`
- `src/lcd.c`
- `src/lcd.h`
- `src/rtc.c`
- `src/rtc.h`
- `32.768`
- `src/temp.c`
- `src/temp.h`
- `src/main.c`
- `tools/set_time.py`
- `README.md`

## Open questions
- None logged yet.

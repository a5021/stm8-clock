# STM8L-DISCOVERY clock thermometer

Bare-metal firmware for the STM8L-DISCOVERY board (MB915B, STM8L152C6T6) that
turns it into a desk clock with a thermometer: the 14-segment glass shows the
time for 3 s, then the internal chip temperature for 3 s, alternating. The time
is set from a PC over SWIM by a small Python script — no buttons, no wiring.

```
HH.MM.SS     ->  12.34.56   (dots: only positions 1..4 have COL/DP segments)
 23.5C       ->  temperature, one decimal
```

Written in C for SDCC, no interrupts: the RTC seconds are polled and `sec % 6`
selects which value is on the glass.

## Hardware

| Item | Value |
|------|-------|
| Board | STM8L-DISCOVERY (MB915B) |
| MCU | STM8L152C6T6, 32 KB flash, 2 KB RAM |
| Display | 6 positions, 14-segment glass driven by the on-chip LCD controller |
| Clock | LSE, 32.768 kHz crystal on `PC6`/`PC7`, `LSERDY` confirmed on hardware |
| Temperature | internal sensor of ADC1 against the Vrefint reference |
| Programmer | the board's own ST-Link V1 (`0483:3744`) over its USB port |

Only positions 1..4 of the glass drive COL/DP, so the temperature puts the last
integer digit on position 2 (`25.5C` starts at position 1, `5.0C` at position
2). A negative two digit value has no room for the fraction (`-15C`).

## Layout

| File | Purpose |
|------|---------|
| `src/regs.h` | register addresses used by the firmware |
| `src/lcd.c`, `src/lcd.h` | 14-segment tables and per-position LCD RAM mapping |
| `src/rtc.c`, `src/rtc.h` | LSE (LSI fallback) and RTC calendar |
| `src/temp.c`, `src/temp.h` | ADC1 on Vrefint + internal temperature sensor |
| `src/time_conv.c`, `src/time_conv.h` | Unix time <-> calendar |
| `src/main.c` | mailbox, display alternation |
| `tools/set_time.py` | writes the mailbox and resets the board |

## Requirements

- [SDCC](https://sdcc.sourceforge.net/) with STM8 support, on `PATH`. With the
  scoop package copy `bin/cc1` to `bin/cc1.exe`: SDCC executes `cc1` by name and
  the package ships it without the extension.
- [stm8flash](https://github.com/vdudouyt/stm8flash) for flashing;
  `tools/bin/stm8flash.exe` (not committed) is a Windows build of v1.1 with
  `espstlink.c` replaced by a stub, plus `libusb-1.0.dll`.
- Python 3 for `tools/set_time.py`.

## Build

```bash
make
```

Output: `build/clock.ihx`. DATA is linked at `0x0008` so the SDCC startup never
clears the mailbox at `0x0000..0x0007`.

## Flash

```bash
make flash
# tools/bin/stm8flash -c stlink -p stm8l152c6 -w build/clock.ihx
```

The onboard programmer (`0483:3744`, ST-Link V1) speaks the ST-Link protocol
over its class-8 bulk endpoints, but Windows binds the mass-storage driver
(`USBSTOR`) to it, which `libusb` cannot open. Bind WinUSB once with
[Zadig](https://zadig.akeo.ie/) (`Options > List All Devices > 0483 3744 >
WinUSB > Replace Driver`); after that `stm8flash -c stlink` and `set_time.py`
work through the board's own USB port. An external ST-Link V2 on the SWIM header
also works with `-c stlinkv2`.

## Set the time

```bash
python tools/set_time.py                 # local clock, now
python tools/set_time.py -t "2026-11-15 20:04:00"
python tools/set_time.py -n              # only print the mailbox
```

The script writes 8 bytes to RAM at `0x0000`:

| Offset | Value |
|--------|-------|
| 0, 1 | `0xA5 0x5A` |
| 2..5 | Unix time of the local wall clock, little endian |
| 6 | CRC8, poly `0x07`, init `0x00`, over bytes 0..5 |
| 7 | `0xFF`, reserved |

`stm8flash` resets the board afterwards. On boot the firmware checks the magic
and CRC, loads the calendar, then clears bytes 0 and 1, so a plain reset keeps
the time running.

RAM has no backup: a power cycle restarts the clock at `2027-01-01 00:00:00`
until `set_time.py` runs again. The same happens after every SWIM access
(`stm8flash` resets the chip), so run `set_time.py` last, after flashing or
reading anything.

## Temperature calibration

The internal sensor on this part reads far from the datasheet typicals: at
25.5 C it produced ~676 ADC counts (~492 mV with VDDA = 1225 mV * 4096 / 1681).
`V25_MV` and `V25_TENTHS` in `src/temp.c` hold that single-point calibration;
the slope stays at the datasheet -4.3 mV/C (verified: the reading falls when the
chip is warmed). Trim `TEMP_OFFSET_TENTHS` in `src/temp.h` against a reference
thermometer and reflash. VDDA is recovered from Vrefint, so the reading does not
depend on the supply voltage.

## Notes

- Fast channels of ADC1 are selected by bits of `ADC_SQR1` (`0x10` Vrefint,
  `0x20` temperature sensor) with `SMPT2` at 384 cycles and the ADC prescaler
  at /2; `ADC_SQR2..SQR4` do not change the converted channel.
- SDCC for STM8 stores 16-bit values big-endian, which matters when reading RAM
  contents over SWIM.
- Any SWIM access resets the chip and therefore the RTC, see above.

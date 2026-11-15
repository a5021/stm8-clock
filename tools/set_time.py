#!/usr/bin/env python3
"""Set the STM8 clock-thermometer time over SWIM.

The firmware reads an 8-byte mailbox at RAM address 0x0000 on every boot:

    0   0xA5
    1   0x5A
    2   Unix time, little endian, 4 bytes
    6   CRC8 (poly 0x07, init 0x00) over bytes 0..5
    7   0xFF, reserved

This script builds that mailbox, writes it into RAM with stm8flash and lets
stm8flash reset the board, which applies the time and clears the magic bytes.
"""

import argparse
import calendar
import os
import struct
import subprocess
import sys
import tempfile
import time

MB_SIZE = 8
MB_MAGIC = b"\xA5\x5A"


def crc8(data: bytes) -> int:
    crc = 0
    for byte in data:
        crc ^= byte
        for _ in range(8):
            crc = ((crc << 1) ^ 0x07) & 0xFF if crc & 0x80 else (crc << 1) & 0xFF
    return crc


def mailbox(unix_time: int) -> bytes:
    body = MB_MAGIC + struct.pack("<I", unix_time)
    return body + bytes((crc8(body), 0xFF))


def local_wall_clock() -> int:
    """Unix time for the local wall clock, treated as if it were UTC.

    The firmware has no time zone conversion, so the display must show local
    time: send the local hour/minute/second in the Unix time field.
    """
    lt = time.localtime()
    return calendar.timegm(
        (lt.tm_year, lt.tm_mon, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec, 0, 0, 0)
    )


def main() -> int:
    here = os.path.dirname(os.path.abspath(__file__))
    ap = argparse.ArgumentParser(description="Set the STM8 clock time over SWIM.")
    ap.add_argument("-t", "--time", help='local time "YYYY-MM-DD HH:MM:SS" (default: now)')
    ap.add_argument("-c", "--part", default="stm8l152c6", help="STM8 part (default: stm8l152c6)")
    ap.add_argument("-p", "--programmer", default="stlink", help="programmer (default: stlink)")
    ap.add_argument("-S", "--serial", help="programmer serial number, when several are attached")
    ap.add_argument("--stm8flash", default=os.path.join(here, "bin", "stm8flash.exe"),
                    help="path to stm8flash")
    ap.add_argument("-n", "--dry-run", action="store_true", help="show the mailbox, do not program")
    args = ap.parse_args()

    if args.time:
        stamp = calendar.timegm(time.strptime(args.time, "%Y-%m-%d %H:%M:%S"))
    else:
        stamp = local_wall_clock()

    data = mailbox(stamp)
    print("time  " + time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime(stamp)) + " (local)")
    print("mail  " + " ".join("%02X" % b for b in data))
    if args.dry_run:
        return 0

    if not os.path.isfile(args.stm8flash):
        sys.exit("stm8flash not found: %s" % args.stm8flash)

    handle, path = tempfile.mkstemp(suffix=".bin")
    try:
        with os.fdopen(handle, "wb") as f:
            f.write(data)
        cmd = [args.stm8flash, "-c", args.programmer, "-p", args.part,
               "-s", "ram", "-b", str(MB_SIZE), "-w", path]
        if args.serial:
            cmd[1:1] = ["-S", args.serial]
        print("run   " + " ".join(cmd))
        return subprocess.call(cmd)
    finally:
        os.unlink(path)


if __name__ == "__main__":
    sys.exit(main())

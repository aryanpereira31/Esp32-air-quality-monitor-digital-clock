#!/usr/bin/env python3
"""Capture ESP32 serial CSV output to a file.

This logger keeps only CSV-style lines from the serial stream, which makes it
safe to leave running all day without keeping the monitor window open.
"""

from __future__ import annotations

import argparse
import csv
import sys
import time
from pathlib import Path

try:
    import serial
except ImportError:  # pragma: no cover - runtime dependency check
    print("pyserial is required. Install it with: python -m pip install pyserial", file=sys.stderr)
    raise


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Log ESP32 serial CSV data to a file")
    parser.add_argument("--port", required=True, help="Serial port, e.g. COM11")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate (default: 115200)")
    parser.add_argument("--output", required=True, help="Output CSV file path")
    parser.add_argument(
        "--append",
        action="store_true",
        help="Append to the output file instead of overwriting it",
    )
    return parser.parse_args()


def looks_like_csv_line(text: str) -> bool:
    stripped = text.strip()
    if not stripped:
        return False
    if stripped.startswith("System ready"):
        return False
    return "," in stripped


def main() -> int:
    args = parse_args()
    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    mode = "a" if args.append else "w"
    seen_header = False
    showed_first_sample = False

    with serial.Serial(args.port, args.baud, timeout=1) as ser, output_path.open(mode, newline="", encoding="utf-8") as file_handle:
        writer = csv.writer(file_handle)

        print(f"OK: logging {args.port} at {args.baud} baud to {output_path}")
        print("OK: press Ctrl+C to stop.")

        while True:
            raw_line = ser.readline()
            if not raw_line:
                continue

            try:
                line = raw_line.decode("utf-8", errors="replace").strip()
            except UnicodeDecodeError:
                continue

            if not looks_like_csv_line(line):
                continue

            fields = [field.strip() for field in line.split(",")]

            # Write the header once, then keep every numeric sample.
            if not seen_header and fields and fields[0] == "epoch":
                writer.writerow(fields)
                file_handle.flush()
                seen_header = True
                continue

            # If the device restarts and prints the header again, skip duplicates.
            if fields and fields[0] == "epoch":
                continue

            writer.writerow(fields)
            file_handle.flush()
            seen_header = True

            if not showed_first_sample:
                print(f"OK: first reading: {line}")
                showed_first_sample = True

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except serial.SerialException as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        raise SystemExit(1)
    except KeyboardInterrupt:
        print("\nStopped.")
        raise SystemExit(0)

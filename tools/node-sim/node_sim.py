"""Minimal MonsunLink node simulator (Modbus RTU slave) for testing the controller without hardware.

Usage: python node_sim.py COM5 --address 5
Requires: pip install pyserial
Serves the self-description registers and one demo sensor entity.
"""
import argparse
import struct

import serial

MAGIC = 0x4D53
FIRST_DESC = 0x0010


def crc16(data: bytes) -> int:
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            crc = (crc >> 1) ^ 0xA001 if crc & 1 else crc >> 1
    return crc


def build_registers() -> dict[int, int]:
    regs = {0x0000: MAGIC, 0x0001: 0x0000, 0x0002: 0x0100, 0x0003: 0, 0x0004: 1, 0x0005: 1, 0x0006: 10}
    # entity 0: sensor, class 0, unit 0, value in input register 0x0100, scale -1 (0.1 steps)
    desc = [1, 0, 0, 0x0100, 2, 0xFFFF, 0, 0xFFFF]
    for i, v in enumerate(desc):
        regs[FIRST_DESC + i] = v
    return regs


def handle(frame: bytes, address: int, holding: dict[int, int], inputs: dict[int, int]) -> bytes | None:
    if len(frame) < 8 or frame[0] != address:
        return None
    if crc16(frame[:-2]) != struct.unpack("<H", frame[-2:])[0]:
        return None
    fc = frame[1]
    if fc in (0x03, 0x04):
        start, count = struct.unpack(">HH", frame[2:6])
        table = holding if fc == 0x03 else inputs
        data = b"".join(struct.pack(">H", table.get(start + i, 0)) for i in range(count))
        body = bytes([address, fc, len(data)]) + data
        return body + struct.pack("<H", crc16(body))
    return None


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("port")
    ap.add_argument("--address", type=int, default=5)
    ap.add_argument("--baud", type=int, default=19200)
    args = ap.parse_args()

    holding = build_registers()
    inputs = {0x0100: 215}  # 21.5
    with serial.Serial(args.port, args.baud, timeout=0.05) as ser:
        buf = b""
        while True:
            chunk = ser.read(256)
            if chunk:
                buf += chunk
                continue
            if buf:
                reply = handle(buf, args.address, holding, inputs)
                if reply:
                    ser.write(reply)
                buf = b""


if __name__ == "__main__":
    main()

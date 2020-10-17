#!/usr/bin/env python3
#
# read_pressure.py - connect to Blutooth Low Energy device and read the GATT
# Environmental Sensing characteristic for barometric pressure.
#
# NOTE: Must run with sudo or have permission to access Bluetooth device.
#
import asyncio
from bleak import BleakClient

address = input("Device Address (e.g. 12:34:56:78:9A:BC): ")
PRESSURE_UUID = "00002a6d-0000-1000-8000-00805f9b34fb"
async def run(address):
    async with BleakClient(address) as client:
        pressure = await client.read_gatt_char(PRESSURE_UUID)
        print("Barometric Pressure: {0} kPa".format(int.from_bytes(pressure, byteorder='little', signed=False) / 10000))

loop = asyncio.get_event_loop()
loop.run_until_complete(run(address))

#!/usr/bin/env python3
#
# read_humidity.py - connect to Blutooth Low Energy device and read the GATT
# Environmental Sensing characteristic for humidity.
#
# NOTE: Must run with sudo or have permission to access Bluetooth device.
#
import asyncio
from bleak import BleakClient

address = input("Device Address (e.g. 12:34:56:78:9A:BC): ")
HUMIDITY_UUID = "00002a6f-0000-1000-8000-00805f9b34fb"
async def run(address):
    async with BleakClient(address) as client:
        humidity = await client.read_gatt_char(HUMIDITY_UUID)
        print("Humidity: {0} %".format(int.from_bytes(humidity, byteorder='little', signed=False) / 100))

loop = asyncio.get_event_loop()
loop.run_until_complete(run(address))

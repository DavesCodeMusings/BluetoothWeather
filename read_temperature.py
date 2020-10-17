#!/usr/bin/env python3
#
# read_irradiance.py - connect to Blutooth Low Energy device and read the GATT
# Environmental Sensing characteristic for temperature.
#
# NOTE: Must run with sudo or have permission to access Bluetooth device.
#
import asyncio
from bleak import BleakClient

address = input("Device Address (e.g. 12:34:56:78:9A:BC): ")
TEMPERATURE_UUID = "00002a6e-0000-1000-8000-00805f9b34fb"
async def run(address):
    async with BleakClient(address) as client:
        temperature = await client.read_gatt_char(TEMPERATURE_UUID)
        print("Temperature: {0} Celsius".format(int.from_bytes(temperature, byteorder='little', signed=True) / 100))

loop = asyncio.get_event_loop()
loop.run_until_complete(run(address))

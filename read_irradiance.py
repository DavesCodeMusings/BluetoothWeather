#!/usr/bin/env python3
#
# read_irradiance.py - connect to Blutooth Low Energy device and read the GATT
# Environmental Sensing characteristic for irradiance.
#
# NOTE: Must run with sudo or have permission to access Bluetooth device.
#
import asyncio
from bleak import BleakClient

address = input("Device Address (e.g. 12:34:56:78:9A:BC): ")
IRRADIANCE_UUID = "00002a77-0000-1000-8000-00805f9b34fb"
async def run(address):
    async with BleakClient(address) as client:
        irradiance = await client.read_gatt_char(IRRADIANCE_UUID)
        print("Irradiance: {0} W/m^2".format(int.from_bytes(irradiance, byteorder='little', signed=False) / 10))

loop = asyncio.get_event_loop()
loop.run_until_complete(run(address))

#!/bin/sh
# List services offered by Bluetooth Low Energy device.

# Default BLE address to use or get from command-line.
ADDRESS="12:34:56:78:9A:BC"
if [ "$1" != "" ]; then
  ADDRESS=$1
fi

echo "Primary Services"
gatttool --device=$ADDRESS --primary

echo "Characteristics"
gatttool --device=$ADDRESS --characteristics

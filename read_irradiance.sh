#!/bin/sh
# Read irradiance from a Bluetooth Low Energy device.
# The output will be Watts per square meter as specified by the characteristic.

# Default BLE address to use or get from command-line.
ADDRESS="71:A2:AE:D8:21:30"
if [ "$1" != "" ]; then
  ADDRESS=$1
fi

printf "%d\n" $(gatttool --device=$ADDRESS --char-read --uuid=0x2a77 | awk '{ printf("0x%s%s\n", $5, $4) }') | sed 's/..$/\.&/'

# Explanation of how it works:
#
# gatttool --device=$ADDRESS --char-read --uuid=0x2a77
#   Read the characteristic for temperature. The output will be come with
#   extra info that's not needed.
# awk '{ printf("0x%s%s\n", $5, $4) }')
#   Extract only the two pairs of hex digits from the output. These are in
#   LSB first order. Combine them with a leading 0x. For example, 0x00FE.
# printf "%d\n"
#   Take the 0x00FE from gatttool | awk, and convert it to a decimal number.
# sed 's/..$/\.&/'
#   Take the printf output and insert a decimal point two characters from the
#   right, because the GATT irradiance characteristic has a resolution of two
#   decimal places.

#!/bin/sh
# Read humidity from a Bluetooth Low Energy device.
# The output will be a percentage as specified by the characteristic.

# Default BLE address to use or get from command-line.
ADDRESS="12:34:56:78:9A:BC"
if [ "$1" != "" ]; then
  ADDRESS=$1
fi

printf "%d\n" $(gatttool --device=$ADDRESS --char-read --uuid=0x2a6f | awk '{ printf("0x%s%s\n", $5, $4) }') | sed 's/..$/\.&/'

# Explanation of how it works:
#
# gatttool --device=$ADDRESS --char-read --uuid=0x2a6f
#   Read the characteristic for humidity. The output will be come with
#   extra info that's not needed.
# awk '{ printf("0x%s%s\n", $5, $4) }')
#   Extract only the two pairs of hex digits from the output. These are in
#   LSB first order. Combine them with a leading 0x. For example, 0x0DC1.
# printf "%d\n"
#   Take the 0x0DC1 from gatttool | awk, and convert it to a decimal number.
# sed 's/..$/\.&/'
#   Take the printf output and insert a decimal point two characters from the
#   right, because the GATT humidity characteristic has a resolution of two
#   decimal places.

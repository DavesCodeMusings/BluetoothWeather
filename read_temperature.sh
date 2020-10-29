#!/bin/sh

# Default BLE address to use or get from command-line.
ADDRESS="12:34:56:78:9A:BC"
if [ "$1" != "" ]; then
  ADDRESS=$1
fi

printf "%d\n" $(gatttool --device=$ADDRESS --char-read --uuid=0x2a6e | awk '{ printf("0x%s%s\n", $5, $4) }') | sed 's/..$/\.&/'

# Explanation of how it works:
#
# gatttool --device=$ADDRESS --char-read --uuid=0x2a6e
#   Read the characteristic for temperature. The output will be come with
#   extra info that's not needed.
# awk '{ printf("0x%s%s\n", $5, $4) }')
#   Extract only the two pairs of hex digits from the output. These are in
#   LSB first order. Combine them with a leading 0x. For example, 0x01FE.
# printf "%d\n"
#   Take the 0x01FE from gatttool | awk, and convert it to a decimal number.
# sed 's/..$/\.&/'
#   Take the printf output and insert a decimal point two characters from the
#   right, because the GATT temperature characteristic has a resolution of two
#   decimal places.
# The output will be degrees Celsius as specified by the characteristic.

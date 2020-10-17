# Nano33BLESense
A collection of Arduino sketches targeting the Nano 33 BLE Sense device. 

## Arduino Sketches
Run any one of these on an Arduino Nano 33 BLE Sense to collect data from the on-board sensors and make it available via Bluetooth Low Energy.

### BluetoothBattery.ino
A very simple sketch outlining the basics of Bluetooth communication. One step up from Hello World.

### BluetoothWeather.ino
Temp, humidity, and barometric pressure over Bluetooth with the Arduino Nano 33 Sense.

### BluetoothWeather2.ino
Just like BluetoothWeather.ino, except it adds a value for elevation to show how writable characteristics work.

### BluetoothWeather3.ino
Replaces elevation characteristic with irradiance to measure cloud cover.

### irradiance.ino
Measuring ambient light and putting it in proper units for reading via Bluetooth.

## Python client programs
A number of Python programs are included for reading the various sensor values advertised by the Arduino sketches. All are based on the example code provided by the Bleak library home page. https://github.com/hbldh/bleak

The Python code has been tested on Raspberry Pi 3B+ running Raspbian OS Lite with the pip3 package installed and Bleak library installed with pip3.

Install with: sudo apt install python3-pip && sudo pip3 install bleak

Run discover.py to find the hardware address of your Arduino. Use that address as input to the remaining Python programs.

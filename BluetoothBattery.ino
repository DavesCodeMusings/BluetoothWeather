/*
 * Bluetooth Battery
 * A very simple example of using Bluetooth Low Energy (BLE) to give the
 * status of a fictitious Nano 33 Sense battery. This is only an example.
 * It does not actually read a battery, because there isn't one. It simply 
 * reports 100% whenever it is polled.
 *
 * Copyright (c) 2020 David Horton
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <ArduinoBLE.h>

// Declare Bluetooth service name, and characteristic for battery status.
// All are standard GATT names that can be found here:
// https://specificationrefs.bluetooth.com/assigned-values/16-bit%20UUID%20Numbers%20Document.pdf
// https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.battery_level.xml
BLEService batteryService("180F");
BLEUnsignedCharCharacteristic batteryLevelCharacteristic("2A19", BLERead); // 8-bit unsigned percent value 0 - 100, no decimal places.

// Configure Bluetooth and serial debugging.
void setup() {

  // Illuminate LED for 5sec to indicate startup. 
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(5000);

  // Initialize serial for debug output.
  Serial.begin(9600);

  // Initialize Bluetooth communication.
  Serial.println("Initializing Bluetooth communication.");
  if (!BLE.begin()) {
    Serial.println("Failed.");
    while (1); // Loop forever. This has the effect of leaving the LED on, indicating a failure.
  }

  // Set up Bluetooth service.
  Serial.println("Setting up battery service with characteristic for battery level.");
  BLE.setLocalName("Nano33BLE");
  BLE.setAdvertisedService(batteryService);

  // Add battery level characteristic.
  batteryService.addCharacteristic(batteryLevelCharacteristic);
  batteryLevelCharacteristic.writeValue(100); // Make it 100% since it's wall powered.
  
  // Make the service available.
  Serial.println("Advertising services.");
  BLE.addService(batteryService);
  BLE.setConnectable(true);
  BLE.advertise();

  // Turn off LED to indicate startup has completed successfully.
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {

  // Wait for a connection from a central.
  BLEDevice central = BLE.central();

  // When a connection is made, activate LED and write address to serial for debug.
  if (central) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.print("Incoming connection from: ");
    Serial.println(central.address());

    while (central.connected()) {

      // If there were an actual battery, read its capacity and report with:
      // batteryLevelCharacteristic.writeValue(<batteyPercent>);
      // where <batteryPercent> is an unsigned char value between 0 and 100.

      // Delay between updates. (Don't make too long of connections start to timeout.)
      delay(1000);
    }

    // Turn off LED when connection is dropped. 
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Connection terminated.");
  }
}

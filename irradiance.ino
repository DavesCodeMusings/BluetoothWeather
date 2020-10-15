/*
 * Bluetooth Light Meter
 * A simple example of reading ambient light from the APDS-9960 in the 
 * Arduino Nano 33 Sense and transmitting it via Blutooth Low Energy.
 * Use as a learning tool or the basis of a home automation sensor. This
 * sketch uses the same ideas as Bluetooth Weather, but limits to a single
 * characteristic for irradiance.
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
#include <Arduino_APDS9960.h>
 
// Declare the variable for irradiance reading.
float irradiance;

// Declare Bluetooth service names, and irradiance characteristics.
BLEService environmentalSensingService("181A");
// https://specificationrefs.bluetooth.com/assigned-values/16-bit%20UUID%20Numbers%20Document.pdf
BLEUnsignedIntCharacteristic irradianceCharacteristic("2A77", BLERead); // 16-bit unsigned, 1 decimal place.
// https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.irradiance.xml

void setup() {
  // Illuminate LED for 5sec to indicate startup. 
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(5000);

  // Initialize serial for debug output.
  Serial.begin(9600);

  // Initialize gesture sensor. (This sensor also reads ambient light.)
  Serial.println("Initializing gesture sensor.");
  if (!APDS.begin()) {
    Serial.println("Failed.");
    while (1);
  }

  // Initialize Bluetooth communication.
  Serial.println("Initializing Bluetooth communication.");
  if (!BLE.begin()) {
    Serial.println("Failed.");
    while (1);
  }

  // Set up Bluetooth Environmental Sensing service.
  Serial.println("Setting up Environmental Sensing service with a characteristic for irradiance.");
  BLE.setLocalName("Nano33BLE");
  BLE.setAdvertisedService(environmentalSensingService);

  // Add characteristic for irradiance.
  environmentalSensingService.addCharacteristic(irradianceCharacteristic);

  // Make the service available.
  BLE.addService(environmentalSensingService);
  BLE.setConnectable(true);
  Serial.println("Advertising services.");
  BLE.advertise();

  // Turn off LED to indicate startup is complete.
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
      
      // Get reading from sensor and update the charcteristic value.
      if (APDS.colorAvailable()) {
        int red, green, blue, ambient;
        APDS.readColor(red, green, blue, ambient);
        irradiance = (float) ambient / 2360; // irradiance is converted to Watts per square meter. See datasheet for typical irradiance responsivity. https://docs.broadcom.com/doc/AV02-4191EN

        // Write value to serial port for debug.
        Serial.print("Red: ");
        Serial.println(red);
        Serial.print("Green: ");
        Serial.println(green);
        Serial.print("Blue: ");
        Serial.println(blue);
        Serial.print("Ambient: ");
        Serial.println(ambient);
      }
      Serial.print("Irradiance: ");
      Serial.print(irradiance);
      Serial.println("W/sq. meter");
 
      // Update Bluetooth characteristic with new value.
      irradianceCharacteristic.writeValue((uint16_t) round(irradiance * 10)); // Shift for one decimal place.

      // Delay between updates. (Don't make too long or connections start to timeout.)
      delay(1000);
    }

    // Turn off LED when connection is dropped. 
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Connection terminated.");
  }
}

/*
 * Bluetooth Weather 2
 * A simple example of using some of the built-in sensors and LEDs in the 
 * Arduino Nano 33 Sense and transmitting values via Blutooth Low Energy.
 * Use as a learning tool or the basis of a home automation sensor. This
 * sketch builds on the first Bluetooth Weather by adding a characteristic 
 * for elevation. The purpose is to show how parameters can be written.
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
#include <Arduino_HTS221.h>
#include <Arduino_LPS22HB.h>
 
// Declare the variables for elevation, barometric pressure, temperature, and humidity, respectively.
float e;
float p;
float t;
float h;

// Declare Bluetooth service names, and characteristics. All are standard GATT services.
BLEService environmentalSensingService("181A");
// https://specificationrefs.bluetooth.com/assigned-values/16-bit%20UUID%20Numbers%20Document.pdf
BLEUnsignedIntCharacteristic elevationCharacteristic("2A6C", BLERead | BLEWrite); // 32-bit signed in meters, 2 decimal places, with zero representing sea-level.
// https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.elevation.xml
BLEUnsignedIntCharacteristic pressureCharacteristic("2A6D", BLERead); // 32-bit unsigned in Pascals, 1 decimal place.
// https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.pressure.xml
BLEShortCharacteristic temperatureCharacteristic("2A6E", BLERead); // 16-bit signed, 2 decimal places.
// https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.temperature.xml
BLEUnsignedIntCharacteristic humidityCharacteristic("2A6F", BLERead); // 16-bit unsigned, 2 decimal places.
// https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.humidity.xml

// Error pulse of death. Loops forever. Only used if there are problems.
void error_pulse() {
  while (1) {
    for (int i=0; i<3; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(166);
      digitalWrite(LED_BUILTIN, LOW);
      delay(167);
    }
    delay(1001);
  }  
}

// Callback function used when a new value is written to the elevation characteristic.
// In this case, simply update the global variable used for elevation.
void elevationCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  Serial.print("Elevation updated by central: ");
  Serial.println(central.address());
  e = (float) elevationCharacteristic.value();
}

void setup() {
  // Illuminate LED for 5sec to indicate startup. 
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(5000);

  // Initialize serial for debug output.
  Serial.begin(9600);

  // Initialize atmospheric sensors.
  Serial.println("Initializing humidity and temperature sensor.");
  if (!HTS.begin()) {
    Serial.println("Failed.");
    error_pulse();
  }
  Serial.println("Initializing barometric pressure sensor.");
  if (!BARO.begin()) {
    Serial.println("Failed.");
    error_pulse();
  }

  // Initialize Bluetooth communication.
  Serial.println("Initializing Bluetooth communication.");
  if (!BLE.begin()) {
    Serial.println("Failed.");
    error_pulse();
  }

  // Set up Bluetooth Environmental Sensing service.
  Serial.println("Setting up service with characteristics for pressure, temperature, and humidity.");
  BLE.setLocalName("Nano33BLE");
  BLE.setAdvertisedService(environmentalSensingService);

  // Add characteristics for elevation, barometric pressure, temperature, and humidity.
  environmentalSensingService.addCharacteristic(elevationCharacteristic);
  elevationCharacteristic.writeValue(0); // Start with sea level, but it's writable and can be updated.
  environmentalSensingService.addCharacteristic(pressureCharacteristic);
  environmentalSensingService.addCharacteristic(temperatureCharacteristic);
  environmentalSensingService.addCharacteristic(humidityCharacteristic);

  // Assign event handler for writable elevation characteristic.
  elevationCharacteristic.setEventHandler(BLEWritten, elevationCharacteristicWritten);
  
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
      // Get readings from sensors and update the charcteristic values.
      p = BARO.readPressure(KILOPASCAL);
      t = HTS.readTemperature(CELSIUS) - 4; // Subtract 4 deg C to compensate for heat generated by Nano 33 board.
      h = HTS.readHumidity();

      // Write values to serial port for debug.
      Serial.print("Elevation: ");
      Serial.print(e/100); // Divide by 100 to account for the 2 decimal places.
      Serial.println("m above sea level.");
      Serial.print("Pressure: ");
      Serial.print(p);
      Serial.println("kPa");
      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.println("°C");
      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.println("%");

      // Update Bluetooth characteristics with new values.
      pressureCharacteristic.writeValue((uint32_t) round(p * 10000)); // Convert kPa to Pa and shift for one decimal place.
      temperatureCharacteristic.writeValue((int16_t) round(t * 100)); // Shift for two decimal places.
      humidityCharacteristic.writeValue((uint16_t) round(h * 100)); // Shift for two decimal places.

      // Delay between updates. (Don't make too long of connections start to timeout.)
      delay(1000);
    }

    // Turn off LED when connection is dropped. 
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Connection terminated.");
  }
}

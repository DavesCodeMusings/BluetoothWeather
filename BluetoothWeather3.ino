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
#include <Arduino_APDS9960.h>
 
// Declare the variables for barometric pressure, temperature, humidity, and irradiance
// respectively. connect_timeout is used forcibly drop clients after so many iterations
// of the while (central.connected()) loop.
float pressure;
float temperature;
float humidity;
float irradiance;
int connect_timeout;

// Declare Bluetooth service names, and characteristics. All are standard GATT services.
BLEService environmentalSensingService("181A");
// https://specificationrefs.bluetooth.com/assigned-values/16-bit%20UUID%20Numbers%20Document.pdf
BLEUnsignedIntCharacteristic pressureCharacteristic("2A6D", BLERead); // 32-bit unsigned in Pascals, 1 decimal place.
// https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.pressure.xml
BLEShortCharacteristic temperatureCharacteristic("2A6E", BLERead); // 16-bit signed, 2 decimal places.
// https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.temperature.xml
BLEUnsignedIntCharacteristic humidityCharacteristic("2A6F", BLERead); // 16-bit unsigned, 2 decimal places.
// https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.humidity.xml
BLEUnsignedIntCharacteristic irradianceCharacteristic("2A77", BLERead); // 16-bit unsigned, 1 decimal place.
// https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.irradiance.xml

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
  Serial.println("Initializing gesture sensor.");
  if (!APDS.begin()) {
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
  Serial.println("Setting up Environmental Sensing service with characteristics for pressure, temperature, and humidity.");
  BLE.setLocalName("Nano33BLE");
  BLE.setAdvertisedService(environmentalSensingService);

  // Add characteristics for barometric pressure, temperature, and humidity.
  environmentalSensingService.addCharacteristic(pressureCharacteristic);
  environmentalSensingService.addCharacteristic(temperatureCharacteristic);
  environmentalSensingService.addCharacteristic(humidityCharacteristic);
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
    connect_timeout = 60; // Equates to seconds when while loop delay is 1000mS.

    while (central.connected()) {
      // Get readings from sensors and update the charcteristic values.
      pressure = BARO.readPressure(KILOPASCAL);
      temperature = HTS.readTemperature(CELSIUS) - 4; // Subtract 4 deg C to compensate for heat generated by Nano 33 board.
      humidity = HTS.readHumidity();
      if (APDS.colorAvailable()) {
        int red, green, blue, ambient;
        APDS.readColor(red, green, blue, ambient);
        irradiance = ambient / 2360; // irradiance is converted to Watts per square meter. See datasheet for typical irradiance responsivity. https://docs.broadcom.com/doc/AV02-4191EN
      }

      // Write values to serial port for debug.
      Serial.print("Pressure: ");
      Serial.print(pressure);
      Serial.println("kPa");
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println("°C");
      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println("%");
      Serial.print("Irradiance: ");
      Serial.print(irradiance);
      Serial.println("W/sq. meter");
      Serial.print("Connect timeout: ");
      Serial.println(connect_timeout);
 
      // Update Bluetooth characteristics with new values.
      pressureCharacteristic.writeValue((uint32_t) round(pressure * 10000)); // Convert kPa to Pa and shift for one decimal place.
      temperatureCharacteristic.writeValue((int16_t) round(temperature * 100)); // Shift for two decimal places.
      humidityCharacteristic.writeValue((uint16_t) round(humidity * 100)); // Shift for two decimal places.
      irradianceCharacteristic.writeValue((uint16_t) round(irradiance * 10)); // Shift for one decimal place.

      // Delay between updates. (Don't make too long of connections start to timeout.)
      delay(1000);

      // Forcibly disconnect any client that has been hanging on too long.
      connect_timeout--;
      if (connect_timeout <= 0) {
        central.disconnect();
      }
    }

    // Turn off LED when connection is dropped. 
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Connection terminated.");
  }
}

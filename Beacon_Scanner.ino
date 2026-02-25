/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
*/

#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEEddystoneURL.h>
#include <BLEEddystoneTLM.h>
#include <BLEBeacon.h>

#define txPower 19

#define sPin 21

#define isMain 1

int scanTime = 1;  //In seconds
BLEScan *pBLEScan;
unsigned long scanStart;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveName() && advertisedDevice.getName() == "Bluedroid_Beacon" ) {
      int rssi = advertisedDevice.getRSSI();
      Serial.print(rssi);
      Serial.print(" ");
      Serial.print(pow(10, ((double) (txPower - rssi - (25-19))) / (10 * 2.3)));
      Serial.print(" ");
      Serial.println(millis() - scanStart);

      digitalWrite(LED_BUILTIN, !(rssi < -40));
      #if isMain == 1
        delay(20);
        if (digitalRead(sPin)) {
          Serial.println("Срабатывание!");
        }
        delay(60);
      #elif isMain == 0
        digitalWrite(sPin, 1);
        delay(100);
        digitalWrite(sPin, 0);
      #endif
    }
  }
};

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  pinMode(LED_BUILTIN, OUTPUT);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); 

  Serial.println("RSSI,dist,ping");
}

void loop() {
  scanStart = millis();
  BLEScanResults *foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan->clearResults();
  delay(10);
}

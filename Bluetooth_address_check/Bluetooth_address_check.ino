#include <M5Atom.h>
#include <BLEDevice.h>

#define SCANTIME 1
const char* uuid = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"; // Tomoru Service UUID

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        int rssi;
        if(advertisedDevice.haveServiceUUID()){
            // Find Tomoru's service UUID.If there is more than one, there will be more than one.
            if(strncmp(advertisedDevice.getServiceUUID().toString().c_str(),uuid, 36) == 0){
                Serial.println(advertisedDevice.getAddress().toString().c_str())
            }
        }
    }
};

// function :: If it is different from the previous state, send data via Sigfox.
//             (Data will be sent at regular intervals while you are out).
void Task1(void *pvParameters){
    // Prepare for BLEscan.
    BLEDevice::init("");
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    while(1){
        Serial.println("check beacon.");
        // Perform BLE search.
        pBLEScan->setActiveScan(true);
        pBLEScan->start(SCANTIME, false);
        pBLEScan->clearResults(); // Reset the BLE results.
        // Take a break from exploring the BLE.
        delay(CHECK_DELAY * 1000);
    }
}

void setup() {
    // for Atom matrix and BLE device.
    M5.begin(true, false, true);
    BLEDevice::init("");
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
}

void loop(){
    pBLEScan->setActiveScan(true);
    pBLEScan->start(SCANTIME, false);
    pBLEScan->clearResults(); // Reset the BLE results.
    M5.update();
}
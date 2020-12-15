#include <M5Atom.h>
#include <BLEDevice.h>

#define SCANTIME 1

BLEScan* pBLEScan;
const char* uuid = "b3b36901-50d3-4044-808d-50835b13a6cd"; // Tomoru Service UUID

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        int rssi;
        if(advertisedDevice.haveServiceUUID()){
            // Find Tomoru's service UUID.If there is more than one, there will be more than one.
            if(strncmp(advertisedDevice.getServiceUUID().toString().c_str(),uuid, 36) == 0){
                Serial.println(advertisedDevice.getAddress().toString().c_str());
            }
        }
    }
};

void setup() {
    // for Atom matrix and BLE device.
    M5.begin(true, false, true);
}

void loop(){
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(SCANTIME, false);
    pBLEScan->clearResults(); // Reset the BLE results.
    M5.update();
}

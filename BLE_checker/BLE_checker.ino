#include <M5Atom.h>
#include <BLEDevice.h>

#define SCANTIME 1
const char* uuid = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"; // Tomoru Service UUID
// Tomoru device address.(Multiple pieces in a row. Where to register.)
const char* device_address[] = {"a4:cf:12:6d:b3:72", 
                                "aa",
                                "a4:cf:12:6d:b3:72",
                                "aa"};
#define DEVICE_COUNT sizeof(device_address) // Number of Tomoru devices
// A list to register rssi values when looking for Beacon.(Interrupt at a certain time)
int rssi_list[DEVICE_COUNT] = {0};

#define CHECK_DELAY 1  // At what time intervals do we check for Beacons? (Sec)
int ble_state;  // A place to register your location.
// Array of locations to register (for debugging purposes, not actually needed)
const char* Registration_location[] = {"A room", "B room", "C room", "D room", "OUTSIDE"};

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        int rssi;
        if(advertisedDevice.haveServiceUUID()){
            // Find Tomoru's service UUID.If there is more than one, there will be more than one.
            if(strncmp(advertisedDevice.getServiceUUID().toString().c_str(),uuid, 36) == 0){
                // Check if the device ID is the one you registered.
                Serial.println(advertisedDevice.getServiceUUID().toString().c_str());
                for(int i = 0; i < sizeof(device_address) / 4; i++){
                    if(strncmp(advertisedDevice.getAddress().toString().c_str(),
                                device_address[i], 17) == 0){
                        // In the case of a registered device ID, assign the RSSI value.
                        rssi = advertisedDevice.getRSSI();
                        Serial.print("Registered devices [" + String(i) + "] :"); 
                        Serial.println(rssi);
                        rssi_list[i] = rssi;
                    }else{
                        // If there is no registered device, assign 0 as the RSSI value.
                        Serial.print("Registered devices [" + String(i) + "] :"); 
                        Serial.println("None!");
                        rssi_list[i] = 0;
                    }
                    // TODO : Determine the position and create the timing for transmission.
                    /*if(rssi > -40){
                        Serial.println("Found!");
                    }*/
                }
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

    // Configure the other core to look for BLEBeacon.
    xTaskCreatePinnedToCore(Task1,"Task1", 4096, NULL, 3, NULL, 1);
}

void loop(){
    Serial.println("aaa");
    M5.update();
    delay(5000);
}

#include <M5Atom.h>
#include <BLEDevice.h>

const char* uuid = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"; // Tomoru Service UUID
// Tomoru device address.(Multiple pieces in a row. Where to register.)
const char* device_address[] = {"a4:cf:12:6d:b3:72", 
                                "aa",
                                "a4:cf:12:6d:b3:72",
                                "aa"};
#define DEVICE_COUNT sizeof(device_address) // Number of Tomoru devices
// A list to register rssi values when looking for Beacon.(Interrupt at a certain time)
int rssi_list[DEVICE_COUNT] = {0};

#define SCANTIME 1  // Sets the scan time for the BLE.
// Since scantime is also included in the delay, the program should be written with that in mind! Delay 0 is fine, though :)
#define CHECK_DELAY 1  // At what time intervals do we check for Beacons? (Sec)
int location;  // A place to register your location.
// Array of locations to register (for debugging purposes, not actually needed)
const char* Registration_location[] = {"A room", "B room", "C room", "D room", "OUTSIDE"};

// It looks up the highest value from the RSSI list and returns its index.
int Take_maxrssi_idx(){
    int max, max_idx;
    max = rssi_list[0]; max_idx = 0;
    for(int i = 0; i < DEVICE_COUNT; i++){
        if(max < rssi_list[i]){
            max = rssi_list[i]; max_idx = i;
        }
    }
    return max_idx;
}

// Check to see if they are outside.
bool isOutside(){
    for(int i = 0; i < DEVICE_COUNT; i++){
        if(rssi_list != 0)  return false
    }
    return true
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        int rssi;

        Serial.println("Check Tomoru to detect your location.");

        if(advertisedDevice.haveServiceUUID()){
            // Find Tomoru's service UUID.If there is more than one, there will be more than one.
            if(strncmp(advertisedDevice.getServiceUUID().toString().c_str(),uuid, 36) == 0){
                // Check if the device ID is the one you registered.
                for(int i = 0; i < sizeof(device_address) / 4; i++){
                    Serial.print("Registered devices [" + String(i) + "] :");
                    if(strncmp(advertisedDevice.getAddress().toString().c_str(),
                                device_address[i], 17) == 0){
                        // In the case of a registered device ID, assign the RSSI value.
                        rssi = advertisedDevice.getRSSI();
                        Serial.println(rssi);
                        rssi_list[i] = rssi;
                    }else{
                        // If there is no registered device, assign 0 as the RSSI value.
                        Serial.println("None!");
                        rssi_list[i] = 0;
                    }
                    // check outside?
                    if(isOutside()){
                        // Since I'm outside, I send the identification code via Sigfox.
                        // TODO : ↑ Feature Implementation.
                        continue
                    }
                    if(location != Take_maxrssi_idx){
                        Serial.println("We've detected that you're in a different location than last time!");
                        Serial.println("We'll send that information to you via Sigfox.")
                        // Since you are in a different location,
                        // use Sigfox to send the code corresponding to that location.
                        // TODO : ↑ Feature Implementation.
                    }
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
        delay(CHECK_DELAY * 1000);  // Take a break from exploring the BLE.
    }
}

void setup() {
    M5.begin(true, false, true);    // // for Atom matrix and BLE device.

    // Configure the other core to look for BLEBeacon.
    xTaskCreatePinnedToCore(Task1,"Task1", 4096, NULL, 3, NULL, 1);
}

void loop(){
    Serial.println("aaa");
    M5.update();
    delay(5000);
}

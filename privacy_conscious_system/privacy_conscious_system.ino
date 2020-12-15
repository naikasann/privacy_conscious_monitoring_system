#include <M5Atom.h>
#include <BLEDevice.h>

const char* uuid = "b3b36901-50d3-4044-808d-50835b13a6cd"; // Tomoru Service UUID
// Tomoru device address.(Multiple pieces in a row. Where to register.)
const char* device_address[] = {"df:aa:a1:81:6b:6f",
                                "de:67:e4:2d:8d:55",
                                "dc:8c:52:2c:b6:9a",
                                "db:9a:8e:b7:3a:fc"};
#define DEVICE_COUNT (sizeof(device_address) / 4)  // Number of Tomoru devices
// Set the initial value of RSSI (should not be 0, bugs will occur.)
#define RSSI_DEFAULT_VALUE -200
// A list to register rssi values when looking for Beacon.(Interrupt at a certain time)
int rssi_list[DEVICE_COUNT];
// initialization rssi list.
for (int i = 0; i < DEVICE_COUNT; i++){
    rssi_list[i] = RSSI_DEFAULT_VALUE;
}

#define SCANTIME 1  // Sets the scan time for the BLE (Sec).
// Since scantime is also included in the delay, the program should be written with that in mind! Delay 0 is fine, though :)
int location = 0;  // A place to register your location.
// Array of locations to register (for debugging purposes, not actually needed)
const char* Registration_location[] = {"67-314", "67-324", "67-211", "67-Entrance", "OUTSIDE"};

// 6-axis inertial sensor information.
bool IMU6886Flag = false;
float accX = 0, accY = 0, accZ = 0;
float gyroX = 0, gyroY = 0, gyroZ = 0;
double sync_acceleration = 0.0;

// for sigfox breakout board.
HardwareSerial Sigfox(2);

// function : Send a four-digit number for Sigfox.
void sendExe(int send_data){
    String sendcommand = "AT$SF=";
    char data[10];

    sprintf(data, "%4d", send_data);
    sendcommand += String(data) + "\r";

    Serial.println(sendcommand);
    if(!sendCmd(sendcommand, "", 2000))  Serial.println("Request Failed");
}
// Run Sigfox commands with UART
bool sendCmd(String cmd, String waitStr, int waitTime){
  unsigned long tim;    String str;

  Sigfox.print(cmd);    Sigfox.flush();

  tim = millis() + waitTime;
  while (millis() < tim) {
      if (Sigfox.available() > 0) {
          char ch = Sigfox.read();
          Serial.print(ch);
          str += String(ch);
          if (str.indexOf("\r> ") >= 0) break;
      }
  }
  if (waitStr == NULL) return true;
  if (str.indexOf(waitStr) >= 0) return true;
  return false;
}

// function : It looks up the highest value from the RSSI list and returns its index.
int Take_maxrssi_idx(){
    int maxx = RSSI_DEFAULT_VALUE;   int max_idx = 0;

    for(int i = 0; i < DEVICE_COUNT; i++){
        if(maxx < rssi_list[i]){
            maxx = rssi_list[i];    max_idx = i;
        }
    }
    return max_idx;
}

// function : Check to see if they are outside.
bool isOutside(){
    for(int i = 0; i < DEVICE_COUNT; i++){
        if(rssi_list[i] != RSSI_DEFAULT_VALUE)  return false;
    }
    return true;
}

// function (worry one) : Reset the list of Rssi. (I'm wondering if I'm in the right place.)
void reset_rssi(){
    for(int i = 0; i < DEVICE_COUNT; i++){
        rssi_list[i] = RSSI_DEFAULT_VALUE;
    }
}

// Obtaining the current location from Beacon
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        int rssi;

        if(advertisedDevice.haveServiceUUID()){
            // Find Tomoru's service UUID.If there is more than one, there will be more than one.
            if(strncmp(advertisedDevice.getServiceUUID().toString().c_str(),uuid, 36) == 0){
                Serial.println(advertisedDevice.getServiceUUID().toString().c_str());
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
                        rssi_list[i] = RSSI_DEFAULT_VALUE;
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
        pBLEScan->setActiveScan(true);  // Perform BLE search.
        pBLEScan->start(SCANTIME, false);
        pBLEScan->clearResults(); // Reset the BLE results.
        // ↑ need this??
        // check outside?
        Serial.println("Check your location...");
        if(isOutside()){
            // Since I'm outside, I send the identification code via Sigfox.
            // TODO : ↑ Feature Implementation.
            // TODO : Set the data for when you go outside!
            sendExe(1234);
        }else{
            // If it's different from the previous location, it's moved, and Sigfox is used to communicate with that location.
            if(location != Take_maxrssi_idx()){
                Serial.println("We've detected that you're in a different location than last time!");
                Serial.println("We'll send that information to you via Sigfox.");
                Serial.println("before location : " + String(Registration_location[location]));
                Serial.println("after  location : " + String(Registration_location[Take_maxrssi_idx()]));
                location = Take_maxrssi_idx();
                // TODO : Set the data for when you go outside
                sendExe(2345);
            }
        }
        // Reset the RSSI list for when you are completely outside.
        reset_rssi();
    }
}

void setup() {
    M5.begin(true, false, true);    // // for Atom matrix and BLE device.

    // set the data rate for the Hardwareserial port
    // sigfox board setup. 32 => tx, 26 => rx
    Sigfox.begin(9600, SERIAL_8N1, 19, 22);
    Serial.println("Connect to the Sigfox Breakout board...");
    Serial.print("Device ID : ");
    sendCmd("AT$I=10\r", "", 10000);

    // for IMU6886 setup
    if (M5.IMU.Init() != 0) IMU6886Flag = false;
    else                    IMU6886Flag = true;

    // Configure the other core to look for BLEBeacon.
    xTaskCreatePinnedToCore(Task1,"Task1", 4096, NULL, 3, NULL, 1);
}

void loop(){
    if (IMU6886Flag == true){
        // Get gyro and acceleration
        M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
        M5.IMU.getAccelData(&accX, &accY, &accZ);

        // Calculate the synthetic acceleration.
        sync_acceleration = sqrt(pow(accX * 1000, 2) + pow(accY * 1000, 2) + pow(accZ * 1000, 2));

        // Serial.printf("%.2f,%.2f,%.2f", gyroX, gyroY, gyroZ);
        // Serial.printf(",%.2f,%.2f,%.2f\r\n", accX * 1000, accY * 1000, accZ * 1000);

        // Create an if statement for synthetic acceleration and gyro to detect falls.

        //sendExe(9999);
    }
    // for M5 update button and more...
    M5.update();
}

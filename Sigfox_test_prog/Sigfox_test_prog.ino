#include "M5Atom.h"

// for sigfox breakout board.
HardwareSerial Sigfox(2);

void setup() {
  // for m5atom setup.
  M5.begin(true, false, true);
  Serial.begin(115200);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  // set the data rate for the Hardwareserial port
  // sigfox board setup. 32 => tx, 26 => rx
  Sigfox.begin(9600, SERIAL_8N1, 19, 22);
  Serial.println("Connect to the Sigfox Breakout board...");
  Serial.print("Device ID : ");
  sendCmd("AT$I=10\r", "", 10000);
  sendExe(1234);
}

void loop() {
  /*if (Serial2.available()) {
    Serial.write(Serial2.read());
  }*/
  if (Serial.available()) {
    Serial2.write(Serial.read());
  }
}

void sendExe(int send_data){
    String sendcommand = "AT$SF=";
    char data[10];

    sprintf(data, "%4d", send_data);
    sendcommand += String(data) + "\r";
    
    Serial.println(sendcommand);
    if(!sendCmd(sendcommand, "", 2000))  Serial.println("Request Failed");
    delay(600000);
}

bool sendCmd(String cmd, String waitStr, int waitTime){
  unsigned long tim;
  String str;
  
  Sigfox.print(cmd);
  Sigfox.flush();

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

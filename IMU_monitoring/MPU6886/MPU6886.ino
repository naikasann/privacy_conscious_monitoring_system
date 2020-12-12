#include "M5Atom.h"
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

float accX = 0, accY = 0, accZ = 0;
float gyroX = 0, gyroY = 0, gyroZ = 0;
float temp = 0;
bool IMU6886Flag = false;

void setup()
{
    M5.begin(true, false, true);
    SerialBT.begin("m5Atom_mat");

    if (M5.IMU.Init() != 0)
        IMU6886Flag = false;
    else
        IMU6886Flag = true;
}

void loop()
{

    if (IMU6886Flag == true)
    {
        M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
        M5.IMU.getAccelData(&accX, &accY, &accZ);

        SerialBT.printf("%.2f,%.2f,%.2f", gyroX, gyroY, gyroZ);
        SerialBT.printf(",%.2f,%.2f,%.2f\r\n", accX * 1000, accY * 1000, accZ * 1000);
    }
    delay(100);
    M5.update();
}

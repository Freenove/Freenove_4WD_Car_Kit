/**********************************************************************
  Filename    : Bluetooth_Remote_Car.ino
  Product     : Freenove 4WD Car for UNO
  Description : A Bluetooth Remote Car.
  Auther      : www.freenove.com
  Modification: 2019/08/06
**********************************************************************/
#include "Freenove_4WD_Car_for_Arduino.h"
#include "BluetoothOrders.h"

#define UPLOAD_VOL_TIME    3000
#define COMMANDS_COUNT_MAX  8
#define INTERVAL_CHAR '#'

u32 lastUploadVoltageTime;
String inputStringBLE = "";
bool stringComplete = false;

void setup() {
  pinsSetup();
  Serial.begin(9600);

}

void loop() {
  if (millis() - lastUploadVoltageTime > UPLOAD_VOL_TIME) {
    upLoadVoltageToApp();
    lastUploadVoltageTime = millis();
  }
  if (stringComplete) {
    String inputCommandArray[COMMANDS_COUNT_MAX];
    int paramters[COMMANDS_COUNT_MAX], paramterCount = 0;
    String inputStringTemp = inputStringBLE;
    for (u8 i = 0; i < COMMANDS_COUNT_MAX; i++) {
      int index = inputStringTemp.indexOf(INTERVAL_CHAR);
      if (index < 0) {
        break;
      }
      paramterCount = i;  //
      inputCommandArray[i] = inputStringTemp.substring(0, index);
      inputStringTemp = inputStringTemp.substring(index + 1);
      paramters[i] = inputCommandArray[i].toInt();
    }
    stringComplete = false;
    inputStringBLE = "";

    char commandChar = inputCommandArray[0].charAt(0);
    switch (commandChar)
    {
      case ACTION_MOVE:
        if (paramterCount == 2) {
          motorRun(paramters[1], paramters[2]);
        }
        break;
      case ACTION_BUZZER:
        if (paramterCount == 1) {
          setBuzzer(paramters[1]);
        }
        break;
      default:
        break;
    }
  }
}

void upLoadVoltageToApp() {
  int voltage = 0;
  if (getBatteryVoltage()) {
    voltage = batteryVoltage * 1000;
  }
  String sendString = String(ACTION_GET_VOLTAGE) + String(INTERVAL_CHAR) + String((voltage)) + String(INTERVAL_CHAR);
  Serial.println(sendString);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputStringBLE += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

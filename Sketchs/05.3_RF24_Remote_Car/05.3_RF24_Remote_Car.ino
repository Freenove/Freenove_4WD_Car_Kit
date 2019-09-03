/**********************************************************************
  Filename    : RF24_Remote_Car.ino
  Product     : Freenove 4WD Car for UNO
  Description : A RF24 Remote Car.
  Auther      : www.freenove.com
  Modification: 2019/08/06
**********************************************************************/
#include "Freenove_4WD_Car_for_Arduino.h"
#include "RF24_Remote.h"

#define NRF_UPDATE_TIMEOUT    1000

u32 lastNrfUpdateTime = 0;

void setup() {
  pinsSetup();
  if (!nrf24L01Setup()) {
    alarm(4, 2);
  }
}

void loop() {
  if (getNrf24L01Data()) {
    updateCarActionByNrfRemote();
    lastNrfUpdateTime = millis();
  }
  if (millis() - lastNrfUpdateTime > NRF_UPDATE_TIMEOUT) {
    lastNrfUpdateTime = millis();
    resetNrfDataBuf();
    updateCarActionByNrfRemote();
  }
}

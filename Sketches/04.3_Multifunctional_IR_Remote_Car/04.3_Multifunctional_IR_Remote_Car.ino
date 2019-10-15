/**********************************************************************
* Filename    : Multifunctional_IR_Remote_Car.ino
* Product     : Freenove 4WD Car for UNO
* Description : A Multifunctional IR-Remote Car.
* Auther      : www.freenove.com
* Modification: 2019/08/06
**********************************************************************/
#include "Freenove_4WD_Car_for_Arduino.h"
#include "Freenove_IR_Remote_Keycode.h"
#include "Freenove_WS2812B_RGBLED_Controller.h"
////////////////////////////////////////////
////Definitions related to IR-remote
#define IR_UPDATE_TIMEOUT     110
#define IR_CAR_SPEED          180

IRrecv irrecv(PIN_IRREMOTE_RECV);
decode_results results;
u32 currentKeyCode, lastKeyCode;
bool isStopFromIR = false;
u32 lastIRUpdateTime = 0;
////////////////////////////////////////////
////Definitions related to Led-strip
#define STRIP_I2C_ADDRESS  0x20
#define STRIP_LEDS_COUNT   10

u8 colorPos = 0;
u8 colorStep = 50;
u8 stripDisplayMode = 1;
u8 currentLedIndex = 0;
u16 stripDisplayDelay = 100;
u32 lastStripUpdateTime = 0;
Freenove_WS2812B_Controller strip(STRIP_I2C_ADDRESS, STRIP_LEDS_COUNT, TYPE_GRB);
///////////////////////////////////////////

void setup() {
  strip.begin();
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  if (irrecv.decode(&results)) {
    isStopFromIR = false;
    currentKeyCode = results.value;
    if (currentKeyCode != 0xFFFFFFFF) {
      lastKeyCode = currentKeyCode;
    }
    switch (lastKeyCode) {
      case IR_REMOTE_KEYCODE_UP:
        motorRun(IR_CAR_SPEED, IR_CAR_SPEED);
        break;
      case IR_REMOTE_KEYCODE_DOWN:
        motorRun(-IR_CAR_SPEED, -IR_CAR_SPEED);
        break;
      case IR_REMOTE_KEYCODE_LEFT:
        motorRun(-IR_CAR_SPEED, IR_CAR_SPEED);
        break;
      case IR_REMOTE_KEYCODE_RIGHT:
        motorRun(IR_CAR_SPEED, -IR_CAR_SPEED);
        break;
      case IR_REMOTE_KEYCODE_CENTER:
        setBuzzer(true);
        break;
      case IR_REMOTE_KEYCODE_0:
        break;
      case IR_REMOTE_KEYCODE_1:
        stripDisplayMode = 1;
        break;
      case IR_REMOTE_KEYCODE_2:
        colorStep += 5;
        if (colorStep > 100)
        {
          colorStep = 100;
        }
        break;
      case IR_REMOTE_KEYCODE_3:
        colorStep -= 5;
        if (colorStep < 5)
        {
          colorStep = 5;
        }
        break;
      case IR_REMOTE_KEYCODE_4:
        stripDisplayMode = 0;
        break;
      case IR_REMOTE_KEYCODE_5:
        stripDisplayDelay -= 20;
        if (stripDisplayDelay < 20)
        {
          stripDisplayDelay = 20;
        }
        break;
      case IR_REMOTE_KEYCODE_6:
        stripDisplayDelay += 20;
        if (stripDisplayDelay > 300)
        {
          stripDisplayDelay = 300;
        }
        break;
    }
    irrecv.resume(); // Receive the next value
    lastIRUpdateTime = millis();
  }
  else {
    if (millis() - lastIRUpdateTime > IR_UPDATE_TIMEOUT)
    {
      if (!isStopFromIR) {
        isStopFromIR = true;
        motorRun(0, 0);
        setBuzzer(false);
      }
      lastIRUpdateTime = millis();
    }
  }
  switch (stripDisplayMode)
  {
    case 0:
      if (millis() - lastStripUpdateTime > stripDisplayDelay)
      {
        for (int i = 0; i < STRIP_LEDS_COUNT; i++) {
          strip.setLedColorData(i, strip.Wheel(colorPos + i * 25));
        }
        strip.show();
        colorPos += colorStep;
        lastStripUpdateTime = millis();
      }
      break;
    case 1:
      if (millis() - lastStripUpdateTime > stripDisplayDelay)
      {
        strip.setLedColor(currentLedIndex, strip.Wheel(colorPos));
        currentLedIndex++;
        if (currentLedIndex == STRIP_LEDS_COUNT)
        {
          currentLedIndex = 0;
          colorPos += colorStep; //nrfDataRead[POT1] / 20;
        }
        lastStripUpdateTime = millis();
      }
      break;
    default:
      break;
  }
}

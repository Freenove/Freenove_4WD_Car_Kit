/**********************************************************************
* Filename    : IR_Remote_Car.ino
* Product     : Freenove 4WD Car for UNO
* Description : An IR-Remote Car.
* Auther      : www.freenove.com
* Modification: 2019/08/06
**********************************************************************/
#include <IRremote.h>
#include "Freenove_4WD_Car_for_Arduino.h"

//define key, the code can not be changed.
#define IR_REMOTE_KEYCODE_UP      0xFF02FD
#define IR_REMOTE_KEYCODE_DOWN    0xFF9867
#define IR_REMOTE_KEYCODE_LEFT    0xFFE01F
#define IR_REMOTE_KEYCODE_RIGHT   0xFF906F
#define IR_REMOTE_KEYCODE_CENTER  0xFFA857

#define IR_UPDATE_TIMEOUT     120
#define IR_CAR_SPEED          180

IRrecv irrecv(PIN_IRREMOTE_RECV);
decode_results results;
u32 currentKeyCode, lastKeyCode;
bool isStopFromIR = false;
u32 lastIRUpdateTime = 0;

void setup() {
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
        motorRun(IR_CAR_SPEED, IR_CAR_SPEED); //move forward
        break;
      case IR_REMOTE_KEYCODE_DOWN:
        motorRun(-IR_CAR_SPEED, -IR_CAR_SPEED); //move back
        break;
      case IR_REMOTE_KEYCODE_LEFT:
        motorRun(-IR_CAR_SPEED, IR_CAR_SPEED);  //turn left
        break;
      case IR_REMOTE_KEYCODE_RIGHT:
        motorRun(IR_CAR_SPEED, -IR_CAR_SPEED);  //turn right
        break;
      case IR_REMOTE_KEYCODE_CENTER:
        setBuzzer(true);              //turn on buzzer
        break;
      default:
        break;
    }
    irrecv.resume(); // Receive the next value
    lastIRUpdateTime = millis(); //write down current time
  }
  else {
    if (millis() - lastIRUpdateTime > IR_UPDATE_TIMEOUT) {
      if (!isStopFromIR) {
        isStopFromIR = true;
        motorRun(0, 0);
        setBuzzer(false);
      }
      lastIRUpdateTime = millis();
    }
  }
}

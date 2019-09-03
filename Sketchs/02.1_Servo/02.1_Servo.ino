/**********************************************************************
* Product     : Freenove 4WD Car for UNO
* Description : use servo.
* Auther      : www.freenove.com
* Modification: 2019/08/05
**********************************************************************/
#include "Servo.h"      //servo liberary

#define PIN_SERVO  2    //define servo pin

Servo servo;            //create servo object to control a servo
byte servoOffset = 0;   // change the value to Calibrate servo

void setup() {
  servo.attach(PIN_SERVO);        //initialize servo 
  servo.write(90 + servoOffset);  //Calibrate servo
}

void loop() {
  servo.write(45);      //make servo rotate to 45°
  delay(1000);          //delay 1000ms

  servo.write(90);      //make servo rotate to 90°
  delay(1000);

  servo.write(135);    //make servo rotate to 135°
  delay(1000);

  servo.write(90);      //make servo rotate to 90°
  delay(1000);
}

/**********************************************************************
* Product     : Freenove 4WD Car for UNO
* Description : Ultrasonic ranging and servo.
* Auther      : www.freenove.com
* Modification: 2019/08/05
**********************************************************************/
#include "Servo.h"             //include servo library
 
#define PIN_SERVO      2       //define servo pin

#define PIN_SONIC_TRIG    7    //define Trig pin
#define PIN_SONIC_ECHO    8    //define Echo pin

#define MAX_DISTANCE    300   //cm
#define SONIC_TIMEOUT   (MAX_DISTANCE*60) // calculate timeout 
#define SOUND_VELOCITY  340  //soundVelocity: 340m/s

Servo servo;             //create servo object
byte servoOffset = 0;    //change the value to Calibrate servo
u8 distance[4];          //define an arry with type u8(same to unsigned char)

void setup() {
  Serial.begin(9600);
  pinMode(PIN_SONIC_TRIG, OUTPUT);// set trigPin to output mode
  pinMode(PIN_SONIC_ECHO, INPUT); // set echoPin to input mode
  servo.attach(PIN_SERVO);        //initialize servo 
  servo.write(90 + servoOffset);  // change servoOffset to Calibrate servo
}

void loop() {
  servo.write(45);
  delay(1000);
  distance[0] = getSonar();   //get ultrsonice value and save it into distance[0]

  servo.write(90);
  delay(1000);
  distance[1] = getSonar();

  servo.write(135);
  delay(1000);
  distance[2] = getSonar();

  servo.write(90);
  delay(1000);
  distance[3] = getSonar();
  
  Serial.print("Distance L / M / R / M2:   ");  //Left/Middle/Right/Middle2
  for (int i = 0; i < 4; i++) {
    Serial.print(distance[i]);     //print ultrasonic in 45°, 90°, 135°, 90°
    Serial.print("/");
  }
  Serial.print('\n');  //next content will be printed in new line
}

float getSonar() {
  unsigned long pingTime;
  float distance;
  digitalWrite(PIN_SONIC_TRIG, HIGH); // make trigPin output high level lasting for 10μs to triger HC_SR04,
  delayMicroseconds(10);
  digitalWrite(PIN_SONIC_TRIG, LOW);
  pingTime = pulseIn(PIN_SONIC_ECHO, HIGH, SONIC_TIMEOUT); // Wait HC-SR04 returning to the high level and measure out this waitting time
  if (pingTime != 0)
    distance = (float)pingTime * SOUND_VELOCITY / 2 / 10000; // calculate the distance according to the time
  else
    distance = MAX_DISTANCE;
  return distance; // return the distance value
}

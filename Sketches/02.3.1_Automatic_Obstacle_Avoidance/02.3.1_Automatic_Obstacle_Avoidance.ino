/**********************************************************************
  Filename    : Automatic_Obstacle_Avoidance.ino
  Product     : Freenove 4WD Car for UNO
  Description : Automatic Obstacle Avoidance mode.
  Auther      : www.freenove.com
  Modification: 2019/08/15
**********************************************************************/
#include <Servo.h>
#define PIN_SERVO           2       

#define MOTOR_DIRECTION     0 //If the direction is reversed, change 0 to 1
#define PIN_DIRECTION_LEFT  4
#define PIN_DIRECTION_RIGHT 3
#define PIN_MOTOR_PWM_LEFT  6
#define PIN_MOTOR_PWM_RIGHT 5

#define PIN_SONIC_TRIG      7
#define PIN_SONIC_ECHO      8

#define PIN_BATTERY         A0

#define OBSTACLE_DISTANCE   40
#define OBSTACLE_DISTANCE_LOW 15

#define MAX_DISTANCE    1000   
#define SONIC_TIMEOUT   (MAX_DISTANCE*60)
#define SOUND_VELOCITY    340   //soundVelocity: 340m/s

Servo servo;
byte servoOffset = 0;
int speedOffset;//batteryVoltageCompensationToSpeed

void setup() {
  pinMode(PIN_DIRECTION_LEFT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_LEFT, OUTPUT);
  pinMode(PIN_DIRECTION_RIGHT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_RIGHT, OUTPUT);

  pinMode(PIN_SONIC_TRIG, OUTPUT);// set trigPin to output mode
  pinMode(PIN_SONIC_ECHO, INPUT); // set echoPin to input mode
  servo.attach(PIN_SERVO);
  calculateVoltageCompensation();
}

void loop() {
  updateAutomaticObstacleAvoidance();
}

void updateAutomaticObstacleAvoidance() {
  int distance[3], tempDistance[3][5], sumDisntance;
  static u8 leftToRight = 0, servoAngle = 0, lastServoAngle = 0;  //
  const u8 scanAngle[2][3] = { {150, 90, 30}, {30, 90, 150} };

  for (int i = 0; i < 3; i++)
  {
    servoAngle = scanAngle[leftToRight][i];
    servo.write(servoAngle);
    if (lastServoAngle != servoAngle) {
      delay(130);
    }
    lastServoAngle = servoAngle;
    for (int j = 0; j < 5; j++) {
      tempDistance[i][j] = getSonar();
      delayMicroseconds(2 * SONIC_TIMEOUT);
      sumDisntance += tempDistance[i][j];
    }
    if (leftToRight == 0) {
      distance[i] = sumDisntance / 5;
    }
    else {
      distance[2 - i] = sumDisntance / 5;
    }
    sumDisntance = 0;
  }
  leftToRight = (leftToRight + 1) % 2;

  if (distance[1] < OBSTACLE_DISTANCE) {        //Too little distance ahead
    if (distance[0] > distance[2] && distance[0] > OBSTACLE_DISTANCE) {     //Left distance is greater than right distance
      motorRun(-(150 + speedOffset), -(150 + speedOffset)); //Move back
      delay(100);
      motorRun(-(150 + speedOffset), (150 + speedOffset));  
    }
    else if (distance[0] < distance[2] && distance[2] > OBSTACLE_DISTANCE) {                   //Right distance is greater than left distance
      motorRun(-(150 + speedOffset), -(150 + speedOffset)); //Move back 
      delay(100);
      motorRun((150 + speedOffset), -(150 + speedOffset));
    }
    else {                      //Get into the dead corner, move back, then turn.
      motorRun(-(150 + speedOffset), -(150 + speedOffset));
      delay(100);
      motorRun(-(150 + speedOffset), (150 + speedOffset));
    }
  }
  else {                        //No obstacles ahead
    if (distance[0] <  OBSTACLE_DISTANCE_LOW) {      //Obstacles on the left front.
      motorRun(-(150 + speedOffset), -(150 + speedOffset)); //Move back
      delay(100);
      motorRun((180 + speedOffset), (50 + speedOffset));
    }
    else if (distance[2] <  OBSTACLE_DISTANCE_LOW) {     //Obstacles on the right front.
      motorRun(-(150 + speedOffset), -(150 + speedOffset)); //Move back
      delay(100);
      motorRun((50 + speedOffset), (180 + speedOffset));
    }
    else {                        //Cruising
      motorRun((80 + speedOffset), (80 + speedOffset));
    }
  }
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

void calculateVoltageCompensation() {
  float voltageOffset = 8.4 - getBatteryVoltage();
  speedOffset = voltageOffset * 20;
}

void motorRun(int speedl, int speedr) {
  int dirL = 0, dirR = 0;
  if (speedl > 0) {
    dirL = 0 ^ MOTOR_DIRECTION;
  } else {
    dirL = 1 ^ MOTOR_DIRECTION;
    speedl = -speedl;
  }

  if (speedr > 0) {
    dirR = 1 ^ MOTOR_DIRECTION;
  } else {
    dirR = 0 ^ MOTOR_DIRECTION;
    speedr = -speedr;
  }

  digitalWrite(PIN_DIRECTION_LEFT, dirL);
  digitalWrite(PIN_DIRECTION_RIGHT, dirR);
  analogWrite(PIN_MOTOR_PWM_LEFT, speedl);
  analogWrite(PIN_MOTOR_PWM_RIGHT, speedr);
}


float getBatteryVoltage() {
  pinMode(PIN_BATTERY, INPUT);
  int batteryADC = analogRead(PIN_BATTERY);
  float batteryVoltage = batteryADC / 1023.0 * 5.0 * 4;
  return batteryVoltage;
}

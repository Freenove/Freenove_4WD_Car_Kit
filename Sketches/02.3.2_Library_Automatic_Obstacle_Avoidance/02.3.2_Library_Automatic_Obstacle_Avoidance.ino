/**********************************************************************
* Filename    : Automatic_Obstacle_Avoidance.ino
* Product     : Freenove 4WD Car for UNO
* Description : Automatic Obstacle Avoidance mode.
* Auther      : www.freenove.com
* Modification: 2019/08/05
**********************************************************************/

#include "Freenove_4WD_Car_for_Arduino.h"
#include "Automatic_Obstacle_Avoidance_Mode.h"

void setup() {
  pinsSetup();   //from Freenove_4WD_Car_for_Arduino.h
  servoSetup();  //from Automatic_Obstacle_Avoidance_Mode.h
  oa_CalculateVoltageCompensation(); //from Automatic_Obstacle_Avoidance_Mode.h
}

void loop() {
  updateAutomaticObstacleAvoidance(); //from Automatic_Obstacle_Avoidance_Mode.h
}

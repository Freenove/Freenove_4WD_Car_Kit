/**********************************************************************
* Product     : Freenove 4WD Car for UNO
* Auther      : www.freenove.com
* Modification: 2019/08/03
**********************************************************************/
#define PIN_DIRECTION_RIGHT 3
#define PIN_DIRECTION_LEFT  4
#define PIN_MOTOR_PWM_RIGHT 5
#define PIN_MOTOR_PWM_LEFT  6

void setup() {
  pinMode(PIN_DIRECTION_LEFT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_LEFT, OUTPUT);
  pinMode(PIN_DIRECTION_RIGHT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_RIGHT, OUTPUT);
}

void loop() {
  //Right motors rotate to one direction
  digitalWrite(PIN_DIRECTION_RIGHT, HIGH);
  analogWrite(PIN_MOTOR_PWM_RIGHT, 100);
  delay(1000);
  analogWrite(PIN_MOTOR_PWM_RIGHT, 0);  
  delay(1000);
  
  //Right motors rotate to opposite direction
  digitalWrite(PIN_DIRECTION_RIGHT, LOW);
  analogWrite(PIN_MOTOR_PWM_RIGHT, 255);
  delay(1000);
  analogWrite(PIN_MOTOR_PWM_RIGHT, 0);  
  delay(1000);
}

#include "Freenove_WS2812B_RGBLED_Controller.h"

#define I2C_ADDRESS  0x20 //define I2C address of LEDs 
#define LEDS_COUNT   10   //it defines number of lEDs

#define MOTOR_DIRECTION     0 //If the direction is reversed, change 0 to 1
#define PIN_DIRECTION_LEFT  4
#define PIN_DIRECTION_RIGHT 3
#define PIN_MOTOR_PWM_LEFT  6
#define PIN_MOTOR_PWM_RIGHT 5

#define PIN_BATTERY      A0
#define PIN_BUZZER      A0
#define MOTOR_PWM_DEAD    10
float batteryVoltage = 0;
bool isBuzzered = false;

Freenove_WS2812B_Controller strip(I2C_ADDRESS, LEDS_COUNT, TYPE_GRB); //initialization

void setup() {
  while (!strip.begin()); //judge if initialization successs

  Serial.begin(9600);  //set baud rate
  pinsSetup();          //set pin mode (output or input )

  strip.setAllLedsColor(0x00FF00); //set all LED color to gree
  motorRun(100, 100);   //Car move forward
  alarm(4, 1);          //4 beat, 1 repeat
  resetCarAction();     //Stop the car and turn off the buzzer

  strip.setAllLedsColor(0xFF0000); //Set all LED color to red
  motorRun(-100, -100); //Car move back
  setBuzzer(true);      //turn on buzzer
  delay(1000);
  resetCarAction();     //Stop the car and turn off the buzzer
}

void loop() {
  if (getBatteryVoltage() == true) {
    Serial.print("voltage: ");
    Serial.println(batteryVoltage);
  }

  int r = random(0, 255);  //a random value between 0~255
  int g = random(0, 255);  //a random value between 0~255
  int b = random(0, 255);  //a random value between 0~255
  strip.setAllLedsColor(r, g, b); //Set all LED color with current r, g, b value
  delay(1000);
}

void pinsSetup() {
  pinMode(PIN_DIRECTION_LEFT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_LEFT, OUTPUT);
  pinMode(PIN_DIRECTION_RIGHT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_RIGHT, OUTPUT);
  setBuzzer(false); //Turn off the Buzzer
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

  speedl = constrain(speedl, 0, 255);
  speedr = constrain(speedr, 0, 255);

  if (abs(speedl) < MOTOR_PWM_DEAD && abs(speedr) < MOTOR_PWM_DEAD) {
    speedl = 0;
    speedr = 0;
  }
  digitalWrite(PIN_DIRECTION_LEFT, dirL);
  digitalWrite(PIN_DIRECTION_RIGHT, dirR);
  analogWrite(PIN_MOTOR_PWM_LEFT, speedl);
  analogWrite(PIN_MOTOR_PWM_RIGHT, speedr);
}

bool getBatteryVoltage() {
  if (!isBuzzered) {
    pinMode(PIN_BATTERY, INPUT);
    int batteryADC = analogRead(PIN_BATTERY);
    if (batteryADC < 614)   // 3V/12V ,Voltage read: <2.1V/8.4V
    {
      batteryVoltage = batteryADC / 1023.0 * 5.0 * 4;
      return true;
    }
  }
  return false;
}

void setBuzzer(bool flag) {
  isBuzzered = flag;
  pinMode(PIN_BUZZER, flag);
  digitalWrite(PIN_BUZZER, flag);
}

void alarm(u8 beat, u8 repeat) {
  beat = constrain(beat, 1, 9);
  repeat = constrain(repeat, 1, 255);
  for (int j = 0; j < repeat; j++) {
    for (int i = 0; i < beat; i++) {
      setBuzzer(true);
      delay(100);
      setBuzzer(false);
      delay(100);
    }
    delay(500);
  }
}

void resetCarAction() {
  motorRun(0, 0);
  setBuzzer(false);
}

#include "Freenove_4WD_Car_for_Arduino.h"
#include "Freenove_WS2812B_RGBLED_Controller.h"

#define I2C_ADDRESS  0x20 //define I2C address of LEDs 
#define LEDS_COUNT   10   //it defines number of lEDs

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

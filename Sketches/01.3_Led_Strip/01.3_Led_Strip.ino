/**********************************************************************
  Product     : Freenove 4WD Car for UNO
  Description : Control ws2812b led through freenove_controller.
  Auther      : www.freenove.com
  Modification: 2019/08/03
**********************************************************************/
#include "Freenove_WS2812B_RGBLED_Controller.h"

#define I2C_ADDRESS  0x20
#define LEDS_COUNT   10  //it defines number of lEDs. 

Freenove_WS2812B_Controller strip(I2C_ADDRESS, LEDS_COUNT, TYPE_GRB); //initialization

void setup() {
  while (!strip.begin());

  strip.setAllLedsColor(0xFF0000); //Set all LED color to red
  delay(2000);
  strip.setAllLedsColor(0x00FF00); //set all LED color to gree
  delay(2000);
  strip.setAllLedsColor(0x0000FF); //set all LED color to blue
  delay(2000);
  strip.setAllLedsColor(255, 255, 0); //set all LED color to yellow. this is just deffent form of rgb value.
  delay(2000);
  strip.setAllLedsColor(0, 0, 0);    //set all LED off .
  delay(2000);

  strip.setLedColor(0, 255, 0, 0);   //set the N0.0 LED to red
  delay(1000);
  strip.setLedColor(1, 0, 255, 0);   //set the N0.1 LED to green
  delay(1000);
  strip.setLedColor(2, 0, 0, 255);   //set the N0.2 LED to blue
  delay(1000);
  strip.setLedColor(3, 255, 255, 0); //set the N0.3 LED to yellow
  delay(1000);
  strip.setLedColor(4, 255, 0, 255); //set the N0.4 LED to purple
  delay(1000);
}

void loop() {
  for (int k = 0; k < 255; k = k + 2) {
    strip.setAllLedsColorData(strip.Wheel(k)); // set color data for all LED
    strip.show();                              // show the color set before
    delay(50);
  }
  delay(3000);

  for (int j = 0; j < 255; j += 2) {
    for (int i = 0; i < LEDS_COUNT; i++) {
      strip.setLedColorData(i, strip.Wheel(i * 256 / LEDS_COUNT + j)); //set color data for LED one by one
    }
    strip.show();  // show the color set
    delay(50);
  }
}

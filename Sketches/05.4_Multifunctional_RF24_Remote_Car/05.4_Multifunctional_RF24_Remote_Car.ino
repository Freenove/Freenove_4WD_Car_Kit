/**********************************************************************
  Filename    : Multifunctional_RF24_Remote_Car.ino
  Product     : Freenove 4WD Car for UNO
  Description : A Multifunctional RF24-Remote Car.
  Auther      : www.freenove.com
  Modification: 2019/08/08
**********************************************************************/
#include "Automatic_Tracking_Line_Mode.h"
#include "Automatic_Obstacle_Avoidance_Mode.h"
#include "Freenove_4WD_Car_for_Arduino.h"
#include "Freenove_WS2812B_RGBLED_Controller.h"
#include "RF24_Remote.h"

#define NRF_UPDATE_TIMEOUT    1000

u32 lastNrfUpdateTime = 0;
u8 nrfCarMode = OFF_OFF_OFF, lastNrfCarMode = OFF_OFF_OFF;
u8 switchModeState = MODE_SWITCHING_WAS_FINISHED;
u8 joystickSwitchState = MODE_SWITCHING_WAS_FINISHED;

#define STRIP_I2C_ADDRESS  0x20
#define STRIP_LEDS_COUNT   10

#define MAX_NUMBER_OF_DISP_MODES  3

u8 colorPos = 0;
u8 colorStep = 50;
u8 stripDisplayMode = 1;
u8 currentLedIndex = 0;
u16 stripDisplayDelay = 100;
u32 lastStripUpdateTime = 0;
Freenove_WS2812B_Controller strip(STRIP_I2C_ADDRESS, STRIP_LEDS_COUNT, TYPE_GRB);

void setup() {
  pinsSetup();
  if (!nrf24L01Setup()) {
    alarm(4, 2);
  }
  servoSetup();
  while (!strip.begin());
  strip.setAllLedsColor(0xFF0000);
}

void loop() {
  if (getNrf24L01Data()) { 
    nrfCarMode = updateNrfCarMode();
    if (nrfCarMode != lastNrfCarMode) {
      if (switchModeState == MODE_SWITCHING_WAS_FINISHED) {
        switchModeState = MODE_SWITCHING_IS_INITIALIZING;
      }
      switch (switchModeState)
      {
        case MODE_SWITCHING_IS_INITIALIZING:
          //Serial.println("Switching Mode Init...");
          resetCarAction();
          writeServo(90);
          switchModeState = MODE_SWITCHING_IS_PROCESSING;
          break;
        case MODE_SWITCHING_IS_PROCESSING:
          if (nrfDataRead[JOYSTICK_Z] == 0) {
            //Serial.println("Switching Mode ...");
            setBuzzer(true);
            switchModeState = MODE_SWITCHING_IS_CONFIRMING;
          }
          break;
        case MODE_SWITCHING_IS_CONFIRMING:
          if (nrfDataRead[JOYSTICK_Z] == 1) {
            //Serial.println("Comfirm Switched.");
            setBuzzer(false);

            switchModeState = MODE_SWITCHING_WAS_FINISHED;
            lastNrfCarMode = nrfCarMode;
            switch (nrfCarMode)
            {
              case MODE_OBSTACKE_AVOIDANCE:
                oa_CalculateVoltageCompensation();
                break;
              case MODE_LINE_TRACKING:
                tk_CalculateVoltageCompensation();
                break;
              default:
                break;
            }
            //Serial.println("Switch mode finished !");
          }
          break;
        case MODE_SWITCHING_WAS_FINISHED:
          break;
        default:
          break;
      }
    }
    else {
      if (switchModeState != MODE_SWITCHING_WAS_FINISHED) {
        switchModeState = MODE_SWITCHING_WAS_FINISHED;
      }
      switch (nrfCarMode)
      {
        case ON_ON_ON:
          break;
        case ON_ON_OFF:   //S1,S2 ON, S3 OFF
          setServoOffset(map(nrfDataRead[1], 0, 1023, -20, 20));
          if (nrfDataRead[JOYSTICK_Z] == 0) {
            setBuzzer(true);
            writeServoOffsetToEEPROM();
          }
          else {
            setBuzzer(false);
          }
          break;
        case ON_OFF_ON:
          break;
        case ON_OFF_OFF:    ////Sonic Obstacle Avoidance Mode, S1 is ON and S2, S3 are OFF
          updateAutomaticObstacleAvoidance();
          break;
        case OFF_ON_ON:
          break;
        case OFF_ON_OFF:    //Tracking Mode,S2 is ON and S1, S3 are OFF.
          updateAutomaticTrackingLine();
          break;
        case OFF_OFF_ON:    //S3 is ON and S1, S2 are OFF
          //stripDisplayMode = nrfDataRead[POT2] / 512;
          switch (joystickSwitchState)
          {
              static u8 switchCounter = 0;
            case MODE_SWITCHING_IS_INITIALIZING:
              if (nrfDataRead[JOYSTICK_X] > 900)
              {
                setBuzzer(true);
                switchCounter++;
                joystickSwitchState = MODE_SWITCHING_IS_PROCESSING;
              }
              else if (nrfDataRead[JOYSTICK_X] < 200)
              {
                setBuzzer(true);
                switchCounter--;
                joystickSwitchState = MODE_SWITCHING_IS_PROCESSING;
              }
              break;
            case MODE_SWITCHING_IS_PROCESSING:
              if ((nrfDataRead[JOYSTICK_X] < 600) && (nrfDataRead[JOYSTICK_X] > 400))
              {
                setBuzzer(false);
                joystickSwitchState = MODE_SWITCHING_IS_CONFIRMING;
              }
              break;
            case MODE_SWITCHING_IS_CONFIRMING:
              stripDisplayMode += switchCounter;
              if (stripDisplayMode == 0xff)
              {
                stripDisplayMode = MAX_NUMBER_OF_DISP_MODES - 1;
              }
              stripDisplayMode %= MAX_NUMBER_OF_DISP_MODES;
              joystickSwitchState = MODE_SWITCHING_WAS_FINISHED;
              break;
            case MODE_SWITCHING_WAS_FINISHED:
              switchCounter = 0;
              joystickSwitchState = MODE_SWITCHING_IS_INITIALIZING;
              break;
            default:
              break;
          }
          break;
        case OFF_OFF_OFF:   //Remote Mode, all switch leds are OFF
          updateCarActionByNrfRemote();
          break;
        default:
          break;
      }
    }
    colorStep = map(nrfDataRead[POT1], 0, 1023, 0, 255);
    stripDisplayDelay = nrfDataRead[POT2];
    lastNrfUpdateTime = millis();
  }

  if (millis() - lastNrfUpdateTime > NRF_UPDATE_TIMEOUT) {
    lastNrfUpdateTime = millis();
    resetNrfDataBuf();
    updateCarActionByNrfRemote();
    nrfCarMode = lastNrfCarMode = MODE_REMOTE_CONTROL;
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
          colorPos += colorStep; //
        }
        lastStripUpdateTime = millis();
      }
      break;
    case 2:
      colorPos = colorStep;
      if (millis() - lastStripUpdateTime > stripDisplayDelay)
      {
        static bool ledState = true;
        if (ledState)
        {
          strip.setAllLedsColor(strip.Wheel(colorPos));
        }
        else
        {
          strip.setAllLedsColor(0x00);
        }
        ledState = !ledState;
        lastStripUpdateTime = millis();
      }
      break;
    default:
      break;
  }
}

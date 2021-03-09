// NRF_Remote.h

#ifndef _NRF_REMOTE_h
#define _NRF_REMOTE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Freenove_4WD_Car_for_Arduino.h"
#include "RF24.h"
#include <FlexiTimer2.h>

extern RF24 radio;

enum RemoteData
{
	POT1 = 0,
	POT2 = 1,
	JOYSTICK_X = 2,
	JOYSTICK_Y = 3,
	JOYSTICK_Z = 4,
	S1 = 5,
	S2 = 6,
	S3 = 7
};
enum RemoteMode {
	ON_ON_ON = 0,
	ON_ON_OFF = 1,		//Servo calibration mode.
	ON_OFF_ON = 2,
	ON_OFF_OFF = 3,		//Ultrasonic obstacle avoidance mode.
	OFF_ON_ON = 4,
	OFF_ON_OFF = 5,		//Line tracking mode.
	OFF_OFF_ON = 6,		//
	OFF_OFF_OFF = 7			//Remote control mode.
};
enum RemoteModeSwitchState {
	MODE_SWITCHING_IS_INITIALIZING = 0,
	MODE_SWITCHING_IS_PROCESSING = 1,
	MODE_SWITCHING_IS_CONFIRMING = 2,
	MODE_SWITCHING_WAS_FINISHED = 3
};

#define MODE_REMOTE_CONTROL				OFF_OFF_OFF
#define MODE_LINE_TRACKING				OFF_ON_OFF
#define MODE_OBSTACKE_AVOIDANCE			ON_OFF_OFF

extern int nrfDataRead[8];

bool nrf24L01Setup();
void checkNrfReceived();
bool getNrf24L01Data();
void clearNrfFlag();
void updateCarActionByNrfRemote();
void resetNrfDataBuf();
u8 updateNrfCarMode();


#endif

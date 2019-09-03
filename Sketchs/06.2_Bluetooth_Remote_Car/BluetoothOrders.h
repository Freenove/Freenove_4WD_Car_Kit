// BluetoothOrders.h

#ifndef _BLUETOOTHORDERS_h
#define _BLUETOOTHORDERS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define ACTION_MOVE   'A'
#define ACTION_STOP   'B'
#define ACTION_RGB   'C'
#define ACTION_BUZZER   'D'
#define ACTION_ULTRASONIC   'E'
#define ACTION_LIGHT_TRACING   'F'
#define ACTION_TRACKING   'G'
#define ACTION_CAR_MODE   'H'
#define ACTION_GET_VOLTAGE   'I'
#define ECHO_OK   'J'
#define ACTION_NONE   'K'

#endif

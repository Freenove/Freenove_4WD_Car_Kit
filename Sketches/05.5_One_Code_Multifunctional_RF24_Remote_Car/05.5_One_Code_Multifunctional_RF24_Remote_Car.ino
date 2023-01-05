/**********************************************************************
  Filename    : Multifunctional_RF24_Remote_Car.ino
  Product     : Freenove 4WD Car for UNO
  Description : A Multifunctional RF24-Remote Car.
  Auther      : www.freenove.com
  Modification: 2019/08/08
**********************************************************************/
#include "Freenove_WS2812B_RGBLED_Controller.h"
#include <EEPROM.h>
#include "Servo.h"
#include "RF24.h"
#include <FlexiTimer2.h>

#define PIN_SERVO      2
#define MOTOR_DIRECTION     0 //If the direction is reversed, change 0 to 1
#define PIN_DIRECTION_LEFT  4
#define PIN_DIRECTION_RIGHT 3
#define PIN_MOTOR_PWM_LEFT  6
#define PIN_MOTOR_PWM_RIGHT 5
#define PIN_SONIC_TRIG    7
#define PIN_SONIC_ECHO    8
#define PIN_IRREMOTE_RECV 9
#define PIN_SPI_MOSI    11
#define PIN_SPI_MISO    12
#define PIN_SPI_SCK     13
#define PIN_BATTERY     A0
#define PIN_BUZZER      A0
#define PIN_TRACKING_LEFT A1
#define PIN_TRACKING_CENTER A2
#define PIN_TRACKING_RIGHT  A3
#define MOTOR_PWM_DEAD    10
#define BAT_VOL_STANDARD  7.0

#define STRIP_I2C_ADDRESS  0x20
#define STRIP_LEDS_COUNT   10
#define MAX_NUMBER_OF_DISP_MODES  3

#define MODE_REMOTE_CONTROL       OFF_OFF_OFF
#define MODE_LINE_TRACKING        OFF_ON_OFF
#define MODE_OBSTACKE_AVOIDANCE     ON_OFF_OFF

#define TK_STOP_SPEED          0
#define TK_FORWARD_SPEED        (90 + tk_VoltageCompensationToSpeed    )
#define TK_FORWARD_SPEED_LOW      (80 + tk_VoltageCompensationToSpeed    )
#define TK_TURN_SPEED_LV4       (180 + tk_VoltageCompensationToSpeed   )
#define TK_TURN_SPEED_LV3       (150 + tk_VoltageCompensationToSpeed   )
#define TK_TURN_SPEED_LV2       (-140 + tk_VoltageCompensationToSpeed  )
#define TK_TURN_SPEED_LV1       (-160 + tk_VoltageCompensationToSpeed  )
#define TK_SPEED_OFFSET_PER_V     30

#define OA_SERVO_CENTER          (90)
#define OA_SCAN_ANGLE_INTERVAL  50
#define OA_SCAN_ANGLE_MIN   (OA_SERVO_CENTER - OA_SCAN_ANGLE_INTERVAL)
#define OA_SCAN_ANGLE_MAX   (OA_SERVO_CENTER + OA_SCAN_ANGLE_INTERVAL)
#define OA_WAITTING_SERVO_TIME  130
#define OA_CRUISE_SPEED     (110 + oa_VoltageCompensationToSpeed)
#define OA_ROTATY_SPEED_LOW   (120 + oa_VoltageCompensationToSpeed)
#define OA_ROTATY_SPEED_NORMAL    (150 + oa_VoltageCompensationToSpeed)
#define OA_ROTATY_SPEED_HIGH    (180 + oa_VoltageCompensationToSpeed)
#define OA_TURN_SPEED_LV4     (180 + oa_VoltageCompensationToSpeed)
#define OA_TURN_SPEED_LV1     (50 + oa_VoltageCompensationToSpeed )
#define OA_BACK_SPEED_LOW     (110 + oa_VoltageCompensationToSpeed)
#define OA_BACK_SPEED_NORMAL    (150 + oa_VoltageCompensationToSpeed)
#define OA_BACK_SPEED_HIGH      (180 + oa_VoltageCompensationToSpeed)
#define OA_OBSTACLE_DISTANCE    40
#define OA_OBSTACLE_DISTANCE_LOW  15
#define OA_SPEED_OFFSET_PER_V 35
#define OA_SERVO_OFFSET_ADDR_IN_EEPROM    0
#define MAX_DISTANCE    300   //cm
#define SONIC_TIMEOUT   (MAX_DISTANCE*60)
#define SOUND_VELOCITY    340   //soundVelocity: 340m/s

u8 colorPos = 0;
u8 colorStep = 50;
u8 stripDisplayMode = 1;
u8 currentLedIndex = 0;
u16 stripDisplayDelay = 100;
u32 lastStripUpdateTime = 0;
Freenove_WS2812B_Controller strip(STRIP_I2C_ADDRESS, STRIP_LEDS_COUNT, TYPE_GRB);

float batteryVoltage = 0;
bool isBuzzered = false;

Servo servo;
char servoOffset = 0;
int oa_VoltageCompensationToSpeed;
int distance[3];
int tk_VoltageCompensationToSpeed;

#define PIN_SPI_CE      9
#define PIN_SPI_CSN     10

RF24 radio(PIN_SPI_CE, PIN_SPI_CSN);
const byte addresses[6] = "Free1";
int nrfDataRead[8];
bool nrfComplete = false;
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
  ON_ON_OFF = 1,    //Servo calibration mode.
  ON_OFF_ON = 2,
  ON_OFF_OFF = 3,   //Ultrasonic obstacle avoidance mode.
  OFF_ON_ON = 4,
  OFF_ON_OFF = 5,   //Line tracking mode.
  OFF_OFF_ON = 6,   //
  OFF_OFF_OFF = 7     //Remote control mode.
};
enum RemoteModeSwitchState {
  MODE_SWITCHING_IS_INITIALIZING = 0,
  MODE_SWITCHING_IS_PROCESSING = 1,
  MODE_SWITCHING_IS_CONFIRMING = 2,
  MODE_SWITCHING_WAS_FINISHED = 3
};

#define NRF_UPDATE_TIMEOUT    1000
u32 lastNrfUpdateTime = 0;
u8 nrfCarMode = OFF_OFF_OFF, lastNrfCarMode = OFF_OFF_OFF;
u8 switchModeState = MODE_SWITCHING_WAS_FINISHED;
u8 joystickSwitchState = MODE_SWITCHING_WAS_FINISHED;

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
    clearNrfFlag();
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

/////////
void servoSetup() {
  getServoOffsetFromEEPROM();
  servo.attach(PIN_SERVO);
  servo.write(90 + servoOffset);
}

void setServoOffset(char offset) {
  servoOffset = offset = constrain(offset, -100, 100);
  servo.write(90 + offset);
}

void writeServo(u8 n)
{
  servo.write(90 + servoOffset);
}

void writeServoOffsetToEEPROM() {
  servo.write(90 + servoOffset);
  EEPROM.write(OA_SERVO_OFFSET_ADDR_IN_EEPROM, servoOffset);
}

void getServoOffsetFromEEPROM() {
  servoOffset = EEPROM.read(OA_SERVO_OFFSET_ADDR_IN_EEPROM);
  servoOffset = constrain(servoOffset, -10, 10);
}

float getSonar() {
  unsigned long pingTime;
  float distance;
  digitalWrite(PIN_SONIC_TRIG, HIGH); // make trigPin output high level lasting for 10��s to triger HC_SR04,
  delayMicroseconds(10);
  digitalWrite(PIN_SONIC_TRIG, LOW);
  pingTime = pulseIn(PIN_SONIC_ECHO, HIGH, SONIC_TIMEOUT); // Wait HC-SR04 returning to the high level and measure out this waitting time
  if (pingTime != 0)
    distance = (float)pingTime * SOUND_VELOCITY / 2 / 10000; // calculate the distance according to the time
  else
    distance = MAX_DISTANCE;
  return distance; // return the distance value
}

void oa_CalculateVoltageCompensation() {
  getBatteryVoltage();
  float voltageOffset = BAT_VOL_STANDARD - batteryVoltage;
  oa_VoltageCompensationToSpeed = voltageOffset * OA_SPEED_OFFSET_PER_V;
  /*Serial.print(voltageOffset);
  Serial.print('\t');
  Serial.println(oa_VoltageCompensationToSpeed);*/
}

void updateAutomaticObstacleAvoidance() {
  int tempDistance[3][5], sumDisntance;
  static u8 cnt = 0, servoAngle = 0, lastServoAngle = 0;  //
  if (cnt == 0) {
    for (int i = 0; i < 3; i++) {
      servoAngle = OA_SCAN_ANGLE_MAX - i * OA_SCAN_ANGLE_INTERVAL + servoOffset;
      servo.write(servoAngle);
      if (lastServoAngle != servoAngle) {
        delay(OA_WAITTING_SERVO_TIME);
      }
      lastServoAngle = servoAngle;
      for (int j = 0; j < 5; j++) {
        tempDistance[i][j] = getSonar();
        delayMicroseconds(2 * SONIC_TIMEOUT);
        sumDisntance += tempDistance[i][j];
      }
      distance[i] = sumDisntance / 5;
      sumDisntance = 0;
    }
    cnt++;
  }
  else {
    for (int i = 2; i > 0; i--) {
      servoAngle = OA_SCAN_ANGLE_MAX - i * OA_SCAN_ANGLE_INTERVAL + servoOffset;
      servo.write(servoAngle);
      if (lastServoAngle != servoAngle) {
        delay(OA_WAITTING_SERVO_TIME);
      }
      lastServoAngle = servoAngle;
      for (int j = 0; j < 5; j++) {
        tempDistance[i][j] = getSonar();
        delayMicroseconds(2 * SONIC_TIMEOUT);
        sumDisntance += tempDistance[i][j];
      }
      distance[i] = sumDisntance / 5;
      sumDisntance = 0;
    }
    cnt = 0;
  }

  if (distance[1] < OA_OBSTACLE_DISTANCE) {       //Too little distance ahead
    if (distance[0] > OA_OBSTACLE_DISTANCE || distance[2] > OA_OBSTACLE_DISTANCE) {
      motorRun(-OA_BACK_SPEED_LOW, -OA_BACK_SPEED_LOW); //Move back a little
      delay(100);
      if (distance[0] > distance[2]) {      //Left distance is greater than right distance
        motorRun(-OA_ROTATY_SPEED_LOW, OA_ROTATY_SPEED_LOW);
      }
      else {                    //Right distance is greater than left distance
        motorRun(OA_ROTATY_SPEED_LOW, -OA_ROTATY_SPEED_LOW);
      }
    }
    else {                      //Get into the dead corner, move back a little, then spin.
      motorRun(-OA_BACK_SPEED_HIGH, -OA_BACK_SPEED_HIGH);
      delay(100);
      motorRun(-OA_ROTATY_SPEED_NORMAL, OA_ROTATY_SPEED_NORMAL);
    }
  }
  else {                        //No obstacles ahead
    if (distance[0] < OA_OBSTACLE_DISTANCE) {     //Obstacles on the left front.
      if (distance[0] < OA_OBSTACLE_DISTANCE_LOW) { //Very close to the left front obstacle.
        motorRun(-OA_BACK_SPEED_LOW, -OA_BACK_SPEED_LOW); //Move back
        delay(100);
      }
      motorRun(OA_TURN_SPEED_LV4, OA_TURN_SPEED_LV1);
    }
    else if (distance[2] < OA_OBSTACLE_DISTANCE) {      //Obstacles on the right front.
      if (distance[2] < OA_OBSTACLE_DISTANCE_LOW) {   //Very close to the right front obstacle.
        motorRun(-OA_BACK_SPEED_LOW, -OA_BACK_SPEED_LOW); //Move back
        delay(100);
      }
      motorRun(OA_TURN_SPEED_LV1, OA_TURN_SPEED_LV4);
    }
    else {                        //Cruising
      motorRun(OA_CRUISE_SPEED, OA_CRUISE_SPEED);
    }
  }
}

////////
void tk_CalculateVoltageCompensation() {
  getBatteryVoltage();
  float voltageOffset = BAT_VOL_STANDARD - batteryVoltage;
  tk_VoltageCompensationToSpeed = voltageOffset * TK_SPEED_OFFSET_PER_V;
  /*Serial.print(voltageOffset);
  Serial.print('\t');
  Serial.println(tk_VoltageCompensationToSpeed);*/
}

//The black line is detected to be high, whereas the white object (the reflected signal) is detected to be low.
//left center right -->0111
u8 getTrackingSensorVal() {
  u8 trackingSensorVal = 0;
  trackingSensorVal = (digitalRead(PIN_TRACKING_LEFT) == 1 ? 1 : 0) << 2 | (digitalRead(PIN_TRACKING_CENTER) == 1 ? 1 : 0) << 1 | (digitalRead(PIN_TRACKING_RIGHT) == 1 ? 1 : 0) << 0;
  return trackingSensorVal;
}

void updateAutomaticTrackingLine() {
  u8 trackingSensorVal = 0;
  trackingSensorVal = getTrackingSensorVal();
  //Serial.print(trackingSensorVal, BIN);
  switch (trackingSensorVal)
  {
  case 0:   //000
    motorRun(TK_FORWARD_SPEED, TK_FORWARD_SPEED);
    break;
  case 7:   //111
    motorRun(TK_STOP_SPEED, TK_STOP_SPEED);
    break;
  case 1:   //001
    motorRun(TK_TURN_SPEED_LV4, TK_TURN_SPEED_LV1);
    break;
  case 3:   //011
    motorRun(TK_TURN_SPEED_LV3, TK_TURN_SPEED_LV2);
    break;
  case 2:   //010
  case 5:   //101
    motorRun(TK_FORWARD_SPEED, TK_FORWARD_SPEED);
    break;
  case 6:   //110
    motorRun(TK_TURN_SPEED_LV2, TK_TURN_SPEED_LV3);
    break;
  case 4:   //100
    motorRun(TK_TURN_SPEED_LV1, TK_TURN_SPEED_LV4);
    break;
  default:
    break;
  }
  //Serial.println();
  //delay(10);
}
/////////

bool nrf24L01Setup() {
  // NRF24L01
  if (radio.begin()) {                      // initialize RF24
    radio.setPALevel(RF24_PA_MAX);      // set power amplifier (PA) level
    radio.setDataRate(RF24_1MBPS);      // set data rate through the air
    radio.setRetries(0, 15);            // set the number and delay of retries
    radio.openWritingPipe(addresses);   // open a pipe for writing
    radio.openReadingPipe(1, addresses);// open a pipe for reading
    radio.startListening();             // start monitoringtart listening on the pipes opened
    
    FlexiTimer2::set(20, 1.0 / 1000, checkNrfReceived); // call every 20 1ms "ticks"
    FlexiTimer2::start();

	return true;
  }
  return false;
}

void checkNrfReceived() {
 
  delayMicroseconds(1000);
  if (radio.available()) {             // if receive the data
    while (radio.available()) {         // read all the data
      radio.read(nrfDataRead, sizeof(nrfDataRead));   // read data
    }
    nrfComplete = true;
    return;
  }
  nrfComplete = false;
}

bool getNrf24L01Data()
{
  return nrfComplete;
}

void clearNrfFlag() {
  nrfComplete = 0;
}





void updateCarActionByNrfRemote() {
  int x = nrfDataRead[2] - 512;
  int y = nrfDataRead[3] - 512;
  int pwmL, pwmR;
  if (y < 0) {
    pwmL = (-y + x) / 2;
    pwmR = (-y - x) / 2;
  }
  else {
    pwmL = (-y - x) / 2;
    pwmR = (-y + x) / 2;
  }
  motorRun(pwmL, pwmR);

  if (nrfDataRead[4] == 0) {
    setBuzzer(true);
  }
  else {
    setBuzzer(false);
  }
}

void resetNrfDataBuf() {
  nrfDataRead[0] = 0;
  nrfDataRead[1] = 0;
  nrfDataRead[2] = 512;
  nrfDataRead[3] = 512;
  nrfDataRead[4] = 1;
  nrfDataRead[5] = 1;
  nrfDataRead[6] = 1;
  nrfDataRead[7] = 1;
}

u8 updateNrfCarMode() {
  // nrfDataRead [5 6 7] --> 111
  return ((nrfDataRead[5] == 1 ? 1 : 0) << 2) | ((nrfDataRead[6] == 1 ? 1 : 0) << 1) | ((nrfDataRead[7] == 1 ? 1 : 0) << 0);
}

/////////
void pinsSetup() {
  pinMode(PIN_DIRECTION_LEFT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_LEFT, OUTPUT);
  pinMode(PIN_DIRECTION_RIGHT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_RIGHT, OUTPUT);

  pinMode(PIN_SONIC_TRIG, OUTPUT);// set trigPin to output mode
  pinMode(PIN_SONIC_ECHO, INPUT); // set echoPin to input mode

  pinMode(PIN_TRACKING_LEFT, INPUT); // 
  pinMode(PIN_TRACKING_RIGHT, INPUT); // 
  pinMode(PIN_TRACKING_CENTER, INPUT); // 

  setBuzzer(false);
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
/////////

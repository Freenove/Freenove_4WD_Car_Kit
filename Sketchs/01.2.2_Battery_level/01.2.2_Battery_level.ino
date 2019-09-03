/**********************************************************************
  Product     : Freenove 4WD Car for UNO
  Description : Use buzzer and read battery voltage.
  Auther      : www.freenove.com
  Modification: 2019/08/03
**********************************************************************/
#define PIN_BATTERY     A0
#define PIN_BUZZER      A0

float batteryVoltage = 0;

void setup() {
  Serial.begin(9600); //set baud rate
  pinMode(PIN_BUZZER, OUTPUT);

  digitalWrite(PIN_BUZZER, HIGH);
  delay(500);
  digitalWrite(PIN_BUZZER, LOW);
  delay(500);
  digitalWrite(PIN_BUZZER, HIGH);
  delay(500);
  digitalWrite(PIN_BUZZER, LOW);
}

void loop() {
  //Because the buzzer and battery voltage detection use A0 pin together,
  //the buzzer must be turned off when reading battery voltage,
  //otherwise the read battery voltage is incorrect.

  //Turn off buzzer
  pinMode(PIN_BUZZER, false);
  digitalWrite(PIN_BUZZER, false);

  //print battery voltage
  if (getBatteryVoltage()) {
    Serial.print("Battery Voltage : ");
    Serial.print(batteryVoltage);
    Serial.println(" V");
  }
  else {
    Serial.print("Port busy! Please turn off buzzer before reading battery voltage.");
  }
  delay(1000);
}

bool getBatteryVoltage() {
  pinMode(PIN_BATTERY, INPUT);
  int batteryADC = analogRead(PIN_BATTERY);
  if (batteryADC < 614)                      // 3V/12V ,Voltage read: <2.1V/8.4V
  {
    batteryVoltage = batteryADC / 1023.0 * 5.0 * 4;
    return true;
  }else {
    return false;
  }
}

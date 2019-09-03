/**********************************************************************
  Product     : Freenove 4WD Car for UNO
  Description : Use buzzer 
  Auther      : www.freenove.com
  Modification: 2019/08/03
**********************************************************************/
#define PIN_BATTERY     A0
#define PIN_BUZZER      A0

void setup() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, HIGH);
  delay(100);
  digitalWrite(PIN_BUZZER, LOW);
}

void loop() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(PIN_BUZZER, HIGH); //turn on buzzer
    delay(100);
    digitalWrite(PIN_BUZZER, LOW);  //turn off buzzer
    delay(100);
  }
  delay(500);
}

/**********************************************************************
* Filename    : Receive_Bluetooth_Data.ino
* Product     : Freenove 4WD Car for UNO
* Description : Receive data from bluetooth and print it to monitor.
* Auther      : www.freenove.com
* Modification: 2019/08/24
* Notes       : This code comes from the sample program SerialEvent.ino.
**********************************************************************/
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial.println("AT+NAMEBT05");  // Or use Serial.print("AT+NAMEBT05\r\n");
  delay(200);
  Serial.println("AT+ROLE0");
  delay(200);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

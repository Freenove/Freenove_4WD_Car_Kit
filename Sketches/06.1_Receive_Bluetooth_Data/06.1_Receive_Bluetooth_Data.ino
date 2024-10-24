String inputString = "";         // a String to hold incoming data  
bool stringComplete = false;     // whether the string is complete  
long baudRate[] = {2400, 9600, 19200, 115200};  
int baudRateCount = sizeof(baudRate) / sizeof(baudRate[0]);  
  
void setup() {  
  // Loop through each baud rate and send commands  
  for (int i = 0; i < baudRateCount; i++) {  
    Serial.begin(baudRate[i]);  
    delay(100);  
    Serial.flush();
    Serial.println("AT+NAME=BT05");  // Or use Serial.print("AT+NAMEBT05-BLE\r\n");  
    delay(200);  
    Serial.println("AT+ROLE=0");  
    delay(200);  
    Serial.println("AT+UART=4");  
    delay(200);  
  }  
  
  // Finally, set the serial to the desired baud rate (e.g., 115200)  
  Serial.begin(115200);  
    
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
/**********************************************************************
  Filename    : IR_Receiver.ino
  Product     : Freenove 4WD Car for UNO
  Description : A demo of uasage of IR-Recveiver.
  Auther      : www.freenove.com
  Modification: 2019/08/06
**********************************************************************/
#include <IRremote.h>             // incudle IRremote library
#define PIN_IRREMOTE_RECV  9      // Infrared receiving pin

IRrecv irrecv(PIN_IRREMOTE_RECV); // Create a class object used to receive

decode_results results;           // Create a decoding results class object

void setup()
{
  Serial.begin(9600);         // Initialize the serial port and set the baud rate to 9600
  irrecv.enableIRIn();        // Start the receiver
}

void loop() {
  if (irrecv.decode(&results)) {        // Waiting for decoding
    Serial.println(results.value, HEX); // Print out the decoded results
    irrecv.resume();                    // Receive the next value
  }
  delay(100);
}

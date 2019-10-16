/**********************************************************************
  Filename    : Receive_Data.ino
  Product     : Freenove 4WD Car for UNO
  Description :Receive data from remote control and print it to monitor.
  Auther      : www.freenove.com
  Modification: 2019/08/06
**********************************************************************/
#include <SPI.h>
#include "RF24.h"

#define PIN_SPI_CE      9
#define PIN_SPI_CSN     10

RF24 radio(PIN_SPI_CE, PIN_SPI_CSN); // define an object to control NRF24L01
const byte addresses[6] = "Free1";   //set commucation address, same to remote controller
int nrfDataRead[8];            //define an array to save data from remote controller
void setup() {
  Serial.begin(9600);

  // NRF24L01
  if (radio.begin()) {                  // initialize RF24
    radio.setPALevel(RF24_PA_MAX);      // set power amplifier (PA) level
    radio.setDataRate(RF24_1MBPS);      // set data rate through the air
    radio.setRetries(0, 15);            // set the number and delay of retries
    radio.openWritingPipe(addresses);   // open a pipe for writing
    radio.openReadingPipe(1, addresses);// open a pipe for reading
    radio.startListening();             // start monitoringtart listening on the pipes opened
    Serial.println("Start listening remote data ... ");
  }
  else {
    Serial.println("Not found the nrf chip!");
  }

}

void loop() {
  delayMicroseconds(1000);
  if (radio.available()) {              // if receive the data
    while (radio.available()) {         // read all the data
      radio.read(nrfDataRead, sizeof(nrfDataRead));   // read data
    }
    Serial.print("P1/P2/X/Y/Z/S1/S2/S3 : ");
    for (int i = 0; i < sizeof(nrfDataRead) / 2; i++) {
      Serial.print(nrfDataRead[i]);
      Serial.print('\t');
    }
    Serial.print('\n');
  }
}

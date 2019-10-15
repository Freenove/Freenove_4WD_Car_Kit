#include "RF24_Remote.h"

RF24 radio(PIN_SPI_CE, PIN_SPI_CSN);
const byte addresses[6] = "Free1";
int nrfDataRead[8];

bool nrf24L01Setup() {
	// NRF24L01
	if (radio.begin()) {                      // initialize RF24
		radio.setPALevel(RF24_PA_LOW);      // set power amplifier (PA) level
		radio.setDataRate(RF24_1MBPS);      // set data rate through the air
		radio.setRetries(0, 15);            // set the number and delay of retries
		radio.openWritingPipe(addresses);   // open a pipe for writing
		radio.openReadingPipe(1, addresses);// open a pipe for reading
		radio.startListening();             // start monitoringtart listening on the pipes opened
		return true;
	}
	return false;
}

bool getNrf24L01Data()
{
  delayMicroseconds(1000);
  if (radio.available()) {             // if receive the data
    while (radio.available()) {         // read all the data
      radio.read(nrfDataRead, sizeof(nrfDataRead));   // read data
    }
    return true;
  }
  return false;
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

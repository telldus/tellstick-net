#include <htc.h>
#include "eeprom.h"

void clearEEPROM(unsigned int size) {
	unsigned char i;

	for(i = 0 ; i < size; i++) {
		EEByteWrite(0xA0, i, 0xFF);
		EEAckPolling(0xA0);
	}
}

void initEEPROM() {
	TRISC3 = 1; //Inputs
	TRISC4 = 1;
	OpenI2C(MASTER, SLEW_OFF);
	SSPADD = 67; //100 kHz
}

void saveEEPROM(unsigned char *ptr, unsigned int size) {
	unsigned char i;

	for(i = 0 ; i < size; i++) {
		EEByteWrite(0xA0, i, *ptr++);
		EEAckPolling(0xA0);
	}
}

void loadEEPROM(unsigned char *ptr, unsigned int size ) {
	/*
	* read configuration from EEPROM
	*/
	unsigned char   i ;
	for(i = 0 ; i < size; i++) {
		*ptr++ = EERandomRead(0xA0,i);
	}
}

//
// Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <htc.h>
#include "HardwareProfile.h"
#include "send.h"

#define Lo(param) ((char *)&param)[0]
#define Hi(param) ((char *)&param)[1]

const int FACTOR = 103;
unsigned int times[4];

unsigned char sendPacket[200];

void initSender() {
	/**
	* Clock 1: 25 MHz
	* Prescaler: 1:1
	* Timer: 16 bit
	* Timeout after ?
	**/

	// Prescaler 1:1
	T1CKPS1 = 0;
	T1CKPS0 = 0;

	TMR1CS = 0; //Clock source = internal clock (Fosc/4)

	TMR1ON = 0; // Turn off timer
	TMR1IE = 0; // No interrupts

	//SPDT control
	TX_ANT_TRIS = 0; //Output
	TX_ANT = 0;
}

void sendCommand() {
	unsigned char *j;
	unsigned int nextTimer;
	char status = 0;

	TMR1ON = 0; // Stop timer

	/**
	* The reason we set the flag to 1 is because we don't want to wait for
	* the timer the first time.
	* The first time there is no timer-value calculated ;-)
	*/
	TMR1IF = 1;                       // Set the timer-flag

	for (j=sendPacket; (*j) != '\0'; ++j) {
		if ( (*j) == 1 ) {
			status=~status;
			continue;
		}
		nextTimer = 0xFFFF - ((*j)*FACTOR);

		while(!TMR1IF) {}

		TMR1ON = 0; // Stop timer
		TMR1H = Hi(nextTimer);
		TMR1L = Lo(nextTimer);
		TMR1IF = 0;
		status=~status;
		SENDER=status;
		TMR1ON = 1; //Turn on timer
	}
	while(!TMR1IF) {}
	SENDER=0;
}

void sendExtendedCommand() {
	unsigned char *j;
	unsigned char i;
	unsigned char pulses;
	unsigned char time;
	unsigned char timePointer;
	unsigned char status = 0;
	unsigned int nextTimer; // The value to start the timer with

	char txt[4];

	//Start off by calculating timer-values
	j=sendPacket;
	for(i=0;i<4;++i) {
		if ((*j) == 1) {
			times[i] = 0xFFFF;
		} else {
			times[i] = 0xFFFF - ((*j)*FACTOR);
		}
		j++;
	}

	pulses = (*j);
	j++;

	TMR1ON = 0; // Stop Timer1
	TMR1IF = 1;  // See comment in sendCommand() why we set this to 1

	for (;pulses > 0;j++) {
		time = (*j);

		for (i=0;i<4;++i) {
			timePointer = (time & 0b11000000) >> 6;
			nextTimer = times[timePointer];
			time <<= 2;                        //Shift two bits
			if (nextTimer == 0xFFFF) {
				status=~status;
				continue;
			}
			while( !TMR1IF ) {}           //Wait until timer overflows

			TMR1ON = 0;                  // Stop timer
			TMR1IF = 0;                   // Clear flag
			TMR1H = Hi(nextTimer);             // Load timer
			TMR1L = Lo(nextTimer);
			status=~status;
			SENDER=status;                    // Toggle antenna
			TMR1ON = 1;                  // Start Timer1

			pulses--;
			if (pulses < 1) {
				break;
			}
		}
	}
	while( !TMR1IF ) {}               //Wait until last timer overflows
	SENDER=0;
}

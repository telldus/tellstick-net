//
// Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "rf.h"
#include "config.h"
#include "transmit.h"
#include "HardwareProfile.h"
#include <htc.h>

void activateReceiver() {
	TX_ANT = 0;
	RX_ANT = 1;
}

void activateTransmitter() {
	TX_ANT = 1;
	RX_ANT = 0;
}

void initReceiver() {
	// Prescaler 1:1
	T4CKPS1 = 0;
	T4CKPS0 = 0;

	// Postscaler 1:7
	T4OUTPS3 = 0;
	T4OUTPS2 = 0;
	T4OUTPS1 = 1;
	T4OUTPS0 = 0;

	//T3RD16 = 0; //Read/Write in 16-bit mode

	TMR4ON = 1; // Turn on timer
	TMR4IF = 0;
	PR4 = 0xFF;
	TMR4IE = 1; // Enable interrupt on timer2


	//SPDT control
	RX_ANT_TRIS = 0; //Output
	RECEIVER_TRIS = 1; //Input

	//High priority interrupt
	TMR4IP = 1;
}

void initTransmitter() {
	//SPDT control
	TX_ANT_TRIS = 0; //Output

	//The SENDER is an output
	SENDER_TRIS = 0;

	SENDER = 0;

	//Timer 3

	T3CKPS1 = 0;
	T3CKPS0 = 0;

	TMR3IE = 1;
	TMR3IP = 1;
	TMR3ON = 0;
}

void rfStartTransmit() {
	TMR4ON=0; //Turn off receive sampling
	activateTransmitter();
}

void rfStopTransmit() {
	activateReceiver();
	TMR3ON = 0;
	TMR4ON = 1; //Turn on receive sampling
}

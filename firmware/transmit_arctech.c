//
// Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <htc.h>

#include "TCPIP Stack/TCPIP.h"
#include "livemessage.h"
#include "pwm.h"
#include "send.h"
#include "transmit.h"

#define LO(param) ((char *)&param)[0]
#define HI(param) ((char *)&param)[1]
#define HIGHER(param) ((char *)&param)[2]
#define HIGHEST(param) ((char *)&param)[3]

#define START_LONG_P  3130  //2,72 ms
#define SHORT_P       310   //270 us
#define LONG_P        1510  //1,31 ms

static long house = 0;
static int unit = 0, method = 0;

static void sendOne() {
	SENDER = 1;
	__delay_us(SHORT_P);
	SENDER = 0;
	__delay_us(LONG_P);
	SENDER = 1;
	__delay_us(SHORT_P);
	SENDER = 0;
	__delay_us(SHORT_P);
}

static void sendZero() {
	SENDER = 1;
	__delay_us(SHORT_P);
	SENDER = 0;
	__delay_us(SHORT_P);
	SENDER = 1;
	__delay_us(SHORT_P);
	SENDER = 0;
	__delay_us(LONG_P);
}

static void sendBits( unsigned char chCode, char bits ) {
	char mask = 1;
	char i = 0;

	mask = 1<<(bits-1);
	for (i = 0; i < bits; ++i) {
		if (chCode & mask) {
			sendOne();
		} else {
			sendZero();
		}
		mask = mask >> 1;
	}
}

static void sendArctechCommand() {
	//Start pulse
	SENDER = 1;
	__delay_us(SHORT_P);
	SENDER = 0;
	__delay_us(START_LONG_P);

	//Housecode
	sendBits(HIGHEST(house), 2);
	sendBits(HIGHER(house), 8);
	sendBits(HI(house), 8);
	sendBits(LO(house), 8);

	//Group
	sendZero();

	//On or Off
	if (method == 1) {
		sendOne();
	} else {
		sendZero();
	}

	//Button
	sendBits(unit, 4);

	//Stop pulse
	SENDER = 1;
	__delay_us(SHORT_P);
	SENDER = 0;
}

void sendArctechSelflearning() {
	if (!LMFindHashString("house")) {
		return;
	}
	house = LMTakeInt();
	if (!LMFindHashString("unit")) {
		return;
	}
	unit = LMTakeInt();
	if (!LMFindHashString("method")) {
		return;
	}
	method = LMTakeInt();

	rfStartTransmit();
	for(unsigned char i = 0; i < DEFAULT_REPEATS; ++i) {
		setTXPulses(2);
		sendArctechCommand();
		for(unsigned char j = 0; j < DEFAULT_PAUSE; ++j) {
			__delay_ms(1);
		}
	}
	rfStopTransmit();
}

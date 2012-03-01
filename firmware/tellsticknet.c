//
// Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <htc.h>
#include "TCPIP Stack/TCPIP.h"
#include "TCPIP Stack/Helpers.h"

#include "common.h"
#include "livemessage.h"
#include "pwm.h"
#include "send.h"
#include "transmit.h"

void send() {
	unsigned char pause = DEFAULT_PAUSE;
	unsigned char repeats = DEFAULT_REPEATS;
	unsigned char i, j;
	unsigned long ack;

	if (!LMEnterHash()) {
		return;
	}

	if (LMFindHashString("P")) {
		pause = LMTakeInt();
	}
	if (LMFindHashString("R")) {
		repeats = LMTakeInt();
	}

	if (!LMFindHashString("S")) {
		return;
	}
	if (!LMTakeString(&sendPacket, sizeof(sendPacket))) {
		return;
	}

	rfStartTransmit();
	for(unsigned char i = 0; i < repeats; ++i) {
		setTXPulses(2);
		rfSend(&sendPacket);
		for(unsigned char j = 0; j < pause; ++j) {
			__delay_ms(1);
		}
	}
	rfStopTransmit();
}

void handleMessage() {
	BYTE name[20] = "";

	if (!LMTakeString(&name, sizeof(name))) {
		printf("Could not handle message (to long?)\r\n");
		return;
	}
	if (strcmp(name, "send") == 0) {
		send();
	} else if (strcmp(name, "disconnect") == 0) {
		Reset();
	}
}

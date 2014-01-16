//
// Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "message.h"

#include "TCPIP Stack/TCPIP.h"
#include "pwm.h"
#include <htc.h>
#include <stdio.h>
#include "common.h"
#include "livemessage.h"
#include "tellsticknet.h"

void rfMessageBegin() {
#ifdef DEBUG
	printf("+R");
#endif
	LMClear();
	LMAppendString("Data");
	LMStartHash();
}

void rfMessageBeginRaw() {
#ifdef DEBUG
	printf("+RAW");
#endif
	LMClear();
	LMAppendString("RawData");
	LMStartHash();
}

void rfMessageEnd(unsigned char type) {
#ifdef DEBUG
	printf("\r\n");
#endif
	LMEndHash();
	sendToLocalListeners();
	setRXPulses(type);
}

void rfMessageAddByte(const char *key, unsigned char value) {
#ifdef DEBUG
	printf("%s:%X;", key, value);
#endif
	LMAppendHashInt(key, value);
}

void rfMessageAddLong(const char *key, unsigned long value) {
	LMAppendHashInt(key, value);
#ifdef DEBUG
	printf("%s:", key);

	unsigned char first = 1;
	for(unsigned char i = 0; i < 2*sizeof(value); ++i) {
		//Mask (and shift) out the highest nibble
		unsigned char nibble = (unsigned char)(value >> (8*sizeof(value)-4)) & 0xF;

		if (nibble == 0 && first == 1) {
			//Don't send prepending zeroes
		} else {
			printf("%X", nibble);
			first = 0;
		}
		value <<= 4;
	}
	printf(";");
#endif
}

void rfMessageAddString(const char *key, const char *value) {
#ifdef DEBUG
	printf("%s:%s;", key, value);
#endif
	LMAppendHashString(key, value);
}

void rfMessageAddHexString(const char *key, const char *value, const unsigned char length) {
#ifdef DEBUG
	printf("%s:", key);
	for(unsigned char i = 0; i < length; ++i) {
		printf("%X", value[i]);
	}
	printf(";");
#endif
	LMAppendHashHexString(key, value, length);
}

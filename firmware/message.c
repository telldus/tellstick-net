#include "message.h"

#include "TCPIP Stack/TCPIP.h"
#include "pwm.h"
#include <htc.h>
#include <stdio.h>
#include "common.h"

void rfMessageBegin() {
#ifdef DEBUG
	printf("+R");
#endif
}

void rfMessageBeginRaw() {
#ifdef DEBUG
	printf("+RAW");
#endif
}

void rfMessageEnd(unsigned char type) {
#ifdef DEBUG
	printf("\r\n");
#endif
	setRXPulses(type);
}

void rfMessageAddByte(const char *key, unsigned char value) {
#ifdef DEBUG
	printf("%s:%X;", key, value);
#endif
}

void rfMessageAddLong(const char *key, unsigned long value) {
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
}

void rfMessageAddHexString(const char *key, const char *value, const unsigned char length) {
#ifdef DEBUG
	printf("%s:", key);
	for(unsigned char i = 0; i < length; ++i) {
		printf("%X", value[i]);
	}
	printf(";");
#endif
}

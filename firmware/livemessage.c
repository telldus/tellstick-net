//
// Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <htc.h>
#include <string.h>
#include "TCPIP Stack/TCPIP.h"
#include "common.h"
#include "config.h"

typedef enum {
	SM_CLEARED = 0,
	SM_PARSE_SIGNATURE,
	SM_PARSE_LENGTH,
	SM_PARSE_MESSAGE,
	SM_MESSAGE_RECEIVED,
} SM_LM;

static SM_LM state = SM_CLEARED;

static char buffer[200];
static BYTE bufferP;
static BYTE hashStartP;
static BYTE signature[20];
static BYTE signature2[20];
static ROM BYTE PRIVATE_KEY[] = TELLDUS_LIVE_PRIVATE_KEY;

void LMAppendChar(const char ch) {
	char *pos = strchr(&buffer, '\0');
	if (pos != NULL) {
		(*pos) = ch;
	}
}

void LMClear() {
	memset(&buffer, '\0', sizeof(buffer));
	bufferP = 0;
	state = SM_CLEARED;
}

void LMAppendString(const char *string) {
	BYTE hb = btohexa_high(strlen(string));
	BYTE lb = btohexa_low(strlen(string));
	if (hb != '0') {
		LMAppendChar(hb);
	}
	LMAppendChar(lb);
	LMAppendChar(':');
	strcat(&buffer, string);
}

void LMAppendRomString(ROM BYTE *string) {
	BYTE hb = btohexa_high(strlen(string));
	BYTE lb = btohexa_low(strlen(string));
	if (hb != '0') {
		LMAppendChar(hb);
	}
	LMAppendChar(lb);
	LMAppendChar(':');
	strcat(&buffer, string);
}

void LMAppendInt(unsigned long value) {
	char hex[sizeof(value)*2], i;
	LMAppendChar('i');
	if (value == 0) {
		LMAppendChar('0');
	} else {
		for(i=0; i < sizeof(hex); ++i) {
			hex[i] = btohexa_low(value);
			value >>= 4;
		}

		//Skip all prepending zeros
		for(i=sizeof(hex); i > 0; --i) {
			if (hex[i-1] != '0') {
				break;
			}
		}
		for(; i > 0; --i) {
			LMAppendChar(hex[i-1]);
		}
	}
	LMAppendChar('s');
}

void LMStartHash() {
	LMAppendChar('h');
}

void LMAppendHashString(ROM BYTE *key, const char *value) {
	LMAppendRomString(key);
	LMAppendString(value);
}

void LMAppendHashHexString(const char *key, const char *value, const unsigned char length) {
	unsigned char i;
	LMAppendRomString(key);
	LMAppendChar('i');
	if (value == 0) {
		LMAppendChar('0');
	} else {
		//Skip all prepending zeros
		for(i=0; i<length; ++i) {
			if (value[i] != '0') {
				break;
			}
		}
		for (;i<length;++i) {
			LMAppendChar(btohexa_high(value[i]));
			LMAppendChar(btohexa_low(value[i]));
		}
	}
	LMAppendChar('s');
}

void LMAppendHashInt(const char *key, unsigned long value) {
	LMAppendString(key);
	LMAppendInt(value);
}

void LMEndHash() {
	LMAppendChar('s');
}

BOOL LMNextIsInt() {
	return buffer[bufferP] == 'i';
}

unsigned long LMTakeInt() {
	long retval = 0, ch;
	if (!LMNextIsInt()) {
		return 0;
	}
	++bufferP;
	while(bufferP < sizeof(buffer)) {
		ch = buffer[bufferP++];
		if (ch == 's') {
			break;
		}
		retval <<= 4;
		retval |= hexToByte(ch);
	}
	return retval;
}

BOOL LMTakeString(BYTE* instr, BYTE count) {
	BYTE i = 0, length = 0, ch;

	//Get string length
	while(bufferP < sizeof(buffer)) {
		ch = buffer[bufferP++];
		if (ch == ':') {
			break;
		}
		length <<= 4;
		length |= hexToByte(ch);
	}
	if (length > count-1) { //-1 if for the trailing '\0'
		//Advance bufferP anyway
		bufferP += length;
		return FALSE;
	}
	for (i = 0; i < length; ++i) {
		instr[i] = buffer[bufferP++];
	}
	instr[length] = '\0';
	return TRUE;
}

BOOL LMEnterHash() {
	if (buffer[bufferP] != 'h') {
		return FALSE;
	}
	hashStartP = bufferP+1;

	return TRUE;
}

BOOL LMFindHashString(ROM BYTE *key) {
	unsigned char k[4]; //Increase this if we need to search for longer keys

	bufferP = hashStartP;
	while(bufferP < sizeof(buffer)) {
		if (!LMTakeString(&k, sizeof(k))) {
			return FALSE;
		}
		if (strcmp(key, k) == 0) {
			return TRUE;
		}
		if (LMNextIsInt()) {
			LMTakeInt();
		} else {
			LMTakeString(0,0);
		}
		if (buffer[bufferP] == 's') {
			//End of hash
			return FALSE;
		}
	}
	return FALSE;
}

void LMGenerateSignature() {
	static HASH_SUM hash;

	SHA1Initialize(&hash);
	SHA1AddData(&hash, &buffer, strlen(buffer));
	SHA1AddROMData(&hash, &PRIVATE_KEY, 32);
	SHA1Calculate(&hash, &signature);
}

BOOL validateSignature() {
	BYTE i;
	LMGenerateSignature();

	for(i=0; i < 20; ++i) {
		if (signature[i] != signature2[i]) {
			return FALSE;
		}
	}
	return TRUE;
}

void LMParseChar(const char ch) {
	static BYTE length = 0;
	BYTE i;

	if (state == SM_CLEARED) {
		LMAppendChar(ch);
		if (strncmp(&buffer, "28:", 3) == 0) {
			state = SM_PARSE_SIGNATURE;
		} else if (buffer[2] != 0) {
			buffer[0] = buffer[1];
			buffer[1] = buffer[2];
			buffer[2] = 0;
		}

	} else if (state == SM_PARSE_SIGNATURE) {
		LMAppendChar(ch);
		if (strlen(buffer) == 43) {
			for(i=3; i < 43; i+=2) {
				signature2[(i-3)/2] = (hexToByte(buffer[i]) << 4) | hexToByte(buffer[i+1]);
			}
			LMClear();
			state = SM_PARSE_LENGTH;
			length = 0;
		}

	} else if (state == SM_PARSE_LENGTH) {
		if (ch == ':') {
			LMClear();
			state = SM_PARSE_MESSAGE;
		} else {
			length <<= 4;
			length |= hexToByte(ch);
		}

	} else if (state == SM_PARSE_MESSAGE) {
		LMAppendChar(ch);
		length--;
		if (length == 0) {
			if (validateSignature()) {
				state = SM_MESSAGE_RECEIVED;
			} else {
				LMClear();
				printf("Signature mismatch\r\n");
			}
		}
	} else if (state == SM_MESSAGE_RECEIVED) {
		printf("=== WARNING ===\r\nNew bits received without handling the previous message!\r\n");
		LMClear();
		LMAppendChar(ch);
		state = SM_PARSE_SIGNATURE;
	}
}

BOOL LMHasPendingMessage() {
	return state == SM_MESSAGE_RECEIVED;
}

void LMPrintContent() {
	printf("Content %s\r\n", &buffer);
}

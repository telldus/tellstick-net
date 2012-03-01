//
// Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "config.h"

void delay_ms(int ms);
unsigned char hexToByte(unsigned char hex);

typedef struct {
	BYTE MACAddr[6];
	BYTE secret[11];
} eepromConfig;

extern eepromConfig config;

#define FIRMWARE_VERSION "X"

#ifndef DEBUG
	//Generates a lot of warnings but it seems to to the job!
	#define printf() 1
#endif

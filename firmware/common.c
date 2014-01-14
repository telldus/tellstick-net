//
// Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <htc.h>
#include "TCPIP Stack/TCPIP.h"
#include "HardwareProfile.h"

void delay_ms(int ms) {
	while(ms--) {
		__delay_ms(1);
	}
}

unsigned char hexToByte(unsigned char hex) {
	switch (hex) {
		case '0':
			return 0x0;
		case '1':
			return 0x1;
		case '2':
			return 0x2;
		case '3':
			return 0x3;
		case '4':
			return 0x4;
		case '5':
			return 0x5;
		case '6':
			return 0x6;
		case '7':
			return 0x7;
		case '8':
			return 0x8;
		case '9':
			return 0x9;
		case 'a':
		case 'A':
			return 0xA;
		case 'b':
		case 'B':
			return 0xB;
		case 'c':
		case 'C':
			return 0xC;
		case 'd':
		case 'D':
			return 0xD;
		case 'e':
		case 'E':
			return 0xE;
		case 'f':
		case 'F':
			return 0xF;
	}
	return 0x0;
}

BOOL networkIsBound() {
	if (DHCPIsBound(0)) {
		return TRUE;
	}
	if (AppConfig.Flags.bIsDHCPEnabled) {
		return FALSE;
	}
	return MACIsLinked();
}

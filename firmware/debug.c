//
// Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <htc.h>
#include "TCPIP Stack/TCPIP.h"


static char buffer[1024];
static int p = 0;

void putchflush() {
	UDP_SOCKET s;
	NODE_INFO	Remote;
	int i;

	// Set the MAC destination to be a broadcast
	memset(&Remote, 0xFF, sizeof(Remote));
	s = UDPOpen(0, &Remote, 2000);

	if(s == INVALID_UDP_SOCKET) {
		return;
	}

	if (!UDPIsPutReady(s)) {
		return;
	}
	for(i=0; i<p; ++i) {
		UDPPut(buffer[i]);
	}
	p = 0;
	UDPFlush();
	UDPClose(s);
}

void putch( char ch ) {
	if (p < sizeof(buffer)) {
		buffer[p++] = ch;
	}
}

void debugTask() {
	if (!DHCPIsBound(0)) {
		return;
	}

	if (p == 0) {
		return;
	}

	putchflush();
}


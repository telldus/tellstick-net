
#include <htc.h>
#include "TCPIP Stack/TCPIP.h"

#include "discovery.h"
#include "common.h"
#include "livemessage.h"
#include "tellsticknet.h"

#define LOCAL_ACCESS_PORT	42314

void localAccessTask() {
	static enum {
		LA_RESET = 0,
		LA_LISTEN
	} state = LA_RESET;

	static UDP_SOCKET	socket;
	BYTE ch;

	switch(state) {
		case LA_RESET:
			socket = UDPOpen(LOCAL_ACCESS_PORT, NULL, LOCAL_ACCESS_PORT);

			if(socket == INVALID_UDP_SOCKET) {
				return;
			} else {
				state=LA_LISTEN;
			}
			break;

		case LA_LISTEN:
			// Do nothing if no data is waiting
			if(!UDPIsGetReady(socket)) {
				return;
			}

			// Read all data
			LMClear();
			while(UDPIsGetReady(socket)) {
				UDPGet(&ch);
				LMAppendChar(ch);
			}
			//Discard rest just to be sure
			UDPDiscard();
			handleMessage();
			LMClear();
	}
}

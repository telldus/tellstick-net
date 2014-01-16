
#include <htc.h>
#include "TCPIP Stack/TCPIP.h"

#include "discovery.h"
#include "common.h"
#include "livemessage.h"
#include "tellsticknet.h"

#define LOCAL_ACCESS_PORT	42314

UDP_SOCKET localSocket;

void localAccessTask() {
	static enum {
		LA_RESET = 0,
		LA_LISTEN
	} state = LA_RESET;

	BYTE ch;

	switch(state) {
		case LA_RESET:
			localSocket = UDPOpen(LOCAL_ACCESS_PORT, NULL, LOCAL_ACCESS_PORT);

			if(localSocket == INVALID_UDP_SOCKET) {
				return;
			} else {
				state=LA_LISTEN;
			}
			break;

		case LA_LISTEN:
			// Do nothing if no data is waiting
			if(!UDPIsGetReady(localSocket)) {
				return;
			}

			// Read all data
			LMClear();
			while(UDPIsGetReady(localSocket)) {
				UDPGet(&ch);
				LMAppendChar(ch);
			}
			//Discard rest just to be sure
			UDPDiscard();
			handleMessage();
			LMClear();
	}
}

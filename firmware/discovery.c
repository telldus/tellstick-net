
#include <htc.h>
#include "TCPIP Stack/TCPIP.h"

#include "discovery.h"
#include "common.h"

#define DISCOVERY_PORT	30303
extern NODE_INFO remoteNode;

void discoveryTask() {
	static enum {
		DISCOVERY_HOME = 0,
		DISCOVERY_LISTEN,
		DISCOVERY_REQUEST_RECEIVED,
		DISCOVERY_DISABLED
	} DiscoverySM = DISCOVERY_HOME;

	static UDP_SOCKET	MySocket;
	BYTE i;

	switch(DiscoverySM) {
		case DISCOVERY_HOME:
			MySocket = UDPOpen(DISCOVERY_PORT, NULL, DISCOVERY_PORT);

			if(MySocket == INVALID_UDP_SOCKET) {
				return;
			} else {
				DiscoverySM++;
			}
			break;

		case DISCOVERY_LISTEN:
			// Do nothing if no data is waiting
			if(!UDPIsGetReady(MySocket)) {
				return;
			}

			// See if this is a discovery query or reply
			UDPGet(&i);
			UDPDiscard();
			if(i != 'D') {
				return;
			}

			// We received a discovery request, reply when we can
			DiscoverySM++;

			// Change the destination to the unicast address of the last received packet
			memcpy((void*)&UDPSocketInfo[MySocket].remote.remoteNode, (const void*)&remoteNode, sizeof(remoteNode));

			// No break needed.  If we get down here, we are now ready for the DISCOVERY_REQUEST_RECEIVED state

		case DISCOVERY_REQUEST_RECEIVED:
			if(!UDPIsPutReady(MySocket)) {
				return;
			}

			UDPPutROMString("TellStickNet:");
			// Convert the MAC address bytes to hex (text) and then send it
			for(i=0;i<6;++i) {
				UDPPut(btohexa_high(AppConfig.MyMACAddr.v[i]));
				UDPPut(btohexa_low(AppConfig.MyMACAddr.v[i]));
			}
			UDPPut(':');
			UDPPutString(config.secret);
			UDPPut(':');
			UDPPutROMString(FIRMWARE_VERSION);

			// Send the packet
			UDPFlush();

			// Listen for other discovery requests
			DiscoverySM = DISCOVERY_LISTEN;
			break;

		case DISCOVERY_DISABLED:
			break;
	}
}

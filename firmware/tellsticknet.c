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
#include "eeprom.h"
#include "livemessage.h"
#include "localaccess.h"
#include "pwm.h"
#include "send.h"
#include "transmit.h"
#include "transmit_arctech.h"

static UDP_PORT localPort;
static UDP_SOCKET s = INVALID_UDP_SOCKET;
static NODE_INFO	remote;
static int listenerActive = 0;

void registerListener() {
	memcpy(&remote, &UDPSocketInfo[localSocket].remote, sizeof(remote));
	localPort = UDPSocketInfo[localSocket].remotePort;

	listenerActive = 1;
	
	if(s != INVALID_UDP_SOCKET) {
		UDPClose(s);
		s = INVALID_UDP_SOCKET;
		return;
	}
}

void sendToLocalListeners() {
	int i, len;
	char *b;

	/* Only transmit if at least one listener has registered */
	if (!listenerActive) return;
	
	if(s == INVALID_UDP_SOCKET) {
		s = UDPOpen(localPort, &remote, localPort);
	}
	if(s == INVALID_UDP_SOCKET) {
		return;
	}

	if (!UDPIsPutReady(s)) {
		return;
	}
	b = LMContent();
	len = strlen(b);
	for(i=0; i<len; ++i) {
		UDPPut(b[i]);
	}
	UDPFlush();
}

void send() {
	unsigned char pause = DEFAULT_PAUSE;
	unsigned char repeats = DEFAULT_REPEATS;
	unsigned char i, j;
	unsigned long ack;
	BYTE protocol[20] = "", model[20] = "";

	if (!LMEnterHash()) {
		return;
	}

	if (LMFindHashString("P")) {
		pause = LMTakeInt();
	}
	if (LMFindHashString("R")) {
		repeats = LMTakeInt();
	}

	if (LMFindHashString("protocol")) {
		LMTakeString(&protocol, sizeof(protocol));

		if (LMFindHashString("model")) {
			LMTakeString(&model, sizeof(model));
		}
		if (strcmp(protocol, "arctech") != 0 || strcmp(model, "selflearning") != 0) {
			return;
		}
		sendArctechSelflearning();
		return;
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

void saveIp() {
	if (AppConfig.Flags.bIsDHCPEnabled) {
		config.ipAddr.Val = 0xFFFFFFFF;
		config.netmask.Val = 0xFFFFFFFF;
		config.gateway.Val = 0xFFFFFFFF;
		config.dns1.Val = 0xFFFFFFFF;
		config.dns2.Val = 0xFFFFFFFF;
	} else {
		config.ipAddr.Val = AppConfig.MyIPAddr.Val;
		config.netmask.Val = AppConfig.MyMask.Val;
		config.gateway.Val = AppConfig.MyGateway.Val;
		config.dns1.Val = AppConfig.PrimaryDNSServer.Val;
		config.dns2.Val = AppConfig.SecondaryDNSServer.Val;
	}
	saveEEPROM((unsigned char*)&config, sizeof(config));
	Reset();
}

void setIp() {
	IP_ADDR ip, netmask, gateway, dns1, dns2;

	if (!LMEnterHash()) {
		return;
	}
	if (!LMFindHashString("ip")) {
		return;
	}
	ip.Val = LMTakeInt();
	if (ip.Val == 0xFFFFFFFF) {
		AppConfig.Flags.bIsDHCPEnabled = TRUE;
		AppConfig.Flags.bInConfigMode = TRUE;
		DHCPEnable(0);
		return;
	}

	if (!LMFindHashString("netmask")) {
		return;
	}
	netmask.Val = LMTakeInt();

	if (!LMFindHashString("gateway")) {
		return;
	}
	gateway.Val = LMTakeInt();

	if (!LMFindHashString("dns1")) {
		return;
	}
	dns1.Val = LMTakeInt();

	if (!LMFindHashString("dns2")) {
		return;
	}
	dns2.Val = LMTakeInt();

	AppConfig.Flags.bIsDHCPEnabled = FALSE;
	AppConfig.MyIPAddr.Val = ip.Val;
	AppConfig.MyMask.Val = netmask.Val;
	AppConfig.MyGateway.Val = gateway.Val;
	AppConfig.PrimaryDNSServer.Val = dns1.Val;
	AppConfig.SecondaryDNSServer.Val = dns2.Val;
}

void handleMessage() {
	BYTE name[20] = "";

	if (!LMTakeString(&name, sizeof(name))) {
		printf("Could not handle message (to long?)\r\n");
		return;
	}
	if (strcmp(name, "reglistener") == 0) {
		registerListener();
	} else if (strcmp(name, "send") == 0) {
			send();
	} else if (strcmp(name, "setip") == 0) {
		setIp();
	} else if (strcmp(name, "saveip") == 0) {
		saveIp();
	} else if (strcmp(name, "disconnect") == 0) {
		Reset();
	}
}

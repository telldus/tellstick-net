//
// Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
//
// Copyright: See COPYING file that comes with this distribution
//
//

#define THIS_IS_STACK_APPLICATION
#include "TCPIP Stack/Tick.h"

#include <htc.h>
#include "common.h"
#include "config.h"
#include "debug.h"
#include "discovery.h"
#include "eeprom.h"
#include "localaccess.h"
#include "pwm.h"
#include "receive.h"
#include "transmit.h"
#include "usart.h"

#include "rf.h"

__CONFIG(1, WDTDIS & XINSTDIS);
__CONFIG(2, HSPLL);

APP_CONFIG AppConfig;
eepromConfig config;

void interrupt low_priority LowISR(void) {
	if(INTCONbits.TMR0IF) {
		TickUpdate();
	}
	if (TMR2IF) {
		pwmUpdate();
		TMR2IF = 0;
	}
}

void interrupt HighISR(void) {
	if (TMR4IF) {
		TMR4 = 0xE2;
		rfReceiveUpdate(RECEIVER);

		//Reset timer
		TMR4IF=0;
	}
	if (TMR3IF) {
		TMR3H = 0xFF;
		TMR3L = 0xC4;
		rfTransmitUpdate();
		TMR3IF = 0;
	}
}

void initInterrupts() {
	//Enable interrupt priority
	RCONbits.IPEN = 1;

	//Enable interrupts
	GIEH = 1; //High priority
	GIEL = 1; //Low priority
}

void initWDT() {
	SWDTEN = 1;
}

static void initAppConfig(void) {
	initEEPROM();
	loadEEPROM((unsigned char*)&config, sizeof(config));

	if (config.MACAddr[0] == 0xFF) {
		//Empty EEProm, fallback MAC
		config.MACAddr[5] = 0x00;
		config.MACAddr[4] = 0x00;
		config.MACAddr[3] = 0x00;
		config.MACAddr[2] = 0x54;
		config.MACAddr[1] = 0xCA;
		config.MACAddr[0] = 0xAC;
	}

	AppConfig.Flags.bIsDHCPEnabled = TRUE;
	AppConfig.Flags.bInConfigMode = TRUE;
	memcpy((void*)&AppConfig.MyMACAddr, (ROM void*)config.MACAddr, sizeof(AppConfig.MyMACAddr));
	AppConfig.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
	AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
	AppConfig.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | MY_DEFAULT_MASK_BYTE4<<24ul;
	AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
	AppConfig.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2<<8ul | MY_DEFAULT_GATE_BYTE3<<16ul | MY_DEFAULT_GATE_BYTE4<<24ul;
	AppConfig.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2<<8ul  | MY_DEFAULT_PRIMARY_DNS_BYTE3<<16ul  | MY_DEFAULT_PRIMARY_DNS_BYTE4<<24ul;
	AppConfig.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2<<8ul  | MY_DEFAULT_SECONDARY_DNS_BYTE3<<16ul  | MY_DEFAULT_SECONDARY_DNS_BYTE4<<24ul;
}

int main() {
	DWORD t = 0;

	//Needed because a bug in picc-18?
	MIWRL=0;
	MIWRH=0;

	TRISA = 0;
	TRISB = 0;
	TRISC = 0;
	TRISD = 0;
	TRISE = 0;
	TRISF = 0;
	TRISG = 0;
	PORTA=0;
	PORTB=0;
	PORTC=0;
	PORTD=0;
	PORTE=0;
	PORTF=0;

	OSCTUNE = 0x40; //Speed up to 41.67 MHz

	//Turn off AD
	ADCON1 = 0x0F;

#if defined(DEBUG) && defined(_18F87J60)
	initUsart2();
#endif

	initTransmitter();
	initReceiver();
	initPwm();
	initInterrupts();
	initAppConfig();
	initWDT();
	TickInit();
	StackInit();
	//Set the LED on the connector
	SetLEDConfig(0x3742); //See MAC.h for the values

	printf("Telldus TellStick Net v%s\r\n", FIRMWARE_VERSION);

	while(1) {
		StackTask();
		StackApplications();
		if(TickGet() - t >= TICK_SECOND) {
			t = TickGet();
		}
#if defined(DEBUG)
		debugTask();
#endif
		rfReceiveTask();
		discoveryTask();
		localAccessTask();
		ClrWdt();
	}
	return 42;
}

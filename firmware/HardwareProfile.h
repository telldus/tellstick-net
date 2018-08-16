//
// Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef HARDWAREPROFILE_H
#define HARDWAREPROFILE_H

//#define GetSystemClock()		(25000000ul)      // Hz
#define GetSystemClock()		(41666667ul)      // Hz
#define GetInstructionClock()	(GetSystemClock()/4)
#define GetPeripheralClock()	GetInstructionClock()

#define _XTAL_FREQ GetSystemClock()

//Set these to activate RX polarity swap
#define ETH_RX_POLARITY_SWAP_TRIS (TRISE1)
#define ETH_RX_POLARITY_SWAP_IO (LATE1)

#define LED0_TRIS			(TRISC1)
#define LED0_IO				(LATC1)
#define LED1_TRIS			(TRISC2)
#define LED1_IO				(LATC2)
#define LED2_TRIS			(TRISC5)
#define LED2_IO				(LATC5)

#define LED_BLUE      (LED0_IO)
#define LED_RED       (LED1_IO)
#define DEBUG_IO      (LED2_IO)

#define SENDER_TRIS      (TRISA4)
#define SENDER           (LATA4)
#define RECEIVER_TRIS    (TRISB4)
#define RECEIVER         (RB4)

#define TX_ANT_TRIS      (TRISF1)
#define TX_ANT           (LATF1)
#define RX_ANT_TRIS      (TRISF2)
#define RX_ANT           (LATF2)

#define DATA_BYTES       512

//Timeout after 3,156ms
#define TMR_RELOAD_H     0x80
#define TMR_RELOAD_L     0x00
#define TMR_RELOAD       0x8000

#endif //HARDWAREPROFILE_H

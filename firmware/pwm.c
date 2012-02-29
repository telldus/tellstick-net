
#include <htc.h>
#include "TCPIP Stack/TCPIP.h"
#include "pwm.h"

//LED types
typedef enum {
	PWM = 0,
	BLINK,
	FIXED,
} LED_TYPES;


//PWM variables
struct pwms {
	char duty;
	char fixedDuty;
	signed char dir;
	char pulses;
	LED_TYPES type;
	DWORD blinkTimer;
} txd  = {0,0,1,0,PWM,0},
	rxd  = {0,0,1,0,PWM,0};

void initPwm() {
	TMR2IF = 0;
	PR2 = 0xFF;

	T2CKPS1 = 1; //Prescaler: 1:16

	//Postscaler, does not affect PWM
	T2OUTPS3 = 0;
	T2OUTPS2 = 1;
	T2OUTPS1 = 1;
	T2OUTPS0 = 1;

	//Low priority interrupt
	TMR2IP = 0;
	TMR2IE = 1;
	TMR2ON = 1; // Turn on timer

	CCP1CON = 0;
	CCP2CON = 0;

	//Pwm mode
	//Single output: P2A modulated
	CCP2M3 = 1;
	CCP2M2 = 1;
	P2M1 = 0;
	P2M0 = 0;

	//Single output: P1A modulated
	CCP1M3 = 1;
	CCP1M2 = 1;
	P1M1 = 0;
	P1M0 = 0;

	CCP1M3 = 1;
	CCP1M2 = 1;
	CCP2M3 = 1;
	CCP2M2 = 1;

// 	//We skip the lsb so setting them low
	DC1B1 = 0;
	DC1B0 = 0;
	DC2B1 = 0;
	DC2B0 = 0;

// 	//Turn off the led
	CCPR1L=0x0;
	CCPR2L=0x0;
}

void pwmUpdate() {
	if (rxd.pulses) {
		rxd.duty += rxd.dir;
		if (rxd.duty >= 0x7F) {
			rxd.dir = -1;
		} else if (rxd.duty == 0) {
			rxd.dir = 1;
			--rxd.pulses;
		}
		CCPR2L = rxd.duty;
	} else if (rxd.type == FIXED) {
		CCPR2L = rxd.fixedDuty;
	} else if (rxd.type == BLINK) {
		if (TickGet()-rxd.blinkTimer > TICK_SECOND) {
			rxd.fixedDuty = 0xFF - rxd.fixedDuty;
			CCPR2L = rxd.fixedDuty;
			rxd.blinkTimer = TickGet();
		}
	} else {
		CCPR2L = 0x0;
	}


	if (txd.pulses) {
		txd.duty += txd.dir;
		if (txd.duty >= 0x7F) {
			txd.dir = -1;
		} else if (txd.duty == 0) {
			txd.dir = 1;
			--txd.pulses;
		}
		CCPR1L = txd.duty;
	} else if (txd.type == FIXED) {
		CCPR1L = txd.fixedDuty;
	} else {
		CCPR1L = 0x0;
	}
}

void setRXBlink() {
	rxd.type = BLINK;
	rxd.fixedDuty = 0xFF;
	CCPR2L = 0xFF;
	rxd.blinkTimer = TickGet();
}

void setRXPulses(unsigned char count) {
	rxd.pulses = count;
}

void setTXPulses(unsigned char count) {
	txd.pulses = count;
}

void setRXFixed(char level) {
	rxd.type = FIXED;
	rxd.fixedDuty = level;
}

void setTXFixed(char level) {
	txd.type = FIXED;
	txd.fixedDuty = level;
}

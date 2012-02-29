
#include <htc.h>
#include "config.h"
#include "usart.h"

void putch( char ch ) {
#if defined(DEBUG) && defined(_18F87J60)
	usart2PutByte(ch);
#endif
}

void usartPutByte( char ch ) {
	while (!TXSTA1bits.TRMT);
	TXREG = ch;
}


char usartReady() {
	return PIR1bits.RCIF;
}

unsigned char usartGetByte() {
	unsigned char rslt = RCREG;

	if (RCSTA1bits.OERR) {
		RCSTA1bits.CREN = 0;
		RCSTA1bits.CREN = 1;
	}
	return rslt;
}

void initUsart() {
	unsigned short i;

	RCSTA1 = 0;
	RCSTA1bits.SPEN = 1;  //Enable Usart
	RCSTA1bits.CREN = 1;  //Enable reciever

	TXSTA1bits.TX9D = 0;  //8-bit transmission
	TXSTA1bits.TXEN = 1;  //Transmit enabled
	TXSTA1bits.BRGH = 0;  //No High speed

	TRISC7 = 1;
	TRISC6 = 0;

	while (PIR1bits.RCIF) {
		i = RCREG;
	}

	SPBRG = 67;
}

void initUsart2() {
	unsigned short i;

	RCSTA2 = 0;
	RCSTA2bits.SPEN2 = 1;  //Enable Usart2
	RCSTA2bits.CREN2 = 1;  //Enable reciever

	TXSTA2bits.TX9D2 = 0;  //8-bit transmission
	TXSTA2bits.TXEN2 = 1;  //Transmit enabled
	TXSTA2bits.BRGH2 = 0;  //No High speed

	TRISG2 = 1;
	TRISG1 = 0;

	while (PIR3bits.RC2IF) {
		i = RCREG2;
	}

	SPBRG2 = 67;
}

void usart2PutByte( char ch ) {
	while (!TXSTA2bits.TRMT2);
	TXREG2 = ch;
}

char usart2Ready() {
	return PIR3bits.RC2IF;
}

unsigned char usart2GetByte() {
	unsigned char rslt = RCREG2;

	if (RCSTA2bits.OERR2) {
		RCSTA2bits.CREN2 = 0;
		RCSTA2bits.CREN2 = 1;
	}
	return rslt;
}

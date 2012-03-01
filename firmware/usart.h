//
// Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
//
// Copyright: See COPYING file that comes with this distribution
//
//

void putch( char ch );

void initUsart();
void initUsart2();

char usartReady();
char usart2Ready();

unsigned char usartGetByte();
unsigned char usart2GetByte();

void usartPutByte( char ch );
void usart2PutByte( char ch );

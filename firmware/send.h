//
// Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
//
// Copyright: See COPYING file that comes with this distribution
//
//

extern unsigned char sendPacket[200];

#define DEFAULT_PAUSE    11
#define DEFAULT_REPEATS  10

void initSender();
void sendCommand();
void sendExtendedCommand();

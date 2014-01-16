//
// Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
//
// Copyright: See COPYING file that comes with this distribution
//
//

void LMAppendChar(const char ch);
void LMClear();

void LMAppendString(const char *string);
void LMAppendRomString(ROM BYTE *string);
void LMAppendInt(unsigned long value);

void LMStartHash();
void LMAppendHashString(ROM BYTE *key, const char *value);
void LMAppendHashHexString(const char *key, const char *value, const unsigned char length);
void LMAppendHashInt(ROM BYTE *key, unsigned long value);
void LMEndHash();

BOOL LMNextIsInt();
unsigned long LMTakeInt();
BOOL LMTakeString(BYTE* buffer, BYTE count);
BOOL LMEnterHash();
BOOL LMFindHashString(ROM BYTE *key);

void LMParseChar(const char ch);

BOOL LMHasPendingMessage();

void LMSendToSocket();

char *LMContent();
void LMPrintContent();

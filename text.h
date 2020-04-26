#include <stddef.h>
//#include <stdio.h>
#include <stdlib.h> 

#ifndef TEXT_H
#define TEXT_H

#include "../foenixLibrary/mytypes.h"

extern char text_alphabet[3][26];
extern byte text_version;   
extern ushort text_abbreviationtableAddress;
//extern byte *zorkData;

void text_initialize(byte version, ushort abbreviationtableAddress);
char * readText(ushort address, byte length);
ushort text_printLiteral(ushort startAddress);

#endif /* TEXT_H */
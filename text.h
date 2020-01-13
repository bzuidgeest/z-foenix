#include <stddef.h>
//#include <stdio.h>
#include <stdlib.h> 

typedef unsigned short ushort;
typedef unsigned char byte;

extern char text_alphabet[3][26];
extern byte text_version;   
extern ushort text_abbreviationtableAddress;
//extern byte *zorkData;

void text_initialize(byte version, ushort abbreviationtableAddress);
char * readText(ushort address, byte length);
ushort text_printLiteral(ushort startAddress);

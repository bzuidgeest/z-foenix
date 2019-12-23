#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include "zip.h"

extern char text_alphabet[3][26];
extern byte text_version;   
extern ushort text_abbreviationtableAddress;

void text_initialize(byte version, ushort abbreviationtableAddress);
char * readText(ushort address, byte length);
ushort text_printLiteral(ushort startAddress);
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include "text.h"
#include "zip.h"

/*
3.1
Z-machine text is a sequence of ZSCII character codes (ZSCII is a system similar to ASCII: see S 3.8 below). These ZSCII values are encoded into memory 
using a string of Z-characters. The process of converting between Z-characters and ZSCII values is given in SS 3.2 to 3.7 below.

3.2
Text in memory consists of a sequence of 2-byte words. Each word is divided into three 5-bit 'Z-characters', plus 1 bit left over, arranged as

   --first byte-------   --second byte---
   7    6 5 4 3 2  1 0   7 6 5  4 3 2 1 0
   bit  --first--  --second---  --third--

The bit is set only on the last 2-byte word of the text, and so marks the end. 
*/
char * readText(ushort address, byte length)
{
    char output[length * 3];
    ushort enc;
    int i;

    // zero out text with "spaces"
    memset(output, 0, length * 3);

    if (length % 2 != 0)
    {
        printf("Length values for text cannot be an odd value.");
        exit(1);
    }

    for (i = 0; i < length; i = i + 2)
    {
        enc = (zorkData[i] << 8) + zorkData[i + 1];
        output[(i * 3)] = (enc & 0x7C00) >> 10;
        output[(i * 3) + 1] = (enc & 0x03E0) >> 5;
        output[(i * 3) + 2] = (enc & 0x1F);

        if ((zorkData[i] >> 8) == 1)
            break;  
    }

    return output;
}

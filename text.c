//#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
//#include <sys/stat.h>
#include <sys/types.h>
#include "text.h"
#include "zip.h"
#include "data.h"

char text_alphabet[3][26] = {
    { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' }, 
    { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' }, 
    { ' ', '\n', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', ',', '!', '?', '_', '#', '\'', '"', '/', '\\', '-', ':', '(', ')' }
};

byte text_version = 0;
ushort text_abbreviationtableAddress = 0;

void text_initialize(byte version, ushort abbreviationtableAddress)
{
    byte i = 0;
    text_version = version;
    text_abbreviationtableAddress = abbreviationtableAddress;

    if (version == 1)
    {
        // adjust alphabet table for version 1
        for (i = 2; i < 22; i++)
        {
            text_alphabet[2][i - 1] = text_alphabet[2][i];
        }
        text_alphabet[2][21] = '<';
    }
}

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
/*
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
*/
ushort text_printLiteral(ushort startAddress)
{
    short encodedText = 0;
    char text[3];
    byte counter;
    byte shiftLock = 0;
    byte currentAlphabet = 0;
    byte printAbbreviation = 0;

    ushort byteCounter = 0;

    // loop while end bit is not send
    while ((encodedText >> 16) == 0)
    {
        // read next two bytes encoding next three characters.
        encodedText = data_loadWord(startAddress);
        startAddress += 2;
        byteCounter += 2;

        text[0] = (encodedText & 0x7C00) >> 10;
        text[1] = (encodedText & 0x03E0) >> 5;
        text[2] = (encodedText & 0x1F);

        for (counter = 0; counter < 3; counter++)
        {
            if (printAbbreviation >= 1)
            {
                // The formula in the spec is wrong. It does not account for the fact dat the abbreviationtable has 2 bytes for every address
                ushort index = text_abbreviationtableAddress + (64 * (printAbbreviation - 1) + 2 * text[counter]);
                ushort waddress = data_loadWord(index);
                //ushort waddress = (zorkData[text_abbreviationtableAddress + text[counter]] << 8) + zorkData[text_abbreviationtableAddress + text[counter] + 1];
                text_printLiteral(waddress << 1);

                printAbbreviation = 0;
                continue;
            }

            if (text[counter] > 5)
            {
                printf("%c", text_alphabet[currentAlphabet][text[counter] - 6]);

                if (currentAlphabet > 0 && shiftLock == 0)
                {
                    currentAlphabet = 0;
                }
            }
            else // special characters
            {
                switch(text[counter])
                {
                    case 0:
                        printf(" ");
                        break;
                    case 1:
                        if (text_version != 1)
                        {
                            printAbbreviation = text[counter];
                        }
                        else
                            printf("\n");
                        break;
                    case 2:
                        if (text_version >= 3)
                        {
                            printAbbreviation = text[counter];
                        }
                        else
                        {
                            switch(currentAlphabet)
                            {
                                case 0:
                                    currentAlphabet = 1;
                                    break;
                                case 1:
                                    currentAlphabet = 2;
                                    break;
                                case 2:
                                    currentAlphabet = 0;
                                    break;   
                            }
                        }
                        break;
                    case 3:
                        if (text_version >= 3)
                        {
                            printAbbreviation = text[counter];
                        }
                        else
                        {
                            switch(currentAlphabet)
                            {
                                case 0:
                                    currentAlphabet = 2;
                                    break;
                                case 1:
                                    currentAlphabet = 0;
                                    break;
                                case 2:
                                    currentAlphabet = 1;
                                    break;   
                            }
                        }
                        break;
                    case 4:
                        if (text_version < 3)
                            shiftLock = 1;
                        switch(currentAlphabet)
                        {
                            case 0:
                                currentAlphabet = 1;
                                break;
                            case 1:
                                currentAlphabet = 2;
                                break;
                            case 2:
                                currentAlphabet = 0;
                                break;   
                        }
                        break;
                    case 5:
                        if (text_version < 3)
                            shiftLock = 1;
                        switch(currentAlphabet)
                        {
                            case 0:
                                currentAlphabet = 2;
                                break;
                            case 1:
                                currentAlphabet = 0;
                                break;
                            case 2:
                                currentAlphabet = 1;
                                break;   
                        }
                        break;
                    default:
                        printf("special: %d\n", text[counter]);
                        break;
                }
            }
        }
        
    }

    return byteCounter;
}


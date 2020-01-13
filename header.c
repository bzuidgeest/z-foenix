#include <stddef.h>
#include <stdio.h>
#include <stdlib.h> 
#include "header.h"

struct header* zorkHeader;

void header_initialise(char *filename)
{
    FILE *zFile;
    zorkHeader = malloc(sizeof(*zorkHeader));

    zFile = fopen(filename, "rb");
    
    /* make sure we are at the start of the file */
    fseek(zFile, 0, SEEK_SET);
    
    /* read data archive into array */
    int bytesRead = fread(zorkHeader, sizeof(struct header), 1, zFile);
    
    if (bytesRead == 1)
        printf("File read succesfully");
    else
    {
        printf("I could only read %d bytes.", bytesRead);
        exit(1);
    }

    fclose(zFile);
}

ushort getRelease()
{
    return SWAP16(zorkHeader->release);
}

ushort getInitialProgramCounter()
{
    return SWAP16(zorkHeader->programCounterInitial);
}

ushort getDictionaryLocation()
{
    return SWAP16(zorkHeader->dictionaryLocation);
}

ushort getObjectTableLocation()
{
    return SWAP16(zorkHeader->objectTableLocation);
}

ushort getGlobalVariableLocation()
{
    return SWAP16(zorkHeader->globalVariableLocation);
}

ushort getStaticMemoryLocation()
{
    return SWAP16(zorkHeader->staticMemoryLocation);
}

ushort getHighMemoryStart()
{
    return SWAP16(zorkHeader->highMemoryStart);
}

ushort getAbbreviationsLocation()
{
    return SWAP16(zorkHeader->abbreviationsLocation);
}

ushort getAlphabetTableAddress()
{
    return SWAP16(zorkHeader->alphabetTableAddress);
}

ushort getRoutineOffset()
{
    // spec 11.1.0
    return SWAP16(zorkHeader->routineOffset) * 8;
}

ushort getStringOffset()
{
    // spec 11.1.0
    return SWAP16(zorkHeader->stringOffset) * 8;
}        

#include <stddef.h>
//#include <stdio.h>
#include <stdlib.h> 
#include "header.h"

struct header *zorkHeader;

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

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include "header.h"


u_int16_t getRelease()
{
    return SWAP16(zorkHeader->release);
}

u_int16_t getInitialProgramCounter()
{
    return SWAP16(zorkHeader->programCounterInitial);
}

u_int16_t getDictionaryLocation()
{
    return SWAP16(zorkHeader->dictionaryLocation);
}

u_int16_t getObjectTableLocation()
{
    return SWAP16(zorkHeader->objectTableLocation);
}

u_int16_t getGlobalVariableLocation()
{
    return SWAP16(zorkHeader->globalVariableLocation);
}

u_int16_t getStaticMemoryLocation()
{
    return SWAP16(zorkHeader->staticMemoryLocation);
}

u_int16_t getHighMemoryStart()
{
    return SWAP16(zorkHeader->highMemoryStart);
}

u_int16_t getAbbreviationsLocation()
{
    return SWAP16(zorkHeader->abbreviationsLocation);
}

u_int16_t getAlphabetTableAddress()
{
    return SWAP16(zorkHeader->alphabetTableAddress);
}

u_int16_t getRoutineOffset()
{
    // spec 11.1.0
    return SWAP16(zorkHeader->routineOffset) * 8;
}

u_int16_t getStringOffset()
{
    // spec 11.1.0
    return SWAP16(zorkHeader->stringOffset) * 8;
}        
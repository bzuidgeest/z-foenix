#include <stddef.h>
#include <stdio.h>
#include <stdlib.h> 
#include "header.h"
#include "../foenixLibrary/fatfs/ff.h"

struct header* zorkHeader;

extern FATFS FatFs;	/* Work area (filesystem object) for logical drive */
FILE headerFile;

void header_initialise(char *filename)
{
	FRESULT fr = 0; 
	UINT bytesRead = 0;
	ushort temp = 0;
    
	zorkHeader = malloc(sizeof(*zorkHeader));

 	fr = f_mount(&FatFs, "1:", 0);
    fr = f_open(&headerFile, filename, FA_READ);
    if (fr)
	{
		printf("Error %d opening header\n", (int)fr);
	} 

	/* make sure we are at the start of the file */
	f_lseek(&headerFile, 0);
    
    /* read data archive into array */
    f_read(&headerFile, zorkHeader, sizeof(struct header), &bytesRead);

    if (bytesRead == sizeof(struct header))
	{
        printf("File read succesfully\n");
	}
    else
    {
        printf("Iz could only read %d of %d bytes.\n", bytesRead, sizeof(struct header));
        exit(1);
    }

    f_close(&headerFile);
	f_mount(0, "1:", 0);
	
	return;
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

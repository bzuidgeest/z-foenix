#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/stat.h>
#include <sys/types.h>

#define SWAP16(x) (x>>8) | (x<<8)

struct header
{
    u_int8_t version;
    u_int8_t flags1;
    u_int16_t release; 
    u_int16_t highMemoryStart;
    u_int16_t programCounterInitial;
    u_int16_t dictionaryLocation;
    u_int16_t objectTableLocation;
    u_int16_t globalVariableLocation;
    u_int16_t staticMemoryLocation;
    u_int16_t flags2; /* spec says byte but frotz has word, posibly for easier parsing, also need fille byte before serial */
    u_int8_t serial[6]; 
    u_int16_t abbreviationsLocation;
    u_int16_t fileLength;
    u_int16_t fileChecksum;
    u_int8_t interpreterNumber;
    u_int8_t interpreterVersion;
    u_int8_t screenHeightLines;
    u_int8_t screenWidthChars;
    u_int16_t screenWidthUnits;
    u_int16_t screenheightUnits;
    u_int8_t fontHeight;
    u_int8_t fontWidth;
    u_int16_t routineOffset;
    u_int16_t stringOffset;
    u_int8_t defaultBackgroundColor;
    u_int8_t defaultForegroundColor;
    u_int16_t terminatingCharAddress;
    u_int16_t outputStream3pixelWidth;
    u_int16_t revisionNumber;
    u_int16_t alphabetTableAddress;
    u_int16_t headerExtensionTableAddress;
    u_int8_t username[8];

};

struct header *zorkHeader;
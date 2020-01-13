#define SWAP16(x) (x>>8) | (x<<8)

typedef unsigned short ushort;
typedef unsigned char byte;

struct header
{
    byte version;
    byte flags1;
    unsigned short release; 
    unsigned short highMemoryStart;
    unsigned short programCounterInitial;
    unsigned short dictionaryLocation;
    unsigned short objectTableLocation;
    unsigned short globalVariableLocation;
    unsigned short staticMemoryLocation;
    unsigned short flags2; /* spec says byte but frotz has word, posibly for easier parsing, also need fille byte before serial */
    byte serial[6]; 
    unsigned short abbreviationsLocation;
    unsigned short fileLength;
    unsigned short fileChecksum;
    byte interpreterNumber;
    byte interpreterVersion;
    byte screenHeightLines;
    byte screenWidthChars;
    unsigned short screenWidthUnits;
    unsigned short screenheightUnits;
    byte fontHeight;
    byte fontWidth;
    unsigned short routineOffset;
    unsigned short stringOffset;
    byte defaultBackgroundColor;
    byte defaultForegroundColor;
    unsigned short terminatingCharAddress;
    unsigned short outputStream3pixelWidth;
    unsigned short revisionNumber;
    unsigned short alphabetTableAddress;
    unsigned short headerExtensionTableAddress;
    byte username[8];

};

//extern struct header *zorkHeader;

void header_initialise(char *filename);

ushort getRelease(void);
ushort getInitialProgramCounter(void);
ushort getDictionaryLocation(void);
ushort getObjectTableLocation(void);
ushort getGlobalVariableLocation(void);
ushort getStaticMemoryLocation(void);
ushort getHighMemoryStart(void);
ushort getAbbreviationsLocation(void);
ushort getAlphabetTableAddress(void);
ushort getRoutineOffset(void);
ushort getStringOffset(void);

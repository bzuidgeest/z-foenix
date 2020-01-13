

typedef unsigned short ushort;
typedef unsigned char byte;

extern byte *zData;

void data_initialise(ushort highmemAddress, char *filename);
void data_close();

short data_loadWord(ushort address);
void data_saveWord(ushort address, short value);

byte data_loadByte(ushort address);
void data_saveByte(ushort address, byte value);

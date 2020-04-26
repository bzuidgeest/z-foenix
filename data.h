
#ifndef DATA_H
#define DATA_H

#include "../foenixLibrary/mytypes.h"
#include "../foenixLibrary/fatfs/ff.h"

extern BYTE *zData;

void data_initialise(ushort highmemAddress, char *filename);
void data_close();

short data_loadWord(ushort address);
void data_saveWord(ushort address, short value);

BYTE data_loadByte(ushort address);
void data_saveByte(ushort address, BYTE value);

#endif /* DATA_H */
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/stat.h>
#include <sys/types.h>



ushort objecttableAddress;
byte objecttableVersion;
ushort objectlistStart;

void objecttable_initialize(ushort address, byte version);
short objecttable_getDefaultProperty(short number);
ushort objecttable_getObjectAddress(ushort number);

ushort objecttable_getObjectParent(ushort objectAddress);
ushort objecttable_getObjectChild(ushort objectAddress);
ushort objecttable_getObjectSibbling(ushort objectAddress);
ushort objecttable_getObjectPropertiesAddress(ushort objectAddress);
byte objecttable_getObjectAttribute(ushort objectAddress, ushort attributeNumber);
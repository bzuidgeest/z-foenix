#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include "zip.h"


ushort objecttableAddress;
byte objecttableVersion;
ushort objectlistStart;

void objecttable_initialize(ushort address, byte version);
short objecttable_getDefaultProperty(short number);
ushort objecttable_getObjectAddress(ushort number);

ushort objecttable_getObjectParent(ushort objectAddress);
ushort objecttable_getObjectChild(ushort objectAddress);
ushort objecttable_getObjectSibbling(ushort objectAddress);
ushort objecttable_getObjectPropertyTableAddress(ushort objectAddress);
byte objecttable_getObjectAttribute(ushort objectAddress, ushort attributeNumber);



char * objecttable_getObjectName(ushort propertyTableAddress);
ushort objecttable_getFirstPropertyAddress(ushort objectAddress);
ushort objecttable_getNextPropertyAddress(ushort propertyAddress);
void objecttable_insertObject(ushort sourceObjectNumber, ushort destinationObjectNumber);

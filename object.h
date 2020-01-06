#include <stddef.h>
#include <stdio.h>
#include <stdlib.h> 

typedef unsigned short ushort;
typedef unsigned char byte;

ushort objecttableAddress;
byte objecttableVersion;
ushort objectlistStart;

void objecttable_initialize(ushort address, byte version);
short objecttable_getDefaultProperty(short number);
ushort objecttable_getObjectAddress(ushort number);

ushort objecttable_getObjectParent(ushort objectNumber);
void objecttable_setObjectParent(ushort objectAddress, ushort value);
ushort objecttable_getObjectChild(ushort objectNumber);
void objecttable_setObjectChild(ushort objectAddress, ushort value);
ushort objecttable_getObjectSibling(ushort objectNumber);
void objecttable_setObjectSibling(ushort objectAddress, ushort value);
ushort objecttable_getObjectPropertyTableAddress(ushort objectNumber);
byte objecttable_getObjectAttribute(ushort objectNumber, ushort attributeNumber);
void objecttable_setObjectAttribute(ushort objectNumber, ushort attributeNumber);



char * objecttable_getObjectName(ushort objectNumber);
ushort objecttable_getObjectNameAddress(ushort objectNumber);
ushort objecttable_getFirstPropertyAddress(ushort objectAddress);
ushort objecttable_getNextPropertyAddress(ushort propertyAddress);
void objecttable_insertObject(ushort sourceObjectNumber, ushort destinationObjectNumber);
short objecttable_getPropertyShort(ushort objectNumber, ushort propertyNumber);

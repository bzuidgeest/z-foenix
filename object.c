#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include "object.h"
#include "zip.h"


/*
12.1
The object table is held in dynamic memory and its byte address is stored in the word at $0a in the header. (Recall that objects have flags 
attached called attributes, numbered from 0 upward, and variables attached called properties, numbered from 1 upward. An object need not provide 
every property.)

*/
void objecttable_initialize(ushort address, byte version)
{
    objecttableAddress = address;
    objecttableVersion = version;

    if (version < 4)
    {
        objectlistStart = address * 31 * 2;
    }
    else
    {
        objectlistStart = address * 63 * 2;
    }
    
}

/*
12.2
The table begins with a block known as the property defaults table. This contains 31 words in Versions 1 to 3 and 63 in Versions 4 and later. 
When the game attempts to read the value of property n for an object which does not provide property n, the n-th entry in this table is the 
resulting value.
*/
short objecttable_getDefaultProperty(short number)
{
    if ((objecttableVersion < 4 && number < 31) || (objecttableVersion >= 4 && number < 63))
    {
        return (zorkData[objecttableAddress + number] << 8) + zorkData[objecttableAddress + number + 1];
    }
    else
    {
        printf("Invalid property: %d for version %d", number, objecttableAddress);
        exit(1);
    }
}

/*
12.3
Next is the object tree. Objects are numbered consecutively from 1 upward, with object number 0 being used to mean "nothing" (though there is formally 
no such object). The table consists of a list of entries, one for each object. 

12.3.1
In Versions 1 to 3, there are at most 255 objects, each having a 9-byte entry as follows:

   the 32 attribute flags     parent     sibling     child   properties
   ---32 bits in 4 bytes---   ---3 bytes------------------  ---2 bytes--

parent, sibling and child must all hold valid object numbers. The properties pointer is the byte address of the list of 
properties attached to the object. Attributes 0 to 31 are flags (at any given time, they are either on (1) or off (0)) and are 
stored topmost bit first: e.g., attribute 0 is stored in bit 7 of the first byte, attribute 31 is stored in bit 0 of the fourth.

12.3.2
In Version 4 and later, there are at most 65535 objects, each having a 14-byte entry as follows:

    the 48 attribute flags     parent    sibling   child     properties
   ---48 bits in 6 bytes---   ---3 words, i.e. 6 bytes----  ---2 bytes--
*/
ushort objecttable_getObjectAddress(ushort number)
{
    if (number == 0)
    {
        printf("You asked for object zero a got the void1");
        exit(1); 
    }

    if ((objecttableVersion < 4 && number <= 255) || (objecttableVersion >= 4 && number <= 65535))
    {
        return objectlistStart + (number - 1) * 2;
    }
    else
    {
        printf("Invalid object: %d for version %d", number, objecttableAddress);
        exit(1);
    }
}

ushort objecttable_getObjectParent(ushort objectAddress)
{
    if(objecttableVersion < 4)
    {
        return (objectAddress + 4);
    }
    else
    {
        return ((objectAddress + 6) << 8) + (objectAddress + 7);
    }
}

ushort objecttable_getObjectChild(ushort objectAddress)
{
    if(objecttableVersion < 4)
    {
        return (objectAddress + 6);
    }
    else
    {
        return ((objectAddress + 10) << 8) + (objectAddress + 11);
    }
}

ushort objecttable_getObjectSibbling(ushort objectAddress)
{
    if(objecttableVersion < 4)
    {
        return (objectAddress + 5);
    }
    else
    {
        return ((objectAddress + 8) << 8) + (objectAddress + 9);
    }
}

ushort objecttable_getObjectPropertiesAddress(ushort objectAddress)
{
    if(objecttableVersion < 4)
    {
        return ((objectAddress + 7) << 8) + (objectAddress + 8);
    }
    else
    {
        return ((objectAddress + 12) << 8) + (objectAddress + 13);
    }
}

byte objecttable_getObjectAttribute(ushort objectAddress, ushort attributeNumber)
{
    if ((objecttableVersion < 4 && attributeNumber < 32) || (objecttableVersion >= 4 && attributeNumber < 48))
    {
        // for every 8 bit get the next byte using division
        // then shift the byte modules 8 to the right to get the interesting bit at pos 0
        // then use "and" 1 to check if that bit is set and return
        return (zorkData[objectAddress + (attributeNumber / 8)] >> (7 - (attributeNumber % 8))) & 0x01; 
    }
    else
    {
        printf("Invalid attribute: %d for version %d", attributeNumber, objecttableVersion);
        exit(1);
    }
}
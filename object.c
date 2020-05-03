#include <stddef.h>
#include <stdio.h>
#include <stdlib.h> 
#include "object.h"
#include "data.h"

// Zork data
//extern byte *zorkData;

ushort objecttableAddress;
byte objecttableVersion;
ushort objectlistStart;

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
        // 31 default properties * 2 bytes
        objectlistStart = address + 62;
    }
    else
    {
        // 63 default properties * 2 bytes
        objectlistStart = address + 126;
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
        return data_loadWord(objecttableAddress + number);
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
        if (objecttableVersion <= 3)
        {
			//printf ("\nobjectlistStart %d number %d",objectlistStart, number);
            // first object number is one, so minus one for zero index layout. 
            // V3 and lower have 9 bytes per entry
            return objectlistStart + (number - 1) * 9;
        }
        else
        {
            // V4 and higher have 14 bytes per entry
            return objectlistStart + (number - 1) * 14;
        }
        
    }
    else
    {
        printf("Invalid object: %d for version %d", number, objecttableAddress);
        exit(1);
    }
}

ushort objecttable_getObjectParent(ushort objectNumber)
{
    // get object address from object index
    ushort objectAddress = objecttable_getObjectAddress(objectNumber);

    if(objecttableVersion < 4)
    {
        return data_loadByte(objectAddress + 4);
    }
    else
    {
        return data_loadWord(objectAddress + 6);
    }
}

void objecttable_setObjectParent(ushort objectAddress, ushort value)
{
    if(objecttableVersion < 4)
    {
        //zorkData[objectAddress + 4] = value & 0xFF;
        data_saveByte(objectAddress + 4, value);
    }
    else
    {
        //zorkData[objectAddress + 6] = (value >> 8) & 0xFF;
        //zorkData[objectAddress + 7] = (value) & 0xFF;
        data_saveWord(objectAddress + 6, value);
    }
}


ushort objecttable_getObjectChild(ushort objectNumber)
{
    // get object address from object index
    ushort objectAddress = objecttable_getObjectAddress(objectNumber);

    if(objecttableVersion < 4)
    {
        return data_loadByte(objectAddress + 6);
    }
    else
    {
        return data_loadWord(objectAddress + 10);
    }
}

void objecttable_setObjectChild(ushort objectAddress, ushort value)
{
    if(objecttableVersion < 4)
    {
        //zorkData[objectAddress + 6] = value & 0xFF;
        data_saveByte(objectAddress + 6, value);
    }
    else
    {
        //zorkData[objectAddress + 10] = (value >> 8) & 0xFF;
        //zorkData[objectAddress + 11] = (value) & 0xFF;
        data_saveWord(objectAddress + 10, value);
    }
}

ushort objecttable_getObjectSibling(ushort objectNumber)
{
    // get object address from object index
    ushort objectAddress = objecttable_getObjectAddress(objectNumber);

    if(objecttableVersion < 4)
    {
        return data_loadByte(objectAddress + 5);
    }
    else
    {
        return data_loadWord(objectAddress + 8);
    }
}

void objecttable_setObjectSibling(ushort objectAddress, ushort value)
{
     if(objecttableVersion < 4)
    {
        //zorkData[objectAddress + 5] = value & 0xFF;
        data_saveByte(objectAddress + 5, value);
    }
    else
    {
        //zorkData[objectAddress + 8] = (value >> 8) & 0xFF;
        //zorkData[objectAddress + 9] = (value) & 0xFF;
        data_saveWord(objectAddress + 8, value);
    }
}

ushort objecttable_getObjectPropertyTableAddress(ushort objectNumber)
{
    // get object address from object index
    ushort objectAddress = objecttable_getObjectAddress(objectNumber);
	//printf("objectAddress: %d", objectAddress);
    if(objecttableVersion < 4)
    {
		//printf("word: %d", data_loadWord(objectAddress + 7));
        return data_loadWord(objectAddress + 7);
    }
    else
    {
        return data_loadWord(objectAddress + 12);
    }
}

byte objecttable_getObjectAttribute(ushort objectNumber, ushort attributeNumber)
{
    // get object address from object index
    ushort objectAddres = objecttable_getObjectAddress(objectNumber);

    if ((objecttableVersion < 4 && attributeNumber < 32) || (objecttableVersion >= 4 && attributeNumber < 48))
    {
        // for every 8 bit get the next byte using division
        // then shift the byte modules 8 to the right to get the interesting bit at pos 0
        // then use "and" 1 to check if that bit is set and return
        return (data_loadByte(objectAddres + (attributeNumber / 8)) >> (7 - (attributeNumber % 8))) & 0x01; 
    }
    else
    {
        printf("Invalid attribute: %d for version %d", attributeNumber, objecttableVersion);
        exit(1);
    }
}

void objecttable_setObjectAttribute(ushort objectNumber, ushort attributeNumber)
{
    // get object address from object index
    ushort objectAddres = objecttable_getObjectAddress(objectNumber);

    if ((objecttableVersion < 4 && attributeNumber < 32) || (objecttableVersion >= 4 && attributeNumber < 48))
    {
        // for every 8 bit get the next byte using division
        // then shift the byte modules 8 to the left to get the interesting bit at the attribute position in the current byte
        // then use "or" 1 to set that bit for the object.
        data_loadByte(objectAddres + (attributeNumber / 8)) | (0x01 << (7 - (attributeNumber % 8))); 
    }
    else
    {
        printf("Invalid attribute set: %d for version %d", attributeNumber, objecttableVersion);
        exit(1);
    }
}


/*
12.4
Each object has its own property table. Each of these can be anywhere in dynamic memory (indeed, a game can legally change an object's properties 
table address in play, provided the new address points to another valid properties table). The header of a property table is as follows:

   text-length     text of short name of object
  -----byte----   --some even number of bytes---

where the text-length is the number of 2-byte words making up the text, which is stored in the usual format. (This means that an object's short 
name is limited to 765 Z-characters.) After the header, the properties are listed in descending numerical order. (This order is essential and is not a 
matter of convention.)
*//*
char * objecttable_getObjectName(ushort objectNumber)
{
    ushort propertyTableAddress = objecttable_getObjectPropertyTableAddress(objectNumber);
    byte length = zorkData[propertyTableAddress];
    char *name = readText(propertyTableAddress + 1, length);
    return *name;
}*/
// compiler problem
ushort objecttable_getObjectNameAddress(ushort objectNumber)
{
	ushort address = objecttable_getObjectPropertyTableAddress(objectNumber);
	//printf("\naddress: %d, number:%d", objecttable_getObjectPropertyTableAddress(objectNumber), objectNumber);
    return address + 1;
}

/*
12.4.1

In Versions 1 to 3, each property is stored as a block

   size byte     the actual property data
                ---between 1 and 8 bytes--

where the size byte is arranged as 32 times the number of data bytes minus one, plus the property number. A property list is terminated by a 
size byte of 0. (It is otherwise illegal for a size byte to be a multiple of 32.)

12.4.2
In Versions 4 and later, a property block instead has the form

   size and number       the actual property data
  --1 or 2 bytes---     --between 1 and 64 bytes--

The property number occupies the bottom 6 bits of the first size byte.

12.4.2.1
If the top bit (bit 7) of the first size byte is set, then there are two size-and-number bytes as follows. In the first byte, bits 0 to 5 
contain the property number; bit 6 is undetermined (it is clear in all Infocom or Inform story files); bit 7 is set. In the second byte, bits 0 to 5 
contain the property data length, counting in bytes; bit 6 is undetermined (it is set in Infocom story files, but clear in Inform ones); bit 7 is always set.

12.4.2.1.1
***[1.0] A value of 0 as property data length (in the second byte) should be interpreted as a length of 64. (Inform can compile such properties.)

12.4.2.2
If the top bit (bit 7) of the first size byte is clear, then there is only one size-and-number byte. Bits 0 to 5 contain the property number; 
bit 6 is either clear to indicate a property data length of 1, or set to indicate a length of 2; bit 7 is clear. 
*/
ushort objecttable_getFirstPropertyAddress(ushort objectNumber)
{
    ushort propertytableAddress = objecttable_getObjectPropertyTableAddress(objectNumber);
	ushort offset = data_loadByte(propertytableAddress) * 2 + 1;
	//printf("propertytableAddress: %d %d %d\n",propertytableAddress, data_loadByte(propertytableAddress), data_loadByte(propertytableAddress) * 2 + 1);
    // First property of an object should be start of property table + the lenght byte of the object name + object name bytes.
    return propertytableAddress + offset;
}

ushort objecttable_getNextPropertyAddress(ushort propertyAddress)
{
    short value = data_loadByte(propertyAddress);
    // add 1 for the size byte, moves to second size byte for V4+
    propertyAddress++;

    if (data_loadByte(0) <= 3)
    {
        value >>= 5;
    }
    else if (!(value & 0x80))
    {
	    value >>= 6;
    }
    else 
    {
	    value = data_loadByte(propertyAddress); 
        value &= 0x3f;
	    if (value == 0) value = 64;	/* demanded by Spec 1.0 */
    }

    /* Add property length to current property pointer (lenght is stored minus 1 so +1) */
    return propertyAddress + value + 1;
}
/*
Moves object O to become the first child of the destination object D. (Thus, after the operation the child of D is O, 
and the sibling of O is whatever was previously the child of D.) All children of O move with it. (Initially O can be at any point in the object tree; 
it may legally have parent zero.) 
*/
//parent    sibling   child 
void objecttable_insertObject(ushort sourceObjectNumber, ushort destinationObjectNumber)
{
    // find object addresses for both objects
    ushort sourceObjectAddress = objecttable_getObjectAddress(sourceObjectNumber);
    ushort destinationObjectAddress = objecttable_getObjectAddress(destinationObjectNumber);
    // get first child of destionation object
    ushort firstChildOfDestination = objecttable_getObjectChild(destinationObjectNumber);

    // remove object from its current location in the tree and stitch tree back together
    ushort sourceObjectParent = objecttable_getObjectParent(sourceObjectNumber);
    if (sourceObjectParent != 0)
    {
        printf("need to write tree patching code");
        exit(1);
    }

    
    //ushort firstChildOfDestinationAddress = objecttable_getObjectAddress(firstChildOfDestination);
    
    // insert source object before first child of destination object
    // make destinationobject the parent of the sourceobject
    objecttable_setObjectParent(sourceObjectAddress, destinationObjectNumber);
    // set destination child property to sourceobject
    objecttable_setObjectChild(destinationObjectAddress, sourceObjectNumber);
    // set the old first child of destination to be the sibling of sourceobject.
    objecttable_setObjectSibling(sourceObjectAddress, firstChildOfDestination);
}

short objecttable_getPropertyShort(ushort objectNumber, ushort propertyNumber)
{
    ushort propertyAddress = objecttable_getFirstPropertyAddress(objectNumber);
    ushort mask = (objecttableVersion <= 3) ? 0x1F : 0x3F;

    if (objecttableVersion <= 3)
    {
		//printf("propertyaddress %d, mask: %x", propertyAddress, mask);
        while ((data_loadByte(propertyAddress) & mask) > propertyNumber && data_loadByte(propertyAddress) != 0)
        {
            propertyAddress = objecttable_getNextPropertyAddress(propertyAddress);
        }

        if ((data_loadByte(propertyAddress) & mask) == propertyNumber)
        {
            // get property
            byte size = data_loadByte(propertyAddress) >> 5;
            if (size == 0)
            {
                return data_loadByte(propertyAddress + 1);
            }
            else
            {
                return data_loadWord(propertyAddress + 1);
            }
        }
        else
        {
            /* property list is empty or does not have property return default value */
            return objecttable_getDefaultProperty(propertyNumber);
        }
        
    }
    else
    {
        printf("implement V4 property loading!");
        exit(1);
    }
    
}

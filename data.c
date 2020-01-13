#include <stdio.h>
#include "data.h"
#include "header.h"

#define SWAP16(x) (x>>8) | (x<<8)

ushort data_highmemAddress;
byte *zData;
FILE *zFile;

void data_initialise(ushort highmemAddress, char *filename)
{
    data_highmemAddress = highmemAddress;

    //load dynamic data to memory
    zFile = fopen(filename, "rb");
    
    /*
    char cwd[2048];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf("Current working dir: %s\n", cwd);
    }
    struct stat fileStat;
    if (fstat(fileno(fp),&fileStat) < 0)    
        return 1;
 
    printf("File Size: \t\t%d bytes\n",fileStat.st_size);   
   
    /* size array for data archive */
    // allocate memory to contain the whole file:
    //zorkData = (u_int8_t*) malloc (sizeof(u_int8_t) * fileStat.st_size);
    
    zData = (byte *)malloc(sizeof(byte) * highmemAddress);
    if (zData == NULL) {fputs ("Memory error",stderr); exit (2);}
    
    /* make sure we are at the start of the file */
    fseek(zFile, 0, SEEK_SET);
    
    /* read data archive into array */
    int bytesRead = fread(zData, 1, highmemAddress, zFile);
    
    if (bytesRead == highmemAddress)
        printf("File read succesfully");
    else
    {
        printf("I could only read %d bytes.", bytesRead);
        exit(1);
    }
}

void data_close()
{
    fclose(zFile);
}

short data_loadWord(ushort address)
{
    short value = 0;
    short bytesRead = 0;

    if (address > data_highmemAddress)
    {
        //load from disk
        fseek(zFile, address, SEEK_SET);
        bytesRead = fread(&value, 1, 2, zFile); 
        value = ntohs(value); 
        return value;
    }
    else
    {
        //load from memory
        return (zData[address] << 8) + zData[address + 1];
    }
    
}

void data_saveWord(ushort address, short value)
{
     if (address > data_highmemAddress)
    {
        //save to disk
        printf("should not happen, on disk part is read only!");
        exit(1);  
    }
    else
    {
        //save to from memory
        zData[address] = value >> 8;
        zData[address + 1] = (value & 0xFF);
    }
}

byte data_loadByte(ushort address)
{
    byte value = 0;
    short bytesRead = 0;

    if (address > data_highmemAddress)
    {
        //load from disk   
        fseek(zFile, address, SEEK_SET);
        bytesRead = fread(&value, 1, 1, zFile); 
        return value;
    }
    else
    {
        //load from memory
        return zData[address];
    }
}

void data_saveByte(ushort address, byte value)
{
    if (address > data_highmemAddress)
    {
        //save to disk
        printf("should not happen, on disk part is read only!");
        exit(1);   
    }
    else
    {
        //save to memory
        zData[address] = value;
    }
}
#include <stdio.h>
#include <stdlib.h>
#include "../foenixLibrary/fatfs/ff.h"
#include "data.h"
#include "header.h"

#define SWAP16(x) (x>>8) | (x<<8)

unsigned short data_highmemAddress;
BYTE *zData;
//FILE *zFile;

FATFS FatFs;	/* Work area (filesystem object) for logical drive */
FIL zFile;		/* File object */

void data_initialise(unsigned short highmemAddress, char *filename)
{
	FRESULT fr;
	UINT bytesRead, bw;         /* File read/write count */

    data_highmemAddress = highmemAddress;

	// mount SD-card
	f_mount(&FatFs, "1:", 0);

    //load dynamic data to memory
    //zFile = fopen(filename, "rb");

	fr = f_open(&zFile, filename, FA_READ);
    if (fr)
	{
		printf("Error loading data, %d", (int)fr);
	} 

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
    
    zData = (BYTE *)malloc(highmemAddress);
    if (zData == NULL) {fputs ("Memory error\n",stderr); exit (2);}
    
    /* make sure we are at the start of the file */
    f_lseek(&zFile, 0);
    
    /* read data archive into array */
    f_read(&zFile, zData, highmemAddress, &bytesRead);
    
    if (bytesRead == highmemAddress)
        printf("File read succesfully\n");
    else
    {
        printf("I could only read %d of %d bytes.\n", bytesRead, sizeof(struct header));
        exit(1);
    }
}

void data_close()
{
    f_close(&zFile);
	f_mount(0, "1:", 0);
}

short data_loadWord(ushort address)
{
    short value = 0;
    UINT bytesRead = 0;

    if (address > data_highmemAddress)
    {
        //load from disk
        f_lseek(&zFile, address);
        f_read(&zFile, &value, 2, &bytesRead); 
        
		//value = ntohs(value); 
        asm { lda %%value; xba; sta %%value; }
		
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
    UINT bytesRead = 0;

    if (address > data_highmemAddress)
    {
        //load from disk   
        f_lseek(&zFile, address);
        f_read(&zFile, &value, 1, &bytesRead); 

        return value;
    }
    else
    {
		//printf("\nmem address: %d",address);
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


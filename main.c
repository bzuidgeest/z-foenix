#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include "header.h"
#include "stack.h"
#include "zip.h"
#include "callStack.h"

//http://jiten-thakkar.com/posts/writing-generic-stack-in-c
//https://www.techiedelight.com/introduction-linked-lists/
//https://www.codesdope.com/blog/article/making-a-stack-using-linked-list-in-c/

typedef u_int16_t ushort;

void gameLoop(void);
void ReadInstruction();
void StoreResult(short value);

void z_get_prop_len(void);

byte opcode;
byte opcodeExtended;
short operands[8];
short operandType[8];
short storeLocation;


u_int8_t *zorkData;
extern struct header *zorkHeader;

struct stack *stack;

u_int8_t currentInstruction;

u_int16_t programCounter;
short *globals = NULL;

int main()
{
    // Create the stack
    stack = stack_new(1024);

    // Clear variables
    //memset(variableStack, 0, 512 * 15);

    //create the callstack
    // fix in later version we start in "main" function, might need to push a functionData struct on the callstack for that.
    callStack_initialize();
    

    FILE * fp;

    fp = fopen ("/home/bart/ZIP/ZORK1.DAT", "rb");
    char fileText[100];
    fgets(fileText, 100, fp);
    printf("first 100 char: %s\n", fileText);
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
    zorkData = (u_int8_t*) malloc (sizeof(u_int8_t) * fileStat.st_size);
    if (zorkData == NULL) {fputs ("Memory error",stderr); exit (2);}
    
    /* make sure we are at the start of the file */
    fseek(fp, 0, SEEK_SET);
    
    /* read data archive into array */
    int bytesRead = fread(zorkData, 1, fileStat.st_size, fp);
    
    if (bytesRead == fileStat.st_size)
        printf("File read succesfully");
    else
    {
        printf("I could only read %d bytes.", bytesRead);
    }
    
    /* stick header to array */
    zorkHeader = zorkData;

    u_int16_t a = getInitialProgramCounter();
    u_int16_t b = getInitialProgramCounter();

    u_int16_t c = getObjectTableLocation();
    u_int16_t d = getGlobalVariableLocation();
    u_int16_t e = getStaticMemoryLocation();
    u_int16_t f = getHighMemoryStart();
    u_int16_t g = getAbbreviationsLocation();
    u_int16_t h = getAlphabetTableAddress();

    fclose(fp);
    
    // initialize stuff
    globals = &zorkData[getGlobalVariableLocation()];

    gameLoop();

    printf("Hello, World!");
    return 0;
} 

void gameLoop()
{
    programCounter = getInitialProgramCounter();

    
    u_int16_t operands[8];
    u_int8_t operandTypes[8];

    // Clear operandtypes
    memset(operandTypes, 0, 8);

    u_int8_t tempByte;
    do
    {
        currentInstruction = zorkData[programCounter++]; 

        /* VAR instruction */
        if ((currentInstruction & 0xC0) == 0xC0)   
        {
            if (currentInstruction >= 0xE0)
            {
                switch (currentInstruction)
                {
                    case 0xE0:
                        
                        tempByte = zorkData[programCounter++];
                        // get operand types
                        operandTypes[3] = tempByte & 0x03;
                        operandTypes[2] = (tempByte >> 2) & 0x03;
                        operandTypes[1] = (tempByte >> 4) & 0x03;
                        operandTypes[0] = (tempByte >> 6) & 0x03;

                        // Read operands
                        int i = 0;
                        while (operandTypes[i] != 0x03)
                        {
                            switch(operandTypes[i])
                            {
                                case 0x0:
                                    operands[i] = zorkData[programCounter] << 8 | zorkData[programCounter + 1];
                                    programCounter += 2;
                                    break;
                                case 0x1:
                                case 0x2:
                                    operands[i] = zorkData[programCounter++];
                                    break;
                            }
                            i++;
                        }
                        
                        // New functiondata object to push on the callstack
                        functionData fd;

                        // Read storage location and push on stack;
                        fd.returnValueStorage = zorkData[programCounter++];

                        // Push return program counter to stack
                        fd.returnAddress = programCounter;

                        // Push current stackDepth to stack and set to zero
                        fd.returnStackSize = stack_size(stack);
                        
                        // read number of locals in routine
                        // fix packed addressess implement 1.2.3 here 
                        u_int16_t x = zorkData[operands[0] << 1]; // (left shift 1 gives *2)
                        fd.locals = malloc(x * 2); // times two as short is two bytes per item
                        // Clear locals
                        memset(fd.locals, 0, x * 2);
                        for (int i = 0; i < x; i++)
                        {
                            if(operandTypes[i + 1] != 0x03)
                            {
                               fd.locals[i] = operands[i + 1]; 
                            }
                            else
                            {
                                //fd.locals[i] = 0;
                                // Done copying the operands over. break loop.
                                break;
                            }
                        }

                        // call routine
                        // counter + number of locals * 2 + 1 for the byte with local count.
                        callStack_push(fd);
                        programCounter = (operands[0] << 1) + (x * 2) + 1; 


                        break;
                    default:
                        break;
                }

            }
            else
            {
                
            }
             
            continue;
        }

        /* SHORT instruction */
        if ((currentInstruction & 0x80) == 0x80)
        {
            // 0x80 - 0x8F Large const
            // 0x90 - 0x9F small const
            // 0xA0 - 0xAF variable (locals / globals)
            switch(currentInstruction)
            {
                case 0x80:
                    break;
                case 0x81:
                    break;
                case 0x84:
                case 0x94:
                case 0xA4:
                    z_get_prop_len();
                    break;

            }
            continue;
        }

        /* Extended instructions for level 5 and above */
        if (currentInstruction == 0xBE && zorkHeader->version >= 5)
        {

            continue;
        }

        /* LONG instruction */
        switch(currentInstruction)
        {
            case 0x54:
                break;
        }

    } while (1);
    


}

void ReadInstruction()
{
    opcode = zorkData[programCounter++];

    // 2 operand
    if (opcode <= 0x7F) 
    {
        switch ((opcode >> 5))
        {
            case 0: // smallconstant smallconstant
                operands[0] = zorkData[programCounter++];
                operands[1] = zorkData[programCounter++];
                operandType[0] = 0x01;
                operandType[1] = 0x01;
                operandType[2] = 0x03;
                break;
            case 1: // smallconstant variablenumber
                operands[0] = zorkData[programCounter++];
                operands[1] = zorkData[programCounter++];
                operandType[0] = 0x01;
                operandType[1] = 0x02;
                operandType[2] = 0x03;
                break;
            case 2: // variable small
                operands[0] = zorkData[programCounter++];
                operands[1] = zorkData[programCounter++];
                operandType[0] = 0x02;
                operandType[1] = 0x01;
                operandType[2] = 0x03;
                break;
            case 3: // variable variable
                operands[0] = zorkData[programCounter++];
                operands[1] = zorkData[programCounter++];
                operandType[0] = 0x02;
                operandType[1] = 0x02;
                operandType[2] = 0x03;
                break;
            default:
                // Impossible do nothing
                break;
        }


    }

    // 2 operand and 0 operand
    if (opcode >= 0x80 && opcode < 0xBF)
    {
        switch ((opcode >> 4))
        {
            case 0: // largeconstant
                operands[0] = (zorkData[programCounter++] << 8) + zorkData[programCounter++];
                operandType[0] = 0x00;
                operandType[1] = 0x03;
                break;
            case 1: // smallconstant
                operands[0] = zorkData[programCounter++];
                operandType[0] = 0x01;
                operandType[1] = 0x03;
                break;
            case 2: // variable
                operands[0] = zorkData[programCounter++];
                operandType[0] = 0x02;
                operandType[1] = 0x03;
                break;
            case 3: // zero op or extended op
                if (opcode == 0xBE)
                {
                    opcodeExtended = zorkData[programCounter++];
                    // fix load operands
                }
                else
                {
                    operandType[0] = 0x03;
                }
                
                break;
            default:
                // Impossible do nothing
                break;
        }
    }

    // variable operand count opcodes
    if (opcode >= 0xC0 && opcode < 0xFF)
    {
        byte tempByte = zorkData[programCounter++];
        // get operand types
        for (byte i = 3; i >= 0; i--)
        {
            operandType[i] = tempByte & 0x03;
            tempByte = tempByte >> 2;
        }
        if (operandType[3] != 0x03 && operandType[2] != 0x03 && operandType[1] != 0x03 && operandType[0] != 0x03)
        {
            // Load second operand byte
            tempByte = zorkData[programCounter++];
            for (byte i = 7; i >= 4; i--)
            {
                operandType[i] = tempByte & 0x03;
                tempByte = tempByte >> 2;
            }
        }

        // Read operands
        for(int i = 0; operandType[i] != 0x3 && i < 8; i++)
        {
            switch(operandType[i])
            {
                case 0x0:
                    //operands[i] = zorkData[programCounter] << 8 | zorkData[programCounter + 1];
                    //programCounter += 2;
                    operands[i] = zorkData[programCounter++] << 8 | zorkData[programCounter++];
                    break;
                case 0x1:
                case 0x2:
                    operands[i] = zorkData[programCounter++];
                    break;
            }
        }
    }

    storeLocation = zorkData[programCounter++];
}

void StoreResult(short value)
{
    if (storeLocation == 0)
    {
        stack_push(stack, value);
    }
    else if (storeLocation <= 15)
    {
        callStack_top().locals[storeLocation - 1] = value;
    }
    else
    {
        globals[storeLocation - 0x0F] = value;
    }
}

void z_get_prop_len(void)
{
    u_int16_t location = 0;
    u_int16_t store = 0;
    short value;

    if (currentInstruction = 0x94)
    {
        // get location of property
        // apperenty location is location of actual property data, not location of preceding size byte
        // fix unsure what location represents, might be a object number in the object table.
        location = zorkData[programCounter++] - 1;
        // get location to store result
        store = zorkData[programCounter++];

        // get actual value
        //addr = zargs[0] - 1;
        value = zorkData[location];

        /* Calculate length of property */

        //if (h_version <= V3)
            value = (value >> 5) + 1;
        //else if (!(value & 0x80))
        //    value = (value >> 6) + 1;
        /*else {

        value &= 0x3f;

        if (value == 0) value = 64;	/* demanded by Spec 1.0 */

        //}

        if (store == 0)
        {
            stack_push(stack, value);
        }
        else if (store <= 15)
        {
            callStack_top().locals[store - 1] = value;
        }
        else
        {
            globals[store - 0x0F] = value;
        }
        
    }
}
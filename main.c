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

void initialize(void);
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

//u_int8_t currentInstruction;

u_int16_t programCounter;
short globals[240];

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
    //globals = &zorkData[getGlobalVariableLocation()];
	initialize();

    gameLoop();

    printf("Hello, World!");
    return 0;
} 

void initialize(void)
{
	ushort address = getGlobalVariableLocation();
	// load initial values for globals
	for(byte i = 0; i < 240; i++)
	{
		globals[i] = (zorkData[address] << 8) + zorkData[address + 1];
		address += 2;
	}
}

void gameLoop()
{
    programCounter = getInitialProgramCounter();

    printf("\n");
    //u_int16_t operands[8];
    //u_int8_t operandTypes[8];

    // Clear operandtypes
    //memset(operandTypes, 0, 8);

    byte tempByte;
    do
    {
		printf("PC: %d,", programCounter);
        ReadInstruction();
        //currentInstruction = zorkData[programCounter++]; 
		printf(" OPCODE: 0x%X (%d)\n", opcode, opcode);

        /* VAR instruction */
        if ((opcode & 0xC0) == 0xC0)   
        {
            if (opcode >= 0xE0)
            {
                switch (opcode)
                {
                    case 0xE0:
						z_call_vs();
                        break;
                    case 0xE1:
                        z_storew();
                        break;
                    default:
						printf("unimplemented opcode A: %d", opcode);
                        exit(1);
						break;
                }

            }
            else
            {
				printf("unimplemented opcode B: %d", opcode);
                exit(1);
				break;
            }
             
            continue;
        }

        /* SHORT instruction */
        if ((opcode & 0x80) == 0x80)
        {
            // 0x80 - 0x8F Large const
            // 0x90 - 0x9F small const
            // 0xA0 - 0xAF variable (locals / globals)
            switch(opcode)
            {
                case 0x80:
                    break;
                case 0x81:
                    break;
                case 0xA0:
                    z_jz();
                    break;
                case 0x84:
                case 0x94:
                case 0xA4:
                    z_get_prop_len();
                    break;
                /*case 0xAB:
                    z_ret()
                    break;*/
				default: 
					printf("unimplemented opcode C: %d\n", opcode);
                    exit(1);
					break;
            }
            continue;
        }

        /* Extended instructions for level 5 and above */
        if (opcode == 0xBE && zorkHeader->version >= 5)
        {
			printf("unimplemented opcode D: %d\n", opcode);
            exit(1);
			break;

            continue;
        }

        /* LONG instruction */
        switch(opcode)
        {
            case 0x54:
			case 0x74:
				z_add();
                break;
            case 0x55:
                z_sub();
                break;
			case 0x61:
				z_je();
                break;
			default:
				printf("unimplemented opcode E: %d\n", opcode);
                exit(1);
				break;
        }

    } while (1);
    


}

void ReadInstruction()
{
    // Clear operandtypes and operands
    memset(operandType, 0, 8);
    memset(operands, 0, 8);

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
					printf("Extended opcode, need to implement operand loading!");
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
        for (short i = 3; i >= 0; i--)
        {
            operandType[i] = tempByte & 0x03;
            tempByte = tempByte >> 2;
        }
        if (operandType[3] != 0x03 && operandType[2] != 0x03 && operandType[1] != 0x03 && operandType[0] != 0x03)
        {
            // Load second operand byte
            tempByte = zorkData[programCounter++];
            for (short i = 7; i >= 4; i--)
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
}

/* 4.6 "Store" instructions return a value: e.g., mul multiplies its two operands together. 
Such instructions must be followed by a single byte giving the variable number of where to put the result. 
*/
void storeResult(short value)
{
	ushort storeLocation = zorkData[programCounter++];

	printf("Storing %d to %d\n", value, storeLocation);

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

/* 4.7
Instructions which test a condition are called "branch" instructions. The branch information is stored in one or two bytes, 
indicating what to do with the result of the test. If bit 7 of the first byte is 0, a branch occurs when the condition was false; 
if 1, then branch is on true. If bit 6 is set, then the branch occupies 1 byte only, and the "offset" is in the range 0 to 63, 
given in the bottom 6 bits. If bit 6 is clear, then the offset is a signed 14-bit number given in bits 0 to 5 of the first byte 
followed by all 8 of the second.

4.7.1
An offset of 0 means "return false from the current routine", and 1 means "return true from the current routine".

4.7.2
Otherwise, a branch moves execution to the instruction at address
*/
void branchTo(int value)
{
    byte branchByte1 = zorkData[programCounter++];
    short branchOffset = 0;

    if ((branchByte1 & 0b01000000) == 0)
    {
        // 2 byte offset
        branchOffset = ((branchByte1 & 0b00111111) << 8) + zorkData[programCounter++];
    }
    else
    {
        // 1 byte offset
        branchOffset = (branchByte1 & 0b00111111);
    }
    
    if ((branchByte1 & 0xb10000000 && value == 1) || ((branchByte1 & 0xb10000000) == 0 && value == 0))
    {
        programCounter = programCounter + branchOffset - 1;
    }

    if (branchOffset == 0)
    {
        printf("Implement return true or false from function\n");
        exit(1);
    }
}

void z_add(void)
{
	ushort a = 0;
	ushort b = 0;
	ushort result = 0;

	if (opcode == 0x54)
	{
		if(operands[0] < 0x0F)
		{
			a = callStack_top().locals[operands[0]];
		}
		else
		{
			a = globals[operands[0] - 0x0F];
		}
		
		result = a + operands[1];
	}

	if (opcode == 0x74)
	{
		if(operands[0] < 0x0F)
		{
			a = callStack_top().locals[operands[0]];
		}
		else
		{
			a = globals[operands[0] - 0x0F];
		}
		
		if(operands[1] < 0x0F)
		{
			b = callStack_top().locals[operands[1]];
		}
		else
		{
			b = globals[operands[1] - 0x0F];
		}
		result = a + b;
	}

	storeResult(result);
}

void z_sub(void)
{
	ushort a = 0;
	ushort b = 0;
	ushort result = 0;

	if (opcode == 0x55)
	{
		if(operands[0] < 0x0F)
		{
			a = callStack_top().locals[operands[0]];
		}
		else
		{
			a = globals[operands[0] - 0x0F];
		}
		
		result = a - operands[1];
	}

/*	if (opcode == 0x74)
	{
		if(operands[0] < 0x0F)
		{
			a = callStack_top().locals[operands[0]];
		}
		else
		{
			a = globals[operands[0] - 0x0F];
		}
		
		if(operands[1] < 0x0F)
		{
			b = callStack_top().locals[operands[1]];
		}
		else
		{
			b = globals[operands[1] - 0x0F];
		}
		result = a + b;
	}*/

	storeResult(result);
}

void z_call_vs(void)
{
	// New functiondata object to push on the callstack
	functionData fd;

	// Read storage location and push on stack;
	// do not read here, read after return
	//fd.returnValueStorage = storeLocation; //zorkData[programCounter++];

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
		if(operandType[i + 1] != 0x03)
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

}

void z_je(void)
{
	int a = 0;
	int b = 0;
	int result;

	if (opcode == 0x61)
	{
		if(operands[0] < 0x0F)
		{
			a = callStack_top().locals[operands[0] - 1];
		}
		else
		{
			a = globals[operands[0] - 0x0F];
		}
		
		if(operands[1] < 0x0F)
		{
			b = callStack_top().locals[operands[1] - 1];
		}
		else
		{
			b = globals[operands[1] - 0x0F];
		}
		
		
        branchTo(a == b);
	}
}

void z_jz(void)
{
	short a = 0;
/*
	if (opcode == 0x61)
	{
		if(operands[0] < 0x0F)
		{
			a = callStack_top().locals[operands[0] - 1];
		}
		else
		{
			a = globals[operands[0] - 0x0F];
		}
		
        branchTo(a == b);
	}*/

    if (opcode == 0xA0)
    {
        if(operands[0] < 0x0F)
		{
			a = callStack_top().locals[operands[0] - 1];
		}
		else
		{
			a = globals[operands[0] - 0x0F];
		}

        branchTo(a == 0);
    }
}

void z_get_prop_len(void)
{
	printf("needs validation/completion");
    u_int16_t location = 0;
    u_int16_t store = 0;
    short value;

    if (opcode = 0x94)
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

/* 
storew array word-index value
array-->word-index = value, i.e. stores the given value in the word at address array+2*word-index (which must lie in dynamic memory). (See loadw.) 
*/
void z_storew(void)
{
    ushort array = 0;
    short wordIndex = 0;
    short value = 0;

    if (operandType[0] == 0x02)
    {
        if(operands[0] < 0x0F)
		{
			array = callStack_top().locals[operands[0] - 1];
		}
		else
		{
			array = globals[operands[0] - 0x0F];
		}
    }
    else
    {
        array = operands[0];
    }
    
    if (operandType[1] == 0x02)
    {
        if(operands[1] < 0x0F)
		{
			wordIndex = callStack_top().locals[operands[0] - 1];
		}
		else
		{
			wordIndex = globals[operands[0] - 0x0F];
		}
    }
    else
    {
        wordIndex = operands[1];
    }

    if (operandType[2] == 0x02)
    {
        if(operands[2] < 0x0F)
		{
			value = callStack_top().locals[operands[2] - 1];
		}
		else
		{
			value = globals[operands[0] - 0x0F];
		}
    }
    else
    {
        value = operands[2];
    }
    
    zorkData[array + 2 * wordIndex] = (value >> 8); 
    zorkData[array + 2 * wordIndex] = (value & 0x0F); 
}

void z_new_line(void)
{
    // FIX factor out for foenix
    printf("\n");
}
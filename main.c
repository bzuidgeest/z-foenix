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

#define DEBUGx

//http://jiten-thakkar.com/posts/writing-generic-stack-in-c
//https://www.techiedelight.com/introduction-linked-lists/
//https://www.codesdope.com/blog/article/making-a-stack-using-linked-list-in-c/

typedef u_int16_t ushort;

void initialize(void);
void gameLoop(void);
void ReadInstruction(void);
short loadVariable(short number);
void storeVariable(short location, short value);
void StoreResult(short value);

void z_get_prop_len(void);

byte opcode;
byte opcodeExtended;
short operands[8];
short operandType[8];
short storeLocation;

int instructionCount = -1;

byte *zorkData;
extern struct header *zorkHeader;

struct stack *stack;

//u_int8_t currentInstruction;

ushort programCounter;
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
    u_int16_t b = getRelease();

    u_int16_t c = getObjectTableLocation();
    u_int16_t d = getGlobalVariableLocation();
    u_int16_t e = getStaticMemoryLocation();
    u_int16_t f = getHighMemoryStart();
    u_int16_t g = getAbbreviationsLocation();
    u_int16_t h = getAlphabetTableAddress();

    fclose(fp);
    
    printf("\n\n\n");
    // initialize stuff
    //globals = &zorkData[getGlobalVariableLocation()];
	initialize();

    gameLoop();

    //printf("Hello, World!");
    return 0;
} 

void initialize(void)
{
    byte i = 0;
	ushort address = getGlobalVariableLocation();
	
    	// load initial values for globals
	for(i = 0; i < 240; i++)
	{
		globals[i] = (zorkData[address] << 8) + zorkData[address + 1];
		address += 2;
	}

    objecttable_initialize(getObjectTableLocation(), zorkHeader->version);
    text_initialize(zorkHeader->version, getAbbreviationsLocation());
}

void gameLoop()
{
    byte tempByte;

    programCounter = getInitialProgramCounter();
    
    do
    {
        #ifdef DEBUG
		printf("PC: %d,", programCounter);
        #endif

        instructionCount++;

        ReadInstruction();
		
        #ifdef DEBUG
        printf(" OPCODE: 0x%X (%d) instruction: %d\n", opcode, opcode, instructionCount);

        int i = 0;
        while (operandType[i] != 3)
        {
            printf("\tOperand %d value: %d(%d) type: %d\n", i, operands[i], (ushort)operands[i], operandType[i]);
            i++;
        }
        #endif
//printf("%d", instructionCount);
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
                    case 0xE3:
                        z_put_prop();
                        break;
                    case 0xE4:
                        z_read();
                        break;
                    case 0xE5:
                        z_print_char();
                        break;
                    case 0xE6:
                        z_print_num();
                        break;
                    case 0xE8:
                        z_push();
                        break;
                    case 0xE9:
                        z_pull();
                        break;
                    default:
						printf("unimplemented opcode A: %X (%d)", opcode, opcode);
                        exit(1);
						break;
                }

            }
            else
            {
                switch(opcode)
                {
                    case 0xC1:
                        z_je();
                        break;
                    case 0xC9:
                        z_and();
                        break;
                    default:
				        printf("unimplemented opcode B: %d", opcode);
                        exit(1);
				        break;
                }
            }
             
            //continue;
            goto endloop;
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
                case 0x8C:
                    z_jump();
                    break;
                case 0x95:
                    z_inc();
                    break;
                case 0xA1:
                    z_get_sibling();
                    break;
                case 0xA2:
                    z_get_child();
                    break;
                case 0xA3:
                    z_get_parent();
                    break;
                case 0xAA:
                    z_print_obj();
                    break;
                case 0xAB:
                    z_ret();
                    break;
                case 0xB0:
                    z_rtrue();
                    break;
                case 0xB1:
                    z_rfalse();
                    break;
                case 0xB2:
                    z_printf();
                    break;
                case 0xB8:
                    z_ret_popped();
                    break;
                case 0xBB:
                    z_new_line();
                    break;
				default: 
					printf("unimplemented opcode C: %d\n", opcode);
                    exit(1);
					break;
            }
            //continue;
            goto endloop;
        }

        /* Extended instructions for level 5 and above */
        if (opcode == 0xBE && zorkHeader->version >= 5)
        {
			printf("unimplemented opcode D: %d\n", opcode);
            exit(1);
			break;

            //continue;
            goto endloop;
        }

        /* LONG instruction */
        switch(opcode)
        {
            case 0x41:
            case 0x61:
				z_je();
                break;
            case 0x42:
                z_jl();
                break;
            case 0x04:
                z_dec_chk();
                break;
            case 0x05:
                z_inc_chk();
                break;
            case 0x10:
            case 0x30:
                z_loadb();
                break;
            case 0x0D:
            case 0x2D:
                z_store();
                break;
            case 0x46:
                z_jin();
                break;
            case 0x49:
                z_and();
                break;
            case 0x4A:
                z_test_attr();
                break;
            case 0x4B:
                z_set_attr();
                break;
            case 0x0F:
            case 0x4F:
                z_loadw();
                break;
            case 0x54:
			case 0x74:
				z_add();
                break;
            case 0x51:
                z_get_prop();
                break;
            case 0x55:
                z_sub();
                break;
			
            case 0x6E:
				z_insert_obj();
                break;
			default:
				printf("unimplemented opcode E: %d\n", opcode);
                exit(1);
				break;
        }

        endloop:
        //dummy printf to attach breakpoint
        printf("");
    } 
    while (1);
    


}

void ReadInstruction()
{
    short i = 0;
    byte tempByte = 0;

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
                operands[1] = loadVariable(zorkData[programCounter++]);
                operandType[0] = 0x01;
                operandType[1] = 0x02;
                operandType[2] = 0x03;
                break;
            case 2: // variable small
                operands[0] = loadVariable(zorkData[programCounter++]);
                operands[1] = zorkData[programCounter++];
                operandType[0] = 0x02;
                operandType[1] = 0x01;
                operandType[2] = 0x03;
                break;
            case 3: // variable variable
                operands[0] = loadVariable(zorkData[programCounter++]);
                operands[1] = loadVariable(zorkData[programCounter++]);
                operandType[0] = 0x02;
                operandType[1] = 0x02;
                operandType[2] = 0x03;
                break;
            default:
                // Impossible do nothing
                break;
        }


    }

    // 1 operand and 0 operand
    if (opcode >= 0x80 && opcode < 0xBF)
    {
        switch ((opcode >> 4) & 0x03)
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
                operands[0] = loadVariable(zorkData[programCounter++]);
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
        
        tempByte = zorkData[programCounter++];
        
        // get operand types
        for (i = 3; i >= 0; i--)
        {
            operandType[i] = tempByte & 0x03;
            tempByte = tempByte >> 2;
        }
        // 4.5.1 Note that only call_vs2 and call_vn2 can have more than 4 operands, and no instruction can have more than 8. 
        //if (operandType[3] != 0x03 && operandType[2] != 0x03 && operandType[1] != 0x03 && operandType[0] != 0x03 && opcode != 0xE0)
        if (opcode == 0xEC || opcode == 0xFA)
        {
            // Load second operand byte
            tempByte = zorkData[programCounter++];
            for (i = 7; i >= 4; i--)
            {
                operandType[i] = tempByte & 0x03;
                tempByte = tempByte >> 2;
            }
        }
        else
        {
            // signal "end of operands" in case of 4 operand calls. Does nothing when less than 4 operands are used.
            operandType[4] = 0x03;
        }
        

        // Read operands
        for(i = 0; operandType[i] != 0x3 && i < 8; i++)
        {
            switch(operandType[i])
            {
                case 0x0:
                    operands[i] = zorkData[programCounter++] << 8 | zorkData[programCounter++];
                    break;
                case 0x1:
                    operands[i] = zorkData[programCounter++];
                    break;
                case 0x2:
                    // get variable number
                    operands[i] = loadVariable(zorkData[programCounter++]);
                    break;
            }
        }
    }

   
}

short loadVariable(short number)
{
    // change variable number to variable value for processing

    if (number == 0)
    {
        return stack_pop(stack);
    }
    else if(number <= 0x0F)
    {
        // 0 is stack so 1 is first local giving an array offset of 1
        return callStack_top()->locals[number - 1];
    }
    else
    {
        // globals start after 16 (stack at 0 plus 15 locals) so offset 0x10
        return globals[number - 0x10];
    }
}

/* 4.6 "Store" instructions return a value: e.g., mul multiplies its two operands together. 
Such instructions must be followed by a single byte giving the variable number of where to put the result. 
*/
void storeResult(short value)
{
	ushort storeLocation = zorkData[programCounter++];

    if (storeLocation == 0)
    {
        #ifdef DEBUG
        printf("Storing %d (%d) to stack\n", value, (ushort)value, storeLocation);
        #endif

        stack_push(stack, value);
    }
    else if (storeLocation <= 15)
    {
        #ifdef DEBUG
        printf("Storing local %d (%d)  to %d\n", value, (ushort)value, storeLocation);
        #endif

        callStack_top()->locals[storeLocation - 1] = value;
    }
    else
    {
        #ifdef DEBUG
        printf("Storing global %d (%d) to %d\n", value, (ushort)value, storeLocation - 0x10);
        #endif

        globals[storeLocation - 0x10] = value;
    }
}

void storeVariable(short location, short value)
{
    if (location == 0)
    {
        #ifdef DEBUG
        printf("Storing %d (%d) to stack\n", value, (ushort)value, location);
        #endif

        stack_push(stack, value);
    }
    else if (location <= 15)
    {
        #ifdef DEBUG
        printf("Storing local %d (%d) to %d\n", value, (ushort)value, location);
        #endif

        callStack_top()->locals[location - 1] = value;
    }
    else
    {
        #ifdef DEBUG
        printf("Storing global %d (%d) to %d\n", value, (ushort)value, location - 0x10);
        #endif

        globals[location - 0x10] = value;
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
Address after branch data + Offset - 2.
*/
void branchTo(int value)
{
    short branchOffset = 0;
    byte branchByte1 = 0;
    branchByte1 = zorkData[programCounter++];

    if ((branchByte1 & 0x40) == 0)
    {
        // 2 byte offset
        branchOffset = ((branchByte1 & 0x3F) << 8) + zorkData[programCounter++];
    }
    else
    {
        // 1 byte offset
        branchOffset = (branchByte1 & 0x3F);
    }
    
    if ((branchByte1 >> 7 && value == 1) || ((branchByte1 >> 7) == 0 && value == 0))
    {
        if (branchOffset == 0 || branchOffset == 1)
        {
            returnfromRoutine(branchOffset);
        }
        else
        {
            programCounter = programCounter + branchOffset - 2;
        }
    }
}


void returnfromRoutine(short returnValue)
{
    #ifdef DEBUG
    printf("return from: %d\n", callStack_Size());
    #endif

    // return to previous routing by popping the current one from the stack
    functionData returnFrom = callStack_pop();

    // return program counter to previous value;
    programCounter = returnFrom.returnAddress;

    // free the locals. Is this needed?
    //free(returnFrom.locals);

    // pop all unused data from current routine the stack
    while (stack_size(stack) >  returnFrom.returnStackSize)
    {
        stack_pop(stack);
    }

    // return the function result.
    storeResult(returnValue);
}


void z_add(void)
{
	ushort result = 0;

    result = operands[0] + operands[1];

	storeResult(result);
}

void z_sub(void)
{
	ushort result = 0;

    result = operands[0] - operands[1];

	storeResult(result);
}

/*
VAR:224 0 1 call routine ...up to 3 args... -> (result)

The only call instruction in Version 3, Inform reads this as call_vs in higher versions: it calls the routine with 0, 1, 2 or 3 arguments 
as supplied and stores the resulting return value. (When the address 0 is called as a routine, nothing happens and the return value is false.) 
*/
void z_call_vs(void)
{
    short i = 0;
    ushort x;

    if(operands[0] == 0)
    {
        storeResult(0);
        return;
    }

	// New functiondata object to push on the callstack
    callStack_push();
	//callStack_top()->locals = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	// Read storage location and push on stack;
	// do not read here, read after return
	//fd.returnValueStorage = storeLocation; //zorkData[programCounter++];
    
	// Push return program counter to stack
	callStack_top()->returnAddress = programCounter;

	// Push current stackDepth to stack and set to zero
	callStack_top()->returnStackSize = stack_size(stack);
	
	// read number of locals in routine
	// fix packed addressess implement 1.2.3 here 
	x = zorkData[operands[0] << 1]; // (left shift 1 gives *2)
	//fd.locals = malloc(x * 2); // times two as short is two bytes per item
	
    // Clear and load locals from routine header
	//memset(fd.locals, 0, x * 2);
    memset(callStack_top()->locals, 0, 15 * 2);
	for (i = 0; (i / 2) < x; i += 2)
	{
        callStack_top()->locals[i / 2] = (zorkData[(operands[0] << 1) + 1 + i] << 8) + zorkData[(operands[0] << 1) + 2 + i];
	}

    // set locals from function operands
    // first operand is routine address so skip that.
    for (i = 1; operandType[i] != 0x03; i++)
	{
        callStack_top()->locals[i - 1] = operands[i];
	}

	// call routine
	// counter + number of locals * 2 + 1 for the byte with local count.
	//callStack_push(fd);
	programCounter = (operands[0] << 1) + (x * 2) + 1; 


    #ifdef DEBUG
    printf("Added frame: %d returnAddress: %d \n", callStack_Size(), top->data.returnAddress);
    #endif
}

void z_je(void)
{
    short i = 0;
    ushort result = 0;

    if (opcode != 0xC1)
    {
        branchTo(operands[0] == operands[1]);
    }
    else
    {
        for (i = 1; operandType[i] != 3; i++)
        {
            if (operands[0] == operands[i])
                result = 1;
        }
        branchTo(result);
    }
    
}

void z_jz(void)
{
    branchTo(operands[0] == 0);
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

        
        
    }
    
    storeResult(value);
}

/* 
2OP:13 D store (variable) value
Set the VARiable referenced by the operand to value. 
*/
void z_store(void)
{
    storeVariable(operands[0], operands[1]);
}

/* 
storew array word-index value
array-->word-index = value, i.e. stores the given value in the word at address array+2*word-index (which must lie in dynamic memory). (See loadw.) 
*/
void z_storew(void)
{
    ushort array = operands[0];
    short wordIndex = operands[1];
    short value = operands[2];

    zorkData[array + 2 * wordIndex] = (value >> 8); 
    zorkData[(array + 2 * wordIndex) + 1] = (value & 0xFF); 
}



// 2OP:4 4 dec_chk (variable) value ?(label)
// Decrement variable, and branch if it is now less than the given value.
void z_dec_chk(void)
{
    short variableValue = 0;
    short variableNumber = 0;

    // constant gives variable to load
    variableNumber = operands[0];
    variableValue = loadVariable(variableNumber);

    // decrement value and store back;
    storeVariable(variableNumber, --variableValue);
    
    branchTo(variableValue < operands[1]);
}

/*
2OP:5 5 inc_chk (variable) value ?(label)

Increment variable, and branch if now greater than value.
*/
void z_inc_chk(void)
{
    short variableValue = 0;
    short variableNumber = 0;

    // constant gives variable to load
    variableNumber = operands[0];
    variableValue = loadVariable(variableNumber);

    // increment value and store back;
    storeVariable(variableNumber, ++variableValue);
    
    branchTo(variableValue > operands[1]);
}

/*
2OP:16 10 loadb array byte-index -> (result)

Stores array->byte-index (i.e., the byte at address array+byte-index, which must lie in static or dynamic memory).  
*/
void z_loadb(void)
{
    ushort array = 0;
    short byteIndex = 0;
    byte value = 0;

    array = operands[0];
    byteIndex = operands[1];

    value = zorkData[array + byteIndex];
    storeResult(value);
}

/*
2OP:15 F loadw array word-index -> (result)

Stores array-->word-index (i.e., the word at address array+2*word-index, which must lie in static or dynamic memory). 
*/
void z_loadw(void)
{
    ushort array = 0;
    short wordIndex = 0;
    short value = 0;

    array = operands[0];
    wordIndex = operands[1];

    value = (zorkData[array + 2 * wordIndex] << 8) + zorkData[(array + 2 * wordIndex) + 1];    
    storeResult(value);
}

/*
VAR:227 3 put_prop object property value

Writes the given value to the given property of the given object. If the property does not exist for that object, 
the interpreter should halt with a suitable error message. If the property length is 1, then the interpreter should 
store only the least significant byte of the value. (For instance, storing -1 into a 1-byte property results in the property value 255.) 
As with get_prop the property length must not be more than 2: if it is, the behaviour of the opcode is undefined. 
*/
void z_put_prop(void)
{
    ushort propertyAdress;
    short value;
    byte propertyNumberMask;

    if (operands[0] == 0) {
	    printf ("Cannot put something in object 0");
	    exit(1);
    }

    /* Property id is in bottom five or six bits */
    propertyNumberMask = (zorkHeader->version <= 3) ? 0x1f : 0x3f;

    /* Load address of first property */
    propertyAdress = objecttable_getFirstPropertyAddress(operands[0]);

    /* Scan down the property list */
    for (;;) 
    {
	    value = zorkData[propertyAdress];
	    if ((value & propertyNumberMask) <= operands[1])
	        break;
	    propertyAdress = objecttable_getNextPropertyAddress(propertyAdress);
    }

    /* Exit if the property does not exist */
    if ((value & propertyNumberMask) != operands[1])
    {
        printf("Property does not exist");
        exit(1);
    }

    /* Store the new property value (byte or word sized) */
    // skip the length byte V1+
    propertyAdress++;

    if ((zorkHeader->version <= 3 && !(value & 0xe0)) || (zorkHeader->version >= 4 && !(value & 0xc0))) 
    {
	    zorkData[propertyAdress] = operands[2] & 0xFF;
    } 
    else 
    {
	    zorkData[propertyAdress] = (operands[2] >>8);
        zorkData[propertyAdress + 1] = operands[2] & 0xFF;
    }
}

/*
2OP:10 A test_attr object attribute ?(label)

Jump if object has attribute.
*/
void z_test_attr(void)
{
    ushort object = 0;
    byte attribute = 0;

    object = operands[0];
    attribute = operands[1];

    branchTo(objecttable_getObjectAttribute(object, attribute));
}

/*
2OP:14 E insert_obj object destination

Moves object O to become the first child of the destination object D. (Thus, after the operation the child of D is O, 
and the sibling of O is whatever was previously the child of D.) All children of O move with it. (Initially O can be at any point in the object tree; 
it may legally have parent zero.) 
*/
void z_insert_obj(void)
{
    ushort object = 0;
    byte destination = 0;

    object = operands[0];
    destination = operands[1];

    objecttable_insertObject(object, destination);
}

/*
0OP:178 2 print <literal-string>

Print the quoted (literal) Z-encoded string.
*/
void z_printf(void)
{
    programCounter += text_printLiteral(programCounter);
}

/*
0OP:187 B new_line

Print carriage return.
*/
void z_new_line(void)
{
    printf("\n");
}

/*
2OP:9 9 and a b -> (result)

Bitwise AND.
*/
void z_and(void)
{
    ushort a = 0;
    ushort b = 0;

    a = operands[0];
    b = operands[1];

    storeResult(a & b);
}

/*
VAR:230 6 print_num value

Print (signed) number in decimal.
*/
void z_print_num(void)
{
    printf("%d", operands[0]);
}

/*
1OP:140 C jump ?(label)

Jump (unconditionally) to the given label. (This is not a branch instruction and the operand is a 2-byte signed offset to apply to the program counter.) 
It is legal for this to jump into a different routine (which should not change the routine call state), although it is considered bad practice to do 
so and the Txd disassembler is confused by it.

The destination of the jump opcode is:
Address after instruction + Offset - 2
This is analogous to the calculation for branch offsets.
*/
void z_jump(void)
{
    programCounter = programCounter + operands[0] - 2;
}

/*
VAR:229 5 print_char output-character-code

Print a ZSCII character. The operand must be a character code defined in ZSCII for output (see S 3). In particular, it must certainly not be negative or 
larger than 1023. 
*/
void z_print_char(void)
{
    if (operands[0] < 0 && operands[0] > 1023)
    {
        printf("Illegal ZSCII character: %d", operands[0]);
        exit(1);
    }

    if (operands[0] >= 32 && operands[0] <=126)
    {
        printf("%c", operands[0]);
    }
    else
    {
        if (operands[0] > 155 && operands[1] < 255)
        {
            printf("extended char, not implemented yet.");
        }
        else if (operands[0] == 13)
        {
            printf("\n");
        }
        else
        {
            printf("unimplemented character: %d", operands[0]);
        }
        
    }
}

void z_ret(void)
{
    returnfromRoutine(operands[0]);
}

/*
0OP:176 0 rtrue

Return true (i.e., 1) from the current routine.
*/
void z_rtrue(void)
{
    returnfromRoutine(1);
} 

/*
0OP:177 1 rfalse

Return false (i.e., 0) from the current routine.
*/
void z_rfalse(void)
{
    returnfromRoutine(0);
}

/*
VAR:232 8 push value

Pushes value onto the game stack.
*/
void z_push(void)
{
    stack_push(stack, operands[0]);
}

/*
VAR:233 9 1 pull (variable)

6 pull stack -> (result)

Pulls value off a stack. (If the stack underflows, the interpreter should halt with a suitable error message.) 
In Version 6, the stack in question may be specified as a user one: otherwise it is the game stack. 
*/
void z_pull(void)
{
    if (zorkHeader->version < 6)
    {
        storeVariable(operands[0], stack_pop(stack));
    }
    else
    {
        printf("user stack unimplemented.");
        exit(1);
    }
    
}

/*
2OP:11 B set_attr object attribute

Make object have the attribute numbered attribute.
*/
void z_set_attr(void)
{
    objecttable_setObjectAttribute(operands[0], operands[1]);
}

/*
2OP:6 6 jin obj1 obj2 ?(label)

Jump if object a is a direct child of b, i.e., if parent of a is b. 
*/
void z_jin(void)
{
    branchTo(objecttable_getObjectParent(operands[0]) == operands[1]);
}

/*
1OP:138 A print_obj object

Print short name of object (the Z-encoded string in the object header, 
not a property). If the object number is invalid, the interpreter should halt with a suitable error message. 
*/
void z_print_obj(void)
{
    text_printLiteral(objecttable_getObjectNameAddress(operands[0]));
}

/*
1OP:131 3 get_parent object -> (result)

Get parent object (note that this has no "branch if exists" clause). 
*/
void z_get_parent(void)
{
    storeResult(objecttable_getObjectParent(operands[0]));
}

/*
2OP:17 11 get_prop object property -> (result)

Read property from object (resulting in the default value if it had no such declared property). If the property has length 1, the value 
is only that byte. If it has length 2, the first two bytes of the property are taken as a word value. It is illegal for the opcode to be used if 
the property has length greater than 2, and the result is unspecified. 
*/
void z_get_prop(void)
{
    storeResult(objecttable_getPropertyShort(operands[0], operands[1]));
}

/*
1OP:130 2 get_child object -> (result) ?(label)

Get first object contained in given object, branching if this exists, i.e. is not nothing (i.e., is not 0). 
*/
void z_get_child(void)
{
    ushort child = objecttable_getObjectChild(operands[0]);
    storeResult(child);
    branchTo(child != 0);
}

/*
1OP:129 1 get_sibling object -> (result) ?(label)

Get next object in tree, branching if this exists, i.e. is not 0. 
*/
void z_get_sibling(void)
{
    ushort sibling = objecttable_getObjectSibling(operands[0]);
    storeResult(sibling);
    branchTo(sibling != 0);
}

/*
1OP:133 5 inc (variable)

Increment variable by 1. (This is signed, so -1 increments to 0.) 
*/
void z_inc(void)
{
    short variableValue = 0;
    short variableNumber = 0;

    // constant gives variable to load
    variableNumber = operands[0];
    variableValue = loadVariable(variableNumber);

    // increment value and store back;
    storeVariable(variableNumber, ++variableValue);
}

/*
2OP:2 2 jl a b ?(label)

Jump if a < b (using a signed 16-bit comparison)
*/
void z_jl(void)
{
    branchTo(operands[0] < operands[1]);
}

/*
0OP:184 8 ret_popped

Pops top of stack and returns that. (This is equivalent to ret sp, but is one byte cheaper.) 
*/
void z_ret_popped(void)
{
    returnfromRoutine(loadVariable(0));
}

/*
VAR:228 4 1 sread text parse

4 sread text parse time routine
5 aread text parse time routine -> (result) 
*/
void z_read()
{
    printf("done");
    exit(1);
}

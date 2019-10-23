#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//sign extends a bitfield with given size
/* You may find implementing this function helpful */
int bitSigner(unsigned int field, unsigned int size){
    int negative = (field >> (size - 1)) != 0;
    if (negative) {
        int shift = ~0;
        int temp = ((1 << size) - 1) ^ shift;
        unsigned val = temp | field;
        return val;
    } else {
        return field;
    }
}

/* Remember that the offsets should return the offset in BYTES */

int get_branch_offset(Instruction instruction) {
    unsigned twelfth = instruction.sbtype.imm7 >> 6;
    unsigned sixRemain = instruction.sbtype.imm7 & 0x3F;
    unsigned eleventh = instruction.sbtype.imm5  & 1;
    unsigned fourRemain = instruction.sbtype.imm5 >> 1;
    unsigned finalBit = (twelfth << 12) | (eleventh << 11) | (sixRemain << 5) | (fourRemain << 1);
    
    return bitSigner(finalBit, 13);
}

int get_jump_offset(Instruction instruction) {
    unsigned twentyDigit = instruction.ujtype.imm >> 19;
    unsigned tenToOne = (instruction.ujtype.imm >> 9) & (0x3FF);
    unsigned elevenDigit = (instruction.ujtype.imm & 0x1FF) >> 7;
    unsigned nineteenToTwelve = instruction.ujtype.imm & 0xFF;
    unsigned finalBit = (twentyDigit << 20) | (nineteenToTwelve << 12) | (elevenDigit << 11) | (tenToOne << 1);
    
    return bitSigner(finalBit, 21);
}

int get_store_offset(Instruction instruction) {
    unsigned finalBit = (instruction.stype.imm7 << 5) | instruction.stype.imm5;
    
    return bitSigner(finalBit, 12);
}

void handle_invalid_instruction(Instruction instruction) {
    printf("Invalid Instruction: 0x%08x\n", instruction.bits); 
}

void handle_invalid_read(Address address) {
    printf("Bad Read. Address: 0x%08x\n", address);
    exit(0);
}

void handle_invalid_write(Address address) {
    printf("Bad Write. Address: 0x%08x\n", address);
    exit(0);
}


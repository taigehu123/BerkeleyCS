#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"
#include "riscv.h"

void execute_rtype(Instruction, Processor *);
void execute_itype_except_load(Instruction, Processor *);
void execute_branch(Instruction, Processor *);
void execute_jal(Instruction, Processor *);
void execute_load(Instruction, Processor *, Byte *);
void execute_store(Instruction, Processor *, Byte *);
void execute_ecall(Processor *, Byte *);
void execute_lui(Instruction, Processor *);

void execute_instruction(Instruction instruction,Processor *processor,Byte *memory) {    
    /* YOUR CODE HERE: COMPLETE THE SWITCH STATEMENTS */
    switch(instruction.opcode) { // What do we switch on?
        case 0x33:
            execute_rtype(instruction, processor);
            break;
        case 0x03:
            execute_load(instruction, processor, memory);
            break;
        case 0x13:
            execute_itype_except_load(instruction, processor);
            break;
        case 0x73:
            execute_ecall(processor, memory);
            break;
        case 0x23:
            execute_store(instruction, processor, memory);
            break;
        case 0x63:
            execute_branch(instruction, processor);
            break;
        case 0x37:
            execute_lui(instruction, processor);
            break;
        case 0x6f:
            execute_jal(instruction, processor);
            break;
        default: // undefined opcode
            handle_invalid_instruction(instruction);
            exit(0);
            break;
    }
}

void execute_rtype(Instruction instruction, Processor *processor) {
    processor->PC += 4;
    switch(instruction.rtype.funct3) { // What do we switch on?
        case 0x0:
            switch(instruction.rtype.funct7) {
                case 0x00:
                {
                    int val = (int)processor->R[instruction.rtype.rs1] + (int)processor->R[instruction.rtype.rs2];
                    processor->R[instruction.rtype.rd] = val;                  
                }
                break;
                case 0x01:
                {
                    long long val = (int)processor->R[instruction.rtype.rs1] * (int)processor->R[instruction.rtype.rs2];
                    int valTwo = val & 0xFFFFFFFF;
                    processor->R[instruction.rtype.rd] = valTwo;
                }
                break;
                case 0x20:
                {
                    int val = (int)processor->R[instruction.rtype.rs1] - (int)processor->R[instruction.rtype.rs2];
                    processor->R[instruction.rtype.rd] = val;
                }
                break;
                }
            break;
        case 0x1:
            switch(instruction.rtype.funct7) {
                case 0x00:
                {
                    int val = processor->R[instruction.rtype.rs1] << processor->R[instruction.rtype.rs2];
                    processor->R[instruction.rtype.rd] = val;
                }
                break;
                case 0x01:
                {
                    sDouble val = (processor->R[instruction.rtype.rs1] * processor->R[instruction.rtype.rs2]);
                    sWord newVal = val >> 32;
                    processor->R[instruction.rtype.rd] = newVal;
                }
                break;
                }
            break;
        case 0x2:
        {
            int val = ((int)processor->R[instruction.rtype.rs1] < (int)processor->R[instruction.rtype.rs2]) ? 1 : 0;
            processor->R[instruction.rtype.rd] = val;
        }
        break;
        case 0x4:
            switch(instruction.rtype.funct7) {
                case 0x00:
                {
                    int val = processor->R[instruction.rtype.rs1] ^ processor->R[instruction.rtype.rs2];
                    processor->R[instruction.rtype.rd] = val;

                }
                break;
                case 0x01:
                {
                    int val = (int)processor->R[instruction.rtype.rs1] / (int)processor->R[instruction.rtype.rs2];
                    processor->R[instruction.rtype.rd] = val;
                }
                break;
            }
            break;
        case 0x5:
            switch(instruction.rtype.funct7) {
                case 0x00: 
                {
                    Word val = processor->R[instruction.rtype.rs1];
                    unsigned val2 = processor->R[instruction.rtype.rs2];
                    Word shifted = val >> val2;
                    processor->R[instruction.rtype.rd] = shifted;
                    
                }
                break;
                case 0x20: 
                {
                    sWord val = (signed)processor->R[instruction.rtype.rs1];
                    unsigned val2 = processor->R[instruction.rtype.rs2];
                    sWord shifted = val >> val2;
                    processor->R[instruction.rtype.rd] = shifted;
                    
                }
                break;
            }
            break;
        case 0x6:
            switch(instruction.rtype.funct7) {
                case 0x00:
                {
                    int val = processor->R[instruction.rtype.rs1] | processor->R[instruction.rtype.rs2];
                    processor->R[instruction.rtype.rd] = val;
                }
                break;
                case 0x01:
                {
                    int val = processor->R[instruction.rtype.rs1] % processor->R[instruction.rtype.rs2];
                    processor->R[instruction.rtype.rd] = val;
                }
                break;
            }
        break;
        case 0x7:
        {
            int val = processor->R[instruction.rtype.rs1] & processor->R[instruction.rtype.rs2];
            processor->R[instruction.rtype.rd] = val;
        }
        break;
        default:
            handle_invalid_instruction(instruction);
            exit(0);
            break;
    }
}

void execute_itype_except_load(Instruction instruction, Processor *processor) {
    int shiftOp;
    shiftOp = -1;
    processor->PC += 4;
    switch(instruction.itype.funct3) { // What do we switch on?
        case 0x0: 
        {
            int val = (int)processor->R[instruction.itype.rs1] + bitSigner(instruction.itype.imm, 12);
            processor->R[instruction.itype.rd] = val;
        }
        break;
        case 0x1: 
        {
            int val = processor->R[instruction.itype.rs1] << bitSigner(instruction.itype.imm, 12);
            processor->R[instruction.itype.rd] = val;
        }
        break;
        case 0x2: 
        {
            int val = (((int)processor->R[instruction.itype.rs1]) < bitSigner(instruction.itype.imm, 12)) ? 1 : 0;
            processor->R[instruction.itype.rd] = val;
        }
        break;
        case 0x4: 
        {
            int val = processor->R[instruction.itype.rs1] ^ bitSigner(instruction.itype.imm, 12);
            processor->R[instruction.itype.rd] = val;     
        }
        break;
        case 0x5: 
        {
            switch((instruction.itype.imm & 0xFE0) >> 5) {
                case 0x20:
                {
                    signed val = (signed)processor->R[instruction.itype.rs1];
                    int val2 = bitSigner(instruction.itype.imm & 0x01F, 5);
                    signed shifted = val >> val2;
                    processor->R[instruction.rtype.rd] = shifted;
                }
                break;
                case 0x00:
                {
                    unsigned val = processor->R[instruction.itype.rs1];
                    int val2 = bitSigner(instruction.itype.imm & 0x01F, 5);
                    unsigned shifted = val >> val2;
                    processor->R[instruction.rtype.rd] = shifted;
                }
                break;
            }
        }
        break;
        case 0x6: 
        {
            int val = processor->R[instruction.itype.rs1] | bitSigner(instruction.itype.imm, 12);
            processor->R[instruction.itype.rd] = val;    
        }
        break;
        case 0x7: 
        {
            int val = processor->R[instruction.itype.rs1] & bitSigner(instruction.itype.imm, 12);
            processor->R[instruction.itype.rd] = val;
        }
        break;
        default:
            handle_invalid_instruction(instruction);
            exit(0);
            break;
    }
}

void execute_ecall(Processor *p, Byte *memory) {
    switch(p->R[10]) { // What do we switch on?
        case 0x00000001:
            printf("%d", p->R[11]);
            p->PC += 4;
            break;
        case 0x0000000a:
            printf("%s\n", "exiting the simulator");
            p->PC += 4;
            exit(0);
            break;
        default: // undefined ecall
            printf("Illegal ecall number %d\n", -1); // What stores the ecall arg?
            exit(0);
            break;
    }
}

void execute_branch(Instruction instruction, Processor *processor) {
    int branchaddr;
    branchaddr = 0;
    /* Remember that the immediate portion of branches
       is counting half-words, so make sure to account for that. */
    switch(instruction.sbtype.funct3) { // What do we switch on?
        case 0x0:
            if (processor->R[instruction.sbtype.rs1] == processor->R[instruction.sbtype.rs2]) {
                processor->PC += get_branch_offset(instruction);
            } else {
                processor->PC += 4;
            }
            break;
        case 0x1:
            if (processor->R[instruction.sbtype.rs1] != processor->R[instruction.sbtype.rs2]) {
                processor->PC += get_branch_offset(instruction);
            } else {
                processor->PC += 4;
            }
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(0);
            break;
    }
}

void execute_load(Instruction instruction, Processor *processor, Byte *memory) {
    processor->PC += 4;
    unsigned addr = (unsigned) processor->R[instruction.itype.rs1];
    switch(instruction.itype.funct3) { // What do we switch on?
        case 0x0:
        {
            int val = bitSigner(load(memory, addr + bitSigner(instruction.itype.imm, 12), LENGTH_BYTE, 0), 8);
            processor->R[instruction.itype.rd] = val;
        }
        break;
        case 0x1:
        {
            int val = bitSigner(load(memory, addr + bitSigner(instruction.itype.imm, 12), LENGTH_HALF_WORD, 0), 16);
            processor->R[instruction.itype.rd] = val;   
        }
        break;
        case 0x2:
        {
            int val = load(memory, addr + instruction.itype.imm, LENGTH_WORD, 0);
            processor->R[instruction.itype.rd] = val;
       }

        break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
    
}

void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
    processor->PC += 4;
    unsigned addr = (unsigned) processor->R[instruction.stype.rs1];
    switch(instruction.stype.funct3) { // What do we switch on?
        case 0x0:
        {
            int val = processor->R[instruction.stype.rs2] & 0x000000FF;
            store(memory, addr + get_store_offset(instruction), LENGTH_BYTE, val, 0);
        }
        break;
        case 0x1:
        {
            int val = processor->R[instruction.stype.rs2] & 0x0000FFFF;
            store(memory, addr + get_store_offset(instruction), LENGTH_HALF_WORD, val, 0);
        }
        break;
        case 0x2:
        {
            int val = processor->R[instruction.stype.rs2];
            store(memory, addr + get_store_offset(instruction), LENGTH_WORD, val, 0); 
        }
        break;
        default:
            handle_invalid_instruction(instruction);
            exit(0);
            break;
    }
    
}

void execute_jal(Instruction instruction, Processor *processor) {
    int nextPC;
    nextPC = get_jump_offset(instruction);
    processor->R[instruction.ujtype.rd] = processor->PC + 4;
    processor->PC += nextPC;
}

void execute_lui(Instruction instruction, Processor *processor) {
    int imm;
    imm = 0;
    int val = instruction.utype.imm;
    val = val << 12;
    processor->R[instruction.utype.rd] = val;
    processor->PC = processor->PC + 4;
}

/* Checks that the address is aligned correctly */
int check(Address address,Alignment alignment) {
    if(address>0 && address < MEMORY_SPACE){
        if(alignment == LENGTH_BYTE){
            return 1;
        }
        else if( alignment == LENGTH_HALF_WORD ){
            return address%2 == 0;
        }
        else if (alignment == LENGTH_WORD){
            return address%4 ==0;
        }
    }
    return 0;
}

void store(Byte *memory,Address address,Alignment alignment,Word value, int check_align) {
    if((check_align && !check(address,alignment)) || (address >= MEMORY_SPACE)) {
        handle_invalid_write(address);
    }
    if (alignment == LENGTH_BYTE) {
        memory[address] = (value & 0xFF);
    } else if (alignment == LENGTH_HALF_WORD) {
        memory[address] = (value & 0xFF);
        memory[address + 1] = (value & 0xFF00) >> 8;
    } else {
        memory[address] = (value & 0xFF);
        memory[address + 1] = (value & 0xFF00) >> 8;
        memory[address + 2] = (value & 0xFF0000) >> 16;
        memory[address + 3] = (value & 0xFF000000) >> 24;
    }
}

Word load(Byte *memory,Address address,Alignment alignment, int check_align) {
    if((check_align && !check(address,alignment)) || (address >= MEMORY_SPACE)) {
        handle_invalid_read(address);
    }
    // int counter;
    uint32_t toReturn;
    if (alignment == LENGTH_BYTE) {
        toReturn = bitSigner((memory[address] & 0xFF), 8);
    } else if (alignment == LENGTH_HALF_WORD) {
        toReturn = bitSigner((memory[address] & 0xFF) + ((memory[address + 1]) << 8), 16);
    } else {
        toReturn = memory[address] & 0xFF;
        toReturn = toReturn + ((memory[address + 1]) << 8);
        toReturn = toReturn + ((memory[address + 2]) << 16);
        toReturn = toReturn + ((memory[address + 3]) << 24);
    }
    return toReturn;
}







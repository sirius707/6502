#include "cpu.h"
#include <stdio.h>

void set_carry(int8_t bit);
void set_zero(int8_t bit);

void print_dbg_info();
/**********************sample micro instructions *****************/
void microA()
{
    puts("hi1");
}
void microB()
{
    puts("hi2");
}
void microC()
{
    puts("hi3");
}
void microD()
{
    puts("hi4");
}
/******************************************************************/

void mis_fetch_immediate_value()
{
    fetched_value = rom[++PC];
}

void mis_add_fval_accumlator()
{
    set_carry((fetched_value + AC) > 0xff);
    AC += fetched_value;
    set_zero(AC);
}

void cpu_init(int8_t *memory, size_t n)
{
    cycle_count = 0;

    cpu_memory = memory;
    memory_size = n;

    PC = SR = XR = YR = AC = SP = 0;

    fetched_address = 4;
    fetched_value = 4;

    for(int i = 0; i < N_INSTRUCTS; i++){
        for(int j = 0; j < INSTRUCT_LEN; j++){
            instructions[i].mi_indecies[j] = -1;
        }
    }

    for(int i = 0; i < N_MI; i++){
        micro_instructions[i] = NULL;
    }

    mipc = 0;
    extra = -1;
    fetched_instruction = -1;

    micro_instructions[0] = microA;
    micro_instructions[1] = microB;
    micro_instructions[2] = microC;
    micro_instructions[3] = microD;

    micro_instructions[4] = mis_fetch_immediate_value;
    micro_instructions[5] = mis_add_fval_accumlator;

    instructions[1].mi_indecies[0] = 0;
    instructions[1].mi_indecies[1] = 1;
    instructions[1].mi_indecies[2] = 2;
    instructions[1].mi_indecies[3] = 3;

    instructions[2].mi_indecies[0] = 3;
    instructions[2].mi_indecies[1] = 2;
    instructions[2].mi_indecies[2] = 1;
    instructions[2].mi_indecies[3] = 0;

    instructions[69].mi_indecies[0] = 4;
    instructions[69].mi_indecies[1] = 5;


    rom[0] = 69;
    rom[1] = 5;

}

//run a micro instruction
void execute_micro()
{
    if(extra != -1){//to simulate an extra cycle, required when page boundaries are crossed
        (micro_instructions[extra])();
    }else if(instructions[fetched_instruction].mi_indecies[mipc] != -1){
        int ind = instructions[fetched_instruction].mi_indecies[mipc];
        (micro_instructions[ind])();
         mipc++;
    }
    cycle_count++;
}


void cpu_cycle()
{
    INSTRUCTION *instr_ptr;

    if(fetched_instruction == -1){
        fetched_instruction = rom[PC];
        instr_ptr = &instructions[fetched_instruction];
    }

    execute_micro();

    if(instr_ptr->mi_indecies[mipc] == -1){
        mipc = 0;
        fetched_address = 0;
        fetched_value = AC;
        fetched_instruction = -1;
        PC++;
    }
}

void set_carry(int8_t bit)
{
    if(bit){
        SR |= 0x01;
    }else{
        SR &= 0xFE;
    }
}

void set_zero(int8_t bit)
{
    if(bit){
        SR |= 0x02;
    }else{
        SR &= 0xFD;
    }
}

void print_dbg_info()
{
    printf("SR %x, AC = %x, PC = %x,\n", SR, AC, PC);
}

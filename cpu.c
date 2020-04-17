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
            instructions[i].mis_pointers[j] = NULL;
        }
    }

    for(int i = 0; i < N_MI; i++){
        micro_instructions[i] = NULL;
    }

    mipc = 0;
    extra_ptr = NULL;
    fetched_instruction = -1;

    instructions[69].mis_pointers[0] = mis_fetch_immediate_value;
    instructions[69].mis_pointers[1] = mis_add_fval_accumlator;


    rom[0] = 69;
    rom[1] = 5;

}

//run a micro instruction
void execute_micro()
{
    if(extra_ptr != NULL){//to simulate an extra cycle, required when page boundaries are crossed
        (*extra_ptr)();
    }else if(instructions[fetched_instruction].mis_pointers[mipc] != NULL){
        (instructions[fetched_instruction].mis_pointers[mipc])();
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

    if(instr_ptr->mis_pointers[mipc] == NULL){
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

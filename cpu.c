#include "cpu.h"
#include <stdio.h>


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
    rom[1] = 255;
    rom[2] = 69;
    rom[3] = 124;


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


//AUx functions
inline void set_carry(uint16_t bit)
{
   if(bit){
        SR |= 0x01;
    }else{
        SR &= 0xFE;
    }
}

inline uint8_t get_carry()
{
    if(SR & 0xFE)return 1;
    return 0;
}


inline void set_zero(uint16_t bit)
{
    if(bit){
        SR |= 0x02;
    }else{
        SR &= 0xFD;
    }
}

inline void set_negative(uint16_t bit)
{
    if(bit){
        SR |= 0x80;
    }else{
        SR &= 0x7F;
    }
}

inline void set_overflow(uint16_t bit)
{
    if(bit){
        SR |= 0x40;
    }else{
        SR &= 0xBF;
    }
}

inline bool is_ac_negative()
{
    return AC & 0x80;
}
void print_dbg_info()
{
    printf("SR 0x%x, AC = 0x%x, PC = 0x%x,\n", SR, AC, PC);
}


/**********************micro instructions***********************************/

void fodder_op()//filler function
{

}

void mis_fetch_immediate_value()
{
    fetched_value = rom[++PC];
}

//ADC
void mis_add_fval_accumlator()
{
    bool negative = is_ac_negative();
    uint16_t tmp = AC + fetched_value + (SR&0x01);
    set_carry( tmp > 0xff);

    AC = tmp;

    set_zero(!AC);
    set_negative(AC&0x80);

    set_overflow(!(SR&0x01) && negative != is_ac_negative());

}

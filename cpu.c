#include "cpu.h"
#include <stdio.h>

#define CARRY 1
#define ERO   2
#define INTRR 3
#define BREAK 5
#define OVERF 7
#define NEGAT 8

void cpu_init(int8_t *memory, size_t n)
{
    cycle_count = 0;

    cpu_memory = memory;
    memory_size = n;

    PC = SR = XR = YR = AC = SP = 0;
    set_brk(1);

    fetched_low_address = 4;
    fetched_high_address = 4;
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

    //ADC
    instructions[0x0069].mis_pointers[0] = mis_fetch_immediate_value;
    instructions[0x0069].mis_pointers[1] = mis_add_fval_accumlator;

    instructions[0x0065].mis_pointers[0] = mis_fetch_lowbyte_address;
    instructions[0x0065].mis_pointers[1] = mis_fetch_value_lowbyte_address;
    instructions[0x0065].mis_pointers[2] = mis_add_fval_accumlator;

    instructions[0x0075].mis_pointers[0] = mis_fetch_lowbyte_address;
    instructions[0x0075].mis_pointers[1] = mis_add_x_lowbyte_address;
    instructions[0x0075].mis_pointers[2] = mis_fetch_value_lowbyte_address;;
    instructions[0x0075].mis_pointers[3] = mis_add_fval_accumlator;

    instructions[0x006D].mis_pointers[0] = mis_fetch_lowbyte_address;
    instructions[0x006D].mis_pointers[1] = mis_fetch_highbyte_address;
    instructions[0x006D].mis_pointers[2] = mis_fetch_value_big_address;
    instructions[0x006D].mis_pointers[3] = mis_add_fval_accumlator;

    instructions[0x007D].mis_pointers[0] = mis_fetch_lowbyte_address;
    instructions[0x007D].mis_pointers[1] = mis_fetch_highbyte_address;
    instructions[0x007D].mis_pointers[2] = mis_fetch_value_big_address_with_x;
    instructions[0x007D].mis_pointers[3] = mis_add_fval_accumlator;

    instructions[0x0076].mis_pointers[0] = mis_fetch_lowbyte_address;
    instructions[0x0076].mis_pointers[1] = mis_fetch_highbyte_address;
    instructions[0x0076].mis_pointers[2] = mis_fetch_value_big_address_with_y;
    instructions[0x0076].mis_pointers[3] = mis_add_fval_accumlator;

    //STA
    instructions[0x0085].mis_pointers[0] = mis_fetch_lowbyte_address;
    instructions[0x0085].mis_pointers[1] = mis_set_lowbyte_destination;
    instructions[0x0085].mis_pointers[2] = mis_store_ac_in_low_destination;

    //LDx
    instructions[0x00A2].mis_pointers[0] = mis_fetch_immediate_value;
    instructions[0x00A2].mis_pointers[1] = mis_load_xr_fval;


    rom[0] = 0x0069;
    rom[1] = 2;
    rom[2] = 0x0085;
    rom[3] = 35;
    rom[4] = 0x00A2;
    rom[5] = 33;
    rom[6] = 0x007D;
    rom[7] = 2;


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
        fetched_low_address = 0;
        fetched_high_address = 0;
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

inline void set_brk(uint16_t bit)
{
    if(bit){
        SR |= 0x10;
    }else{
        SR &= 0xEF;
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
    printf("SR 0x%x, AC = 0x%x, PC = 0x%x, xr = %d\n", SR, AC, PC, XR);

}


/**********************micro instructions***********************************/

void fodder_op()//filler function
{

}

void mis_fetch_immediate_value()
{
    fetched_value = rom[++PC];
}

void mis_fetch_lowbyte_address()
{
    fetched_low_address = rom[++PC];//should the rom and the memory be the same array? stay tuned
}

void mis_fetch_highbyte_address()
{
    fetched_high_address = rom[++PC];//should the rom and the memory be the same array? stay tuned
}


void mis_fetch_value_lowbyte_address()
{
    fetched_value = rom[fetched_low_address];
}

void mis_fetch_value_big_address()
{
    uint16_t big_address = 0;
    big_address |= fetched_high_address << 4;
    big_address |= fetched_low_address;

    fetched_value = rom[big_address];
}

void mis_fetch_value_big_address_with_x()
{
    fetched_low_address += XR;

    if(fetched_low_address < XR){
        fetched_high_address += 1;
        extra_ptr = mis_fetch_value_big_address;
        return;
    }

    uint16_t big_address = 0;
    big_address |= fetched_high_address << 4;
    big_address |= fetched_low_address;

    fetched_value = rom[big_address];
}

void mis_fetch_value_big_address_with_y()
{
    fetched_low_address += YR;

    if(fetched_low_address < YR){
        fetched_high_address += 1;
        extra_ptr = mis_fetch_value_big_address;
        return;
    }

    uint16_t big_address = 0;
    big_address |= fetched_high_address << 4;
    big_address |= fetched_low_address;

    fetched_value = rom[big_address];
}

void mis_add_x_lowbyte_address()
{
    fetched_low_address += XR;
}

void mis_set_lowbyte_destination()
{
    low_byte_destination = fetched_low_address;
}

//sta
void mis_store_ac_in_low_destination()
{
    rom[low_byte_destination] = AC;
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

    set_overflow(!(SR&CARRY) && negative != is_ac_negative());

}

//ldx
void mis_load_xr_fval()
{
    XR = fetched_value;
}


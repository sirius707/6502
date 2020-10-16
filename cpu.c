#include "cpu.h"
#include <stdio.h>

#define CARRY 1
#define ERO   2
#define INTRR 3
#define BREAK 5
#define OVERF 7
#define NEGAT 8

void cpu_init(uint8_t *memory, size_t n)
{
    cycle_count = 0;

    //cpu_memory = memory;
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

    //for(int i = 0; i < N_ROM; i++)memory[i] = END_OF_ROM;
    if(memory != NULL){
        for(int i = 0; i < memory_size; i++){
                memory[i] = 0;
        }
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

    instructions[0x0061].mis_pointers[0] = mis_fetch_lowbyte_address;
    instructions[0x0061].mis_pointers[1] = mis_add_x_lowbyte_address;
    instructions[0x0061].mis_pointers[2] = mis_indirect_x;
    instructions[0x0061].mis_pointers[3] = mis_fetch_value_big_address;
    instructions[0x0061].mis_pointers[4] = mis_add_fval_accumlator;


    //STA
    instructions[0x0085].mis_pointers[0] = mis_fetch_lowbyte_address;
    instructions[0x0085].mis_pointers[1] = mis_set_lowbyte_destination;
    instructions[0x0085].mis_pointers[2] = mis_store_ac_in_low_destination;

    instructions[0x008D].mis_pointers[0] = mis_fetch_lowbyte_address;
    instructions[0x008D].mis_pointers[1] = mis_fetch_highbyte_address;
    instructions[0x008D].mis_pointers[2] = mis_set_highbyte_destination;
    instructions[0x008D].mis_pointers[3] = mis_store_ac_in_big_destination;

    //STX
    instructions[0x0086].mis_pointers[0] = mis_fetch_lowbyte_address;
    instructions[0x0086].mis_pointers[1] = mis_set_lowbyte_destination;
    instructions[0x0086].mis_pointers[2] = mis_store_x_in_low_destination;

    instructions[0x0096].mis_pointers[0] = mis_fetch_lowbyte_address;
    instructions[0x0096].mis_pointers[1] = mis_add_y_lowbyte_address;
    instructions[0x0096].mis_pointers[2] = mis_set_highbyte_destination;
    instructions[0x0096].mis_pointers[3] = mis_store_x_in_low_destination;

    instructions[0x008E].mis_pointers[0] = mis_fetch_lowbyte_address;
    instructions[0x008E].mis_pointers[1] = mis_fetch_highbyte_address;
    instructions[0x008E].mis_pointers[2] = mis_set_highbyte_destination;
    instructions[0x008E].mis_pointers[3] = mis_store_x_in_big_destination;

    //STY
    instructions[0x0084].mis_pointers[0] = mis_fetch_lowbyte_address;
    instructions[0x0084].mis_pointers[1] = mis_set_lowbyte_destination;
    instructions[0x0084].mis_pointers[2] = mis_store_y_in_low_destination;

    instructions[0x0094].mis_pointers[0] = mis_fetch_lowbyte_address;
    instructions[0x0094].mis_pointers[1] = mis_add_x_lowbyte_address;
    instructions[0x0094].mis_pointers[2] = mis_set_highbyte_destination;
    instructions[0x0094].mis_pointers[3] = mis_store_y_in_low_destination;

    instructions[0x008C].mis_pointers[0] = mis_fetch_lowbyte_address;
    instructions[0x008C].mis_pointers[1] = mis_fetch_highbyte_address;
    instructions[0x008C].mis_pointers[2] = mis_set_highbyte_destination;
    instructions[0x008C].mis_pointers[3] = mis_store_y_in_big_destination;

    //LDx
    instructions[0x00A2].mis_pointers[0] = mis_fetch_immediate_value;
    instructions[0x00A2].mis_pointers[1] = mis_load_xr_fval;

    //TAX
    instructions[0x00AA].mis_pointers[0] = mis_fetch_value_from_a;
    instructions[0x00AA].mis_pointers[1] = mis_store_fetched_x;

    //TAY
    instructions[0x00A8].mis_pointers[0] = mis_fetch_value_from_a;
    instructions[0x00A8].mis_pointers[1] = mis_store_fetched_y;

    //TSX
    instructions[0x00BA].mis_pointers[0] = mis_fetch_value_from_sp;
    instructions[0x00BA].mis_pointers[1] = mis_store_fetched_x;



    memory[0] = 0x69;
    memory[1] = 0x1;
    memory[2] = 0x00AA;
    memory[3] = 0x86;
    memory[4] = 0x22;//address
    memory[5] = END_OF_ROM;


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


bool cpu_cycle()
{
    INSTRUCTION *instr_ptr;

    if(fetched_instruction == -1){
        fetched_instruction = memory[PC];

        if(fetched_instruction == END_OF_ROM){
                return false;
        }

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

    return true;
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

//test cpu
//return: number of cycles
int play()
{
    int i = 0;
    while(cpu_cycle())i++;

    return i;
}

/**********************micro instructions***********************************/

void fodder_op()//filler function
{

}

void mis_fetch_immediate_value()
{
    fetched_value = memory[++PC];
}

void mis_fetch_lowbyte_address()
{
    fetched_low_address = memory[++PC];//should the memory and the memory be the same array? stay tuned
}

void mis_fetch_highbyte_address()
{
    fetched_high_address = memory[++PC];//should the memory and the memory be the same array? stay tuned
}


void mis_fetch_value_lowbyte_address()
{
    fetched_value = memory[fetched_low_address];
}

void mis_fetch_value_big_address()
{
    uint16_t big_address = 0;
    big_address |= fetched_high_address << 8;
    big_address |= fetched_low_address;

    fetched_value = memory[big_address];

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
    big_address |= fetched_high_address << 8;
    big_address |= fetched_low_address;

    fetched_value = memory[big_address];
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
    big_address |= fetched_high_address << 8;
    big_address |= fetched_low_address;

    fetched_value = memory[big_address];
}

void mis_add_x_lowbyte_address()
{
    fetched_low_address += XR;
}


void mis_add_y_lowbyte_address()
{
    fetched_low_address += YR;
}

void mis_set_lowbyte_destination()
{
    low_byte_destination = fetched_low_address;
}

void mis_set_highbyte_destination()
{
    low_byte_destination = fetched_low_address;
    high_byte_destination = fetched_high_address;
}

void mis_indirect_x()
{
    fetched_high_address = fetched_low_address + 1;
}

void mis_fetch_indirect_address_from_fetched_big_address()
{
    uint16_t big_address = 0;
    uint8_t effective_high_address = 0;
    uint8_t effective_low_address = 0;

    big_address |= fetched_high_address << 8;
    big_address |= fetched_low_address;

    effective_high_address = memory[big_address] >> 8;
    effective_low_address = memory[big_address];

    fetched_high_address = effective_high_address;
    fetched_low_address = effective_low_address;
}

//sta
void mis_store_ac_in_low_destination()
{
    memory[low_byte_destination] = AC;
}

void mis_store_x_in_low_destination()
{
    memory[low_byte_destination] = XR;
}

void mis_store_y_in_low_destination()
{
    memory[low_byte_destination] = YR;
}

void mis_store_ac_in_big_destination()
{
     uint16_t big_address = 0;
     big_address |= high_byte_destination << 8;
     big_address |= low_byte_destination;

     memory[big_address] = AC;
}
void mis_store_x_in_big_destination()
{
     uint16_t big_address = 0;
     big_address |= high_byte_destination << 8;
     big_address |= low_byte_destination;

     memory[big_address] = XR;
}

void mis_store_y_in_big_destination()
{
     uint16_t big_address = 0;
     big_address |= high_byte_destination << 8;
     big_address |= low_byte_destination;

     memory[big_address] = YR;
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

void mis_fetch_value_from_a()
{
    fetched_high_address = AC;
}

void mis_fetch_value_from_sp()
{
    fetched_value = SP;
}

void mis_store_fetched_x()
{
    XR = fetched_value;
}

void mis_store_fetched_y()
{
    YR = fetched_value;
}

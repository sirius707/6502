#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>

#define END_OF_ROM 0xFE

#define N_MI 4 //number of micro instructions
#define N_INSTRUCTS 300 //number of instructions

#define INSTRUCT_LEN 5//how many mi's an instruction has

#define N_ROM 500//rom size

typedef struct INSTRUCTION INSTRUCTION;
struct INSTRUCTION{
   void (*mis_pointers[INSTRUCT_LEN])();
};

//ram
static int8_t *stk;
static int8_t *cpu_memory;
static size_t memory_size;

uint8_t *memory;//memory pointer, assign in init

//registers
uint16_t PC;
uint8_t SR;//status flags
uint8_t XR;
uint8_t YR;
uint8_t AC;
uint8_t SP;//stack pointer

size_t cycle_count;
static int fetched_instruction; // instruction to excute next
INSTRUCTION instructions[N_INSTRUCTS];

void cpu_init(uint8_t *ram, size_t n);
bool cpu_cycle();


//micro instruction control
static uint16_t mipc;//program counter for micro instruction
static uint8_t fetched_high_address;
static uint8_t fetched_low_address;
static uint8_t  fetched_value;
static uint8_t low_byte_destination;
static uint8_t high_byte_destination;

static void (*extra_ptr)();//index of an ekstra micro instruction
void (*micro_instructions[N_MI])();

void execute_micro();
int cpu_play();

///////micro instructions////
void mis_fetch_immediate_value();
void mis_add_fval_accumlator();
void mis_fetch_value_lowbyte_address();
void mis_fetch_value_big_address();
void mis_fetch_value_big_address_with_x();
void mis_fetch_value_big_address_with_y();
void mis_fetch_value_from_a();
void mis_fetch_value_from_sp();


void mis_fetch_lowbyte_address();
void mis_fetch_highbyte_address();
void mis_fetch_indirect_address_from_fetched_big_address();
void mis_set_lowbyte_destination();
void mis_store_ac_in_low_destination();
void mis_store_ac_in_big_destination();
void mis_store_x_in_low_destination();
void mis_store_x_in_big_destination();
void mis_store_y_in_low_destination();
void mis_store_y_in_big_destination();
void mis_add_x_lowbyte_address();
void mis_add_y_lowbyte_address();
void mis_load_xr_fval();
void mis_set_highbyte_destination();


void mis_store_fetched_x();
void mis_store_fetched_y();

void mis_indirect_x();
////aux//////////////
void print_dbg_info();
void set_carry(uint16_t bit);
void set_zero(uint16_t bit);
void set_brk(uint16_t bit);
void set_negative(uint16_t bit);
void set_overflow(uint16_t bit);
uint8_t get_carry();
bool is_ac_negative();

#endif // CPU_H_INCLUDED

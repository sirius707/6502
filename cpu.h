#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>

#define N_MI 4 //number of micro instructions
#define N_INSTRUCTS 100 //number of instructions

#define INSTRUCT_LEN 5//how many mi's an instruction has

typedef struct INSTRUCTION INSTRUCTION;
struct INSTRUCTION{
   void (*mis_pointers[INSTRUCT_LEN])();
};

//ram
static int8_t *stk;
static int8_t *cpu_memory;
static size_t memory_size;
#define N_ROM 20//rom size
int8_t rom[N_ROM];

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

void cpu_init(int8_t *memory, size_t n);
void cpu_cycle();


//micro instruction control
static uint16_t mipc;//program counter for micro instruction
static uint16_t fetched_address;
static uint8_t  fetched_value;

static void (*extra_ptr)();//index of an ekstra micro instruction
void (*micro_instructions[N_MI])();

void execute_micro();


///////micro instructions////
void mis_fetch_immediate_value();
void mis_add_fval_accumlator();

////aux//////////////
void print_dbg_info();
void set_carry(uint16_t bit);
void set_zero(uint16_t bit);
void set_negative(uint16_t bit);
void set_overflow(uint16_t bit);
uint8_t get_carry();
bool is_ac_negative();

#endif // CPU_H_INCLUDED

#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <inttypes.h>
#include <stdlib.h>

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

//registers
static int16_t PC;
static int8_t SR;//status flags
static int8_t XR;
static int8_t YR;
static int8_t AC;
static int8_t SP;//stack pointer


#define N_ROM 20//rom size
int8_t rom[N_ROM];

size_t cycle_count;

static int fetched_instruction; // instruction to excute next

INSTRUCTION instructions[N_INSTRUCTS];

void cpu_init(int8_t *memory, size_t n);
void cpu_cycle();


//micro instruction control
static int16_t mipc;//program counter for micro instruction
static int16_t fetched_address;
static int8_t  fetched_value;
static void (*extra_ptr)();//index of an ekstra micro instruction
void (*micro_instructions[N_MI])();

void execute_micro();

#endif // CPU_H_INCLUDED

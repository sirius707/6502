#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

#define RAM_SIZE 1024
uint8_t ram[RAM_SIZE];

int main()
{
    cpu_init(ram, RAM_SIZE);

    //printf("%d %x\n", play(), ram[0x22]);
    cpu_cycle();
    cpu_cycle();
    cpu_cycle();
    cpu_cycle();
    cpu_cycle();
    cpu_cycle();
    cpu_cycle();
    print_dbg_info();

    return 0;
}

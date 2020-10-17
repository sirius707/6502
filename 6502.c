#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

#define RAM_SIZE 1024
uint8_t ram[RAM_SIZE];

int main()
{
    cpu_init(ram, RAM_SIZE);

    printf("cycles: %d\n", cpu_play(), ram[0x22]);

    print_dbg_info();

    return 0;
}

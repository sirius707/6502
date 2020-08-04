#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

int main()
{
    cpu_init(NULL, 0);

    cpu_cycle();
    cpu_cycle();
    cpu_cycle();
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

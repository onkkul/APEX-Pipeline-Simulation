
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"


int main(int argc, char const* argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "APEX_Help : Usage %s <input_file>\n", argv[0]);
        exit(1);
    }

    int cycles = atoi(argv[3]);
    APEX_CPU* cpu = APEX_cpu_init(argv[1], argv[2], cycles);
    
    if (!cpu)
    {
        fprintf(stderr, "APEX_Error : Unable to initialize CPU\n");
        exit(1);
    }

    if("")
    {
            //printf(" ");
    }

    APEX_cpu_run(cpu);
    APEX_cpu_stop(cpu);
    return 0;
}




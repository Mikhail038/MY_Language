#include <stdio.h>
#include <stdlib.h>

#include "proc.h"
#include <string.h>

int main (int argc, char** argv)
{
    FILE* Bin = argc > 2 ? fopen (argv[2], "r") : fopen ("EXAMPLES_MC/code.mc", "r");
    MCA (Bin != NULL, 1);

    StructCPU CPU = {};

    if ((argc > 1) && (strcmp (argv[1], "-g") == 0))
    {
        CPU.mode = DBG_mode;
    }
    else
    {
        CPU.mode = NRM_mode;
    }

    cpu_constructor (Bin, &CPU);

    fclose (Bin);

    execute_code (&CPU);

    cpu_destructor (&CPU);

    return 0;
}

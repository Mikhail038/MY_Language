#include <stdio.h>
#include <stdlib.h>

#include "disasm.h"




int main (int argc, char** argv)
{
    FILE* Bin = argc > 1 ? fopen (argv[1], "rb") : fopen ("EXAMPLES_MC/code.mc", "r");
    MCA (Bin != NULL, 1);

    StructMachineCode Code = {};

    int VramSize = 0;
    check_passport (Bin, &(Code.size), &VramSize);

    read_array_of_code (Bin, &Code);

    fclose (Bin);

    StructDisasm Array = {};

    Array.Buffer = (unsigned char*) calloc (Code.size * TEMPORARY_CONST_4, sizeof (*Array.Buffer));

    make_text_from_code (&Array, &Code);

    FILE* Text = argc > 2 ? fopen (argv[2], "w") : fopen ("EXAMPLES_DISASM/code.disasm", "w");
    MCA (Text != NULL, 1);


    print_text_in_file (Text, &Array);

    fclose (Text);

    free (Code.ArrCode);

    free (Array.Buffer);

    return 0;
}

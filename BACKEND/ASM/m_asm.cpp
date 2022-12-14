#include <stdio.h>
#include <stdlib.h>

#include "asm.h"


int main (int argc, char** argv)
{
    FILE* SourceFile = argc > 1 ? fopen (argv[1], "r") : fopen ("EXAMPLES_ASM/code.asm", "rb");
    MCA (SourceFile != NULL, 1);

    StructSource Source = {};

    int Amnt_lines = count_lines_in_file (SourceFile);
    Source.amnt_symbols = count_symbols_in_file (SourceFile);
    int MaxSize = Amnt_lines + Amnt_lines * 2 * sizeof (double);

    MCA (Amnt_lines >= 0, 1);
    Source.Buffer = (char*) calloc (Source.amnt_symbols, sizeof (*Source.Buffer));
    MCA (Source.Buffer != NULL, 0);

    fread (Source.Buffer, sizeof (*Source.Buffer), Source.amnt_symbols, SourceFile);
    Source.Buffer[Source.amnt_symbols - 1] = '\0';

    fclose (SourceFile);

    //printf ("+%s+\n", Source.Buffer);
    //printf ("AmntSymbols %d AmntLines %d\n", Source.amnt_symbols, Amnt_lines);

    StructMachineCode Code = {};

    Code.ArrCode = (unsigned char*) calloc (1, MaxSize);

    Code.ip = 0;

    const char* Filename = argc > 3 ? argv[3] : "EXAMPLES_LOGS/code.log";

    make_array_of_code (Amnt_lines, &Source, &Code, Filename);

    free (Source.Buffer);


    FILE* Bin = argc > 2 ? fopen (argv[2], "wb") : fopen ("EXAMPLES_MC/code.mc", "wb");
    MCA (Bin != NULL, 1);

    make_bin_file (Bin, &Code);

    fclose (Bin);

    free (Code.ArrCode);

    return 0;
}

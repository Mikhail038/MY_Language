//==================================================================================================================================================================
//29.04.2023
//==================================================================================================================================================================

#include <cstddef>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <locale.h>
#include <type_traits>
#include <wchar.h>

//=============================================================================================================================================================================


#include "FLAG_DETECTOR/flag_detector.h"
#include "MYassert.h"
#include "flag_detector.h"

//=============================================================================================================================================================================

#include <errno.h>

#include "elf_back.h"

#include "back.h"
#include "front.h"


#include "asm.h"
#include "proc.h"

//=============================================================================================================================================================================

int main (int argc, char** argv)
{
    setlocale(LC_CTYPE, "");

    ElfBack Back;

    const ElfHead StandardElfHead = {};
    construct_elf_back(argc, argv, &Back, &StandardElfHead);

    if (Back.exit_marker == true)
    {
        destruct_elf_back (&Back);
        return 0;
    }

    make_elf_file (&Back);
    destruct_elf_back (&Back);

    size_t length = strlen (Back.ex_file_name) + sizeof ("chmod +rwx ");

    char* Command = (char*) calloc (sizeof (char), length);

    sprintf (Command, "chmod +rwx %s", Back.ex_file_name);
    system (Command);

    free (Command);

    return 0;
}

//=============================================================================================================================================================================

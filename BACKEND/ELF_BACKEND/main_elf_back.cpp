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


#include "MYassert.h"
#include "colors.h"

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

    SNode* Root = read_tree ("FILES/ParsedSrc.tr");

    if (Root != NULL)
    {
        make_gv_tree (Root, "BACKEND/GRAPH_VIZ/GraphViz_treeDump", false);

        system ("chmod +rwx EXAMPLES_ELF/code.elf");

        errno = 0;
        // /home/mikhail/PROGA/1st_term/Language/
        FILE* ExFile = fopen ("EXAMPLES_ELF/code.elf", "w");

        if (ExFile == NULL)
        {
            printf ("fopen errno [%d]", errno);
        }

        MLA (ExFile != NULL);

        make_elf_file (Root, ExFile);

        fclose (ExFile);

        delete_tree (&Root);

        system ("chmod +rwx EXAMPLES_ELF/code.elf");

        system("EXAMPLES_ELF/code.elf");
    }

    return 0;
}

//=============================================================================================================================================================================

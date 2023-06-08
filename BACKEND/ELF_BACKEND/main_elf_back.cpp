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

    AstNode* Root = read_tree ("AST/ParsedSrc.tr");

    if (Root != NULL)
    {
        const char* FileName = (argc > 1)  ? argv[1] : "a.elf";

        #ifdef DEBUG_AST_VIZ
        make_graf_viz_tree (Root, "BACKEND/GRAPH_VIZ/GraphViz_treeDump", false);
        #endif

        FILE* ExFile = fopen (FileName, "w");

        MY_LOUD_ASSERT (ExFile != NULL);

        make_elf_file (Root, ExFile);

        fclose (ExFile);

        delete_tree (&Root);

        size_t length = strlen (FileName) + 100;

        char* Command = (char*) calloc (sizeof (*FileName), length);

        sprintf (Command, "chmod +rwx %s", FileName);
        system (Command);

        free (Command);
    }
    else
    {
        printf("No AST found!\n");
    }

    return 0;
}

//=============================================================================================================================================================================

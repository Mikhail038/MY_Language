//=============================================================================================================================================================================

#include <cstddef>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <locale.h>
#include <type_traits>
#include <wctype.h>
#include <wchar.h>

//===================================================================================================================================================================

#include "MYassert.h"
#include "back.h"
#include "colors.h"

//=============================================================================================================================================================================

#include "front.h"

//===================================================================================================================================================================
//WriteTree//
//===================================================================================================================================================================

void write_tree (AstNode* Root, const char* FileName)
{
    FILE* OutputFile = fopen (FileName, "w");
    MY_LOUD_ASSERT (OutputFile != NULL);

    file_wprint (Root, 0, OutputFile);

    fwprintf (OutputFile, L"" FORBIDDEN_TERMINATING_SYMBOL);

    fclose (OutputFile);

    return;
}

void file_wprint (AstNode* Node, int n, FILE* OutputFile)
{
    if (Node == NULL)
    {
        return;
    }

    do_tab (n, OutputFile);

    print_node (Node, OutputFile);

    //if (Node->left != NULL)
    {
        do_tab (n, OutputFile);
        fwprintf (OutputFile, L"{\n");
        file_wprint (Node->left, n + 1, OutputFile);
        do_tab (n, OutputFile);
        fwprintf (OutputFile, L"}\n");
    }

    //if (Node->right != NULL)
    {
        //wprintf (L"i m here %ls\n", Node->data);
        do_tab (n, OutputFile);
        fwprintf (OutputFile, L"{\n");
        file_wprint (Node->right, n + 1, OutputFile);
        do_tab (n, OutputFile);
        fwprintf (OutputFile, L"}\n");
    }

    return;
}

void do_tab (int n, FILE* OutputFile)
{
    for (int i = 0; i < n; i++)
    {
        fwprintf (OutputFile, L"" TAB);
    }

    return;
}

void print_node (AstNode* Node, FILE* OutputFile)
{
    switch (Node->category)
    {
        case CategoryValue:
            fwprintf (OutputFile, L"CategoryValue TValue %lg", Node->data.val);
            break;

        case CategoryLine:
            fwprintf (OutputFile, L"CategoryLine TypeVariable %ls", Node->data.var);
            break;

        case CategoryOperation:
            switch (Node->type)
            {
                #define DEF_OP(d_type, d_condition, d_tokenize, d_print, ...) \
                case d_type: \
                    fwprintf (OutputFile, L"CategoryOperation %ls", d_print); \
                    break;

                #include "Operations.h"

                #undef DEF_OP

                default:
                    MY_LOUD_ASSERT(false);
            }
            break;

        default:
            MY_LOUD_ASSERT(false);
    }

    fwprintf (OutputFile, L"\n");
    return;
}

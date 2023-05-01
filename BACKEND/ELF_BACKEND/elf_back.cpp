//==================================================================================================================================================================
//29.04.2023
//==================================================================================================================================================================

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <locale.h>
#include <wchar.h>
#include <string.h>

#include "MYassert.h"
#include "colors.h"

//=============================================================================================================================================================================

#include "elf_back.h"

#include "back.h"
#include "front.h"


#include "asm.h"
#include "proc.h"
#include <bits/types/FILE.h>
#include <cstdio>

//=============================================================================================================================================================================

void make_elf_file (SNode* Root, FILE* ExFile)
{
    create_elf_header (Root, ExFile);

    return;
}

//==================================================================================================================================================================

void create_elf_header (SNode* Root, FILE* ExFile)
{

    return;
}

//==================================================================================================================================================================

//=============================================================================================================================================================================
//Make ASM//
//=============================================================================================================================================================================

void create_elf_body (SNode* Root, FILE* File)
{
    MLA (File != NULL);

    SBack* Back = (SBack*) calloc (1, sizeof (SBack));

    Back->Funcs         = (SBackFuncTable*)     calloc (1, sizeof (SBackFuncTable));
    Back->Funcs->Table  = (SBackFunc*)          calloc (MAX_FUNCS_ARRAY, sizeof (SBackFunc));
    Back->Funcs->top_index = 0;

    Back->VarStack      = (SStack<SVarTable*>*) calloc (1, sizeof (SStack<SVarTable*>));

    Back->file = File;

    Back->table_cond = none;

    stack_constructor (Back->VarStack, 4);

    //generate_user_functions (Root, Back);

    elf_generate_code (Root, Back);

    free_tables (Back->VarStack);

    fclose (Back->file);

    // for (int i = 0; i < Back->Funcs->top_index; i++)
    // {
    //     printf ("'%ls'[%d]\n", Back->Funcs->Table[i]);
    // }

    free (Back->Funcs->Table);
    free (Back->Funcs);

    free (Back);

    return;
}

void elf_generate_code (SNode* Root, SBack* Back)
{
    fprintf (Back->file, SEP_LINE "\n\n");

    PUT (push);
    fprintf (Back->file, " 0\n"); //TODO ask Danya maybe %d 0
    PUT (pop);
    fprintf (Back->file, " " SHIFT_REG "\n\n");


    PUT (push);
    fprintf (Back->file, " 0\n");
    PUT (pop);
    fprintf (Back->file, " " TOP_REG "\n\n");

    PUT (jump);
    fprintf (Back->file, " " LABEL MAIN_JUMP "\n\n");

    fprintf (Back->file, SEP_LINE "\n\n");

    elf_generate_statement (Root, Back);

    //delete_var_table (Back);

    return;
}

void elf_generate_statement (BACK_FUNC_HEAD_PARAMETERS)
{
    if (CurNode->left != NULL)
    {
        elf_generate_op_node (BACK_LEFT_SON_FUNC_PARAMETERS);
    }

    if (CurNode->right != NULL)
    {
        elf_generate_statement (BACK_RIGHT_SON_FUNC_PARAMETERS);
    }

    return;
}

void elf_generate_function (BACK_FUNC_HEAD_PARAMETERS)
{
    if (wcscmp (CurNode->left->data.var, MAIN_WORD) == 0)
    {
        Back->func_cond = main_f;
    }
    else
    {
        Back->func_cond = any_f;
    }

    fprintf (Back->file, LABEL "%ls:\n", CurNode->left->data.var);

    Back->Funcs->Table[Back->Funcs->top_index].Name = CurNode->left->data.var;

    Back->table_cond = none;
    create_param_var_table (CurNode->left->right, Back);

    elf_pop_parameters (Back);

    Back->Funcs->top_index++;


    elf_generate_statement (BACK_RIGHT_SON_FUNC_PARAMETERS);
    CLEAN_TABLE;

    fprintf (Back->file, "\n\n" SEP_LINE "\n\n\n");

    return;
}

void elf_generate_node (BACK_FUNC_HEAD_PARAMETERS)
{
    if (CurNode->category == COperation)
    {
        elf_generate_op_node (BACK_FUNC_PARAMETERS);
    }

    if (CurNode->category == CValue)
    {
        elf_write_command (push, Back->file);

        fprintf (Back->file, " %lg\n", CurNode->data.val);
    }

    if (CurNode->category == CLine && CurNode->type == TVariable)
    {
        elf_generate_push_var (BACK_FUNC_PARAMETERS);
//         write_command (push, Back->file);
//
//         int Index = find_var (BACK_FUNC_PARAMETERS);
//
//         fprintf (Back->file, " [%d]\n", Index);
    }

    return;
}

void elf_generate_op_node (BACK_FUNC_HEAD_PARAMETERS)
{
    switch (CurNode->type)
    {
        #define DEF_OP(d_type, d_condition, d_tokenize, d_print, d_switch) \
        case d_type: \
            d_switch; \
            break;

        #include "Operations.h"

        #undef DEF_OP
    }

    return;
}

void elf_generate_announce (BACK_FUNC_HEAD_PARAMETERS)
{
    elf_generate_expression (BACK_RIGHT_SON_FUNC_PARAMETERS);

    add_to_var_table (BACK_LEFT_SON_FUNC_PARAMETERS);

    elf_generate_pop_var (BACK_LEFT_SON_FUNC_PARAMETERS);

    elf_incr_top_reg (Back);
//     write_command (pop, Back->file);
//
//     fprintf (Back->file, " [%d]\n\n", Back->RAM_top_index - 1);

    return;
}

void elf_generate_equation (BACK_FUNC_HEAD_PARAMETERS)
{
    elf_generate_expression (BACK_RIGHT_SON_FUNC_PARAMETERS);

    elf_generate_pop_var (BACK_LEFT_SON_FUNC_PARAMETERS);

    return;
}

void elf_generate_input (BACK_FUNC_HEAD_PARAMETERS)
{
    SNode* Node = CurNode->left;

    do
    {
        writeln_command (inp, Back->file);

        elf_generate_pop_var (Node->left, Back);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void elf_generate_output (BACK_FUNC_HEAD_PARAMETERS)
{
    SNode* Node = CurNode->left;

    do
    {
        elf_generate_push_var (Node->left, Back);

        writeln_command (out, Back->file);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void elf_generate_if (BACK_FUNC_HEAD_PARAMETERS)
{
    elf_generate_postorder (BACK_LEFT_SON_FUNC_PARAMETERS);

    PUT (push);
    fprintf (Back->file, " %d\n", FALSE);

    PUT (je);
    fprintf (Back->file, " " LABEL "%d\n", Back->label_cnt);
    int Label_1 = Back->label_cnt;
    Back->label_cnt++;

    CurNode = CurNode->right;

    Back->table_cond = none;
    elf_generate_statement (BACK_LEFT_SON_FUNC_PARAMETERS);
    CLEAN_TABLE;

    PUT (jump);
    fprintf (Back->file, " " LABEL "%d\n", Back->label_cnt);
    int Label_2 = Back->label_cnt;
    Back->label_cnt++;

    fprintf (Back->file,  LABEL "%d:\n", Label_1);

    CurNode = CurNode->right;

    if (CurNode != NULL)
    {
        if (CurNode->category == COperation && CurNode->type == TIf)
        {
            elf_generate_if (BACK_FUNC_PARAMETERS);
        }
        else
        {
            Back->table_cond = none;
            elf_generate_statement (BACK_FUNC_PARAMETERS);
            CLEAN_TABLE;
        }
    }
    fprintf (Back->file,  LABEL "%d:\n", Label_2);


    return;
}

void elf_generate_while (BACK_FUNC_HEAD_PARAMETERS)
{
    int Label_1 = Back->label_cnt;
    fprintf (Back->file,  LABEL "%d:\n", Label_1);
    Back->label_cnt++;

    elf_generate_postorder (BACK_LEFT_SON_FUNC_PARAMETERS);

    PUT (push);
    fprintf (Back->file, " %d\n", FALSE);

    PUT (je);
    int Label_2 = Back->label_cnt;
    fprintf (Back->file, " " LABEL "%d\n", Label_2);
    Back->label_cnt++;

    Back->table_cond = none;
    elf_generate_statement (BACK_RIGHT_SON_FUNC_PARAMETERS);
    CLEAN_TABLE;

    PUT (jump);
    fprintf (Back->file, " " LABEL "%d\n", Label_1);

    fprintf (Back->file,  LABEL "%d:\n", Label_2);

    return;
}

void elf_generate_call (BACK_FUNC_HEAD_PARAMETERS)
{
    fprintf (Back->file, ";++" "\n\n");

    PUT (push);
    fprintf (Back->file, " " TOP_REG "\n");

    elf_push_parameters (BACK_RIGHT_SON_FUNC_PARAMETERS);

    PUT (push);
    fprintf (Back->file, " " TOP_REG "\n");

    PUT (pop);
    fprintf (Back->file, " " SHIFT_REG "\n\n");

    PUT (call);
    fprintf (Back->file, " " LABEL "%ls\n", CurNode->left->data.var);

    PUT (pop);
    fprintf (Back->file, " " TOP_REG "\n");

    fprintf (Back->file, ";++" "\n\n");

    PUT (push);
    fprintf (Back->file, " " FUNC_REG "\n");

    return;
}

void elf_generate_return (BACK_FUNC_HEAD_PARAMETERS)
{
    if (Back->func_cond == main_f)
    {
        writeln_command (hlt, Back->file);
    }
    else
    {
        elf_generate_expression (BACK_LEFT_SON_FUNC_PARAMETERS);

        PUT (pop);
        fprintf (Back->file, " " FUNC_REG "\n");

        PUTLN (ret);
    }

    return;
}

void elf_generate_expression (BACK_FUNC_HEAD_PARAMETERS)
{
    elf_generate_postorder (BACK_FUNC_PARAMETERS);

    return;
}

void elf_generate_postorder (BACK_FUNC_HEAD_PARAMETERS)
{
    if (!(CurNode->category == COperation && CurNode->type == T_Call) && CurNode->left != NULL)
    {
        elf_generate_postorder (BACK_LEFT_SON_FUNC_PARAMETERS);
    }

    if (!(CurNode->category == COperation && CurNode->type == T_Call) && CurNode->right != NULL)
    {
        elf_generate_postorder (BACK_RIGHT_SON_FUNC_PARAMETERS);
    }

    elf_generate_node (BACK_FUNC_PARAMETERS);

    return;
}

void elf_generate_pop_var (BACK_FUNC_HEAD_PARAMETERS)
{
    int Index = find_var (BACK_FUNC_PARAMETERS);

    PUT (push);
    fprintf (Back->file, " %d\n", Index);

    PUT (push);
    fprintf (Back->file, " " SHIFT_REG "\n");

    PUTLN (add);

    PUT (pop);
    fprintf (Back->file, " " COUNT_REG "\n");


    PUT (pop);
    fprintf (Back->file, " [" COUNT_REG "]\n\n");

    return;
}

void elf_generate_push_var (BACK_FUNC_HEAD_PARAMETERS)
{
    int Index = find_var (BACK_FUNC_PARAMETERS);

    PUT (push);
    fprintf (Back->file, " %d\n", Index);

    PUT (push);
    fprintf (Back->file, " " SHIFT_REG "\n");

    PUTLN (add);

    PUT (pop);
    fprintf (Back->file, " " COUNT_REG "\n");

    PUT (push);
    fprintf (Back->file, " [" COUNT_REG "]\n\n");

    return;
}

//=============================================================================================================================================================================

void elf_push_parameters (BACK_FUNC_HEAD_PARAMETERS)
{
    if (CurNode->right != NULL)
    {
        elf_push_parameters (BACK_RIGHT_SON_FUNC_PARAMETERS);
    }

    if (CurNode != NULL)
    {
        elf_generate_expression (BACK_LEFT_SON_FUNC_PARAMETERS);
    }

    // PUT (push);
    // fprintf (Back->file, " " TOP_REG "\n");

    return;
}

void elf_pop_parameters (SBack* Back)
{
    PUT (push);
    fprintf (Back->file, " " SHIFT_REG "\n");

    // PUTLN (m_dup);
    // PUTLN (out);

    PUT (pop);
    fprintf (Back->file, " " COUNT_REG "\n\n");

    for (int i = 0; i < Back->Funcs->Table[Back->Funcs->top_index].parameters ; i++)
    {
        PUT (push);
        fprintf (Back->file, " " COUNT_REG "\n");

        PUT (push);
        fprintf (Back->file, " 1\n");

        PUTLN (add);

        PUT (pop);
        fprintf (Back->file, " " COUNT_REG "\n");

        PUT (pop);
        fprintf (Back->file, " [" COUNT_REG "]\n\n");
    }

//     PUT (push);
//     fprintf (Back->file, " " COUNT_REG "\n");
//
//     PUT (pop);
//     fprintf (Back->file, " " SHIFT_REG "\n");

    return;
}

void elf_incr_top_reg (SBack* Back)
{
    PUT (push);
    fprintf (Back->file, " " TOP_REG "\n");

    PUT (push);
    fprintf (Back->file, " 1\n");

    PUTLN (add);

    PUT (pop);
    fprintf (Back->file, " " TOP_REG "\n\n");

    return;
}

void elf_standard_if_jump (SBack* Back)
{
    fprintf (Back->file, " " LABEL "%d\n", Back->label_cnt);
    Back->label_cnt++;

    PUT (push);
    fprintf (Back->file, " %d\n", FALSE);

    PUT (jump);
    fprintf (Back->file, " " LABEL "%d\n", Back->label_cnt);

    fprintf (Back->file, LABEL "%d:\n", Back->label_cnt - 1);

    PUT (push);
    fprintf (Back->file, " %d\n", TRUE);

    fprintf (Back->file, LABEL "%d:\n", Back->label_cnt);

    Back->label_cnt++;

    return;
}

// SNode* find_main (SNode* Node)
// {
//     if (NODE_IS_OP_AND__ T_Function)
//     {
//         if (Node->left != NULL &&
//         Node->left->category == CLine && (wcscmp (MAIN_WORD, Node->left->data.var) == 0))
//         {
//             return Node->right->right;
//         }
//     }
//
//     SNode* Main = NULL;
//
//     if (Node->left != NULL)
//     {
//         Main = find_main (Node->left);
//     }
//
//     if ((Node->right != NULL) && (Main == NULL))
//     {
//         Main = find_main (Node->right);
//     }
//
//     return Main;
// }

void elf_write_command (ECommandNums eCommand, FILE* File)
{
    #define DEF_CMD(def_line, def_enum, ...) \
    else if (eCommand == def_enum) \
    { \
        fprintf (File, TAB  def_line); \
        return; \
    }

    if (0) {}

    #include "commands.h"

    else
    {
        MLA (0);
    }

    #undef DEF_CMD
}

//=============================================================================================================================================================================

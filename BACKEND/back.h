
#ifndef BACK_H
#define BACK_H

//===================================================================================================================================================================

typedef wchar_t CharT; //will not work with char

//===================================================================================================================================================================

#include "front.h"

#include "stack.h"

#include "asm.h"

#include "proc.h"


//===================================================================================================================================================================

#define NODE_IS_OP_AND__ Node->category == COperation && Node->type ==

#define BACK_FUNC_HEAD_PARAMETERS SNode* CurNode, SBack* Back

#define BACK_FUNC_PARAMETERS CurNode, Back

#define BACK_LEFT_SON_FUNC_PARAMETERS CurNode->left, Back

#define BACK_RIGHT_SON_FUNC_PARAMETERS CurNode->right, Back

//===================================================================================================================================================================

enum EFuncConditions
{
    any_f  = 1,
    main_f = 2
};

enum EVarTableConditions
{
    none  = 1,
    exist = 2
};

typedef struct
{
    EFuncConditions     func_cond   = any_f;
    EVarTableConditions table_cond  = none;
    int                 top_index   = 0;
    FILE*               file        = NULL;
    StructStack*        VarStack    = NULL;
}
SBack;

//===================================================================================================================================================================

// typedef struct
// {
//     CharT* name;
//     int    index;
// }
// SVarAccord;

//===================================================================================================================================================================

void my_b_main (int argc, char** argv);

//===================================================================================================================================================================
//Readres//
//===================================================================================================================================================================

SNode* read_tree (const char* FileName);

SNode* read_node (SSrc* Tree);

//===================================================================================================================================================================
//Make ASM//
//===================================================================================================================================================================

void make_asm_file (SNode* Root, FILE* File);

void generate_main (BACK_FUNC_HEAD_PARAMETERS);

void generate_node (BACK_FUNC_HEAD_PARAMETERS);

void generate_op_node (BACK_FUNC_HEAD_PARAMETERS);

void generate_return (BACK_FUNC_HEAD_PARAMETERS);

void generate_announce (BACK_FUNC_HEAD_PARAMETERS);

void generate_equation (BACK_FUNC_HEAD_PARAMETERS);

void generate_expression (BACK_FUNC_HEAD_PARAMETERS);

void generate_postorder (BACK_FUNC_HEAD_PARAMETERS);

//===================================================================================================================================================================

void add_to_var_table (BACK_FUNC_HEAD_PARAMETERS);

void create_new_var_table (SBack* Back);

void delete_var_table (SBack* Back);

int find_var (BACK_FUNC_HEAD_PARAMETERS);

bool find_in_table (CharT* varName, SVarTable* Table, int* RetIndex);

//===================================================================================================================================================================

SNode* find_main (SNode* Root);

void write_command (ECommandNums eCommand, FILE* File);

void writeln_command (ECommandNums eCommand, FILE* File);

//===================================================================================================================================================================
//Execute//
//===================================================================================================================================================================

void execute_asm_file (FILE* ExFile);

//===================================================================================================================================================================

#endif

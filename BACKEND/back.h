
#ifndef BACK_H
#define BACK_H

//===================================================================================================================================================================

typedef wchar_t CharT; //will not work with char

// typedef struct
// {
//     CharT* name;
//     int    index;
// }
// SVarAccord;
//
// typedef struct
// {
//     int         size = 0;
//     SVarAccord* Arr  = NULL;
// }
// SVarTable;

//===================================================================================================================================================================

#include "front.h"

#include "stackT.h"

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
    any_f   = 1,
    main_f  = 2
};

enum EVarTableConditions
{
    none  = 1,
    exist = 2
};

typedef struct
{
    CharT* Name         = NULL;
    int    parameters   = 0;
}
SBackFunc;

typedef struct
{
    SBackFunc* Table        = NULL;
    int        top_index    = 0;
}
SBackFuncTable;

typedef struct
{
    EFuncConditions     func_cond       = any_f;
    EVarTableConditions table_cond      = none;
    int                 RAM_top_index   = 0;
    int                 label_cnt       = 0;
    FILE*               file            = NULL;
    SBackFuncTable*     Funcs           = NULL;
    SStack<SVarTable*>* VarStack        = NULL;
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

void generate_code (SNode* Root, SBack* Back);

void generate_main (BACK_FUNC_HEAD_PARAMETERS);

void generate_statement (BACK_FUNC_HEAD_PARAMETERS);

void generate_function (BACK_FUNC_HEAD_PARAMETERS);

void generate_node (BACK_FUNC_HEAD_PARAMETERS);

void generate_op_node (BACK_FUNC_HEAD_PARAMETERS);

void generate_input (BACK_FUNC_HEAD_PARAMETERS);

void generate_output (BACK_FUNC_HEAD_PARAMETERS);

void generate_if (BACK_FUNC_HEAD_PARAMETERS);

void generate_while (BACK_FUNC_HEAD_PARAMETERS);

void generate_call (BACK_FUNC_HEAD_PARAMETERS);

void generate_return (BACK_FUNC_HEAD_PARAMETERS);

void generate_announce (BACK_FUNC_HEAD_PARAMETERS);

void generate_equation (BACK_FUNC_HEAD_PARAMETERS);

void generate_expression (BACK_FUNC_HEAD_PARAMETERS);

void generate_postorder (BACK_FUNC_HEAD_PARAMETERS);

void generate_pop_var (BACK_FUNC_HEAD_PARAMETERS);

void generate_push_var (BACK_FUNC_HEAD_PARAMETERS);

//===================================================================================================================================================================

void push_parameters (BACK_FUNC_HEAD_PARAMETERS);

void pop_parameters (SBack* Back);

void incr_top_reg (SBack* Back);

void standard_if_jump (SBack* Back);

//===================================================================================================================================================================

void add_to_var_table (BACK_FUNC_HEAD_PARAMETERS);

void create_new_var_table (SBack* Back);

void create_param_var_table (BACK_FUNC_HEAD_PARAMETERS);

void delete_var_table (SBack* Back);

int find_var (BACK_FUNC_HEAD_PARAMETERS);

bool find_in_table (CharT* varName, SVarTable* Table, int* RetIndex);

void free_tables (SStack<SVarTable*>* VarStack);

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

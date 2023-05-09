#pragma once
//==================================================================================================================================================================
//29.04.2023
//==================================================================================================================================================================

#include "back.h"

//==================================================================================================================================================================
enum ERegisters
{
    rax = 0,
    rbx = 3,
    rcx = 1,
    rdx = 2,
    rsp = 4,
    rbp = 5,
    rsi = 6,
    rdi = 7
};

enum ERegisters_64
{
    r8  = 0,
    r9  = 1,
    r10 = 2,
    r11 = 3,
    r12 = 4,
    r13 = 5,
    r14 = 6,
    r15 = 7
};

typedef struct
{
    EFuncConditions     func_cond       = any_f;
    EVarTableConditions table_cond      = none;
    int                 RAM_top_index   = 0;
    int                 label_cnt       = 0;
    FILE*               file            = NULL;
    SBackFuncTable*     Funcs           = NULL;
    SStack<SVarTable*>* VarStack        = NULL;

    char*               Array           = NULL;
    size_t              cnt             = 0;
}
SElfBack;

//==================================================================================================================================================================

#define ELF_BACK_FUNC_HEAD_PARAMETERS SNode* CurNode, SElfBack* Back

#define ELF_CLEAN_TABLE if (Back->table_cond != none) { elf_delete_var_table (Back); Back->table_cond = exist; }

//==================================================================================================================================================================

void make_elf_file (SNode* Root, FILE* ExFile);

SElfBack* elf_back_constructor (FILE* ExFile);

void elf_back_destructor (SElfBack* Back);

//==================================================================================================================================================================

void create_elf_header (SNode* Root, FILE* ExFile);

void create_elf_body (SNode* Root, FILE* ExFile);

//==================================================================================================================================================================

void generate_elf_array (SNode* Root, SElfBack* Back);

void elf_generate_end_header (SNode* Root, SElfBack* Back);

void elf_generate_code (ELF_BACK_FUNC_HEAD_PARAMETERS);

void generate_main (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_statement (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_function (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_node (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_op_node (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_input (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_output (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_if (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_while (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_call (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_return (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_announce (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_equation (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_expression (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_postorder (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_pop_var (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_push_var (ELF_BACK_FUNC_HEAD_PARAMETERS);

//===================================================================================================================================================================

void elf_push_parameters (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_pop_parameters (SElfBack* Back);

void elf_incr_top_reg (SElfBack* Back);

void elf_standard_if_jump (SElfBack* Back);

void elf_write_command (ECommandNums eCommand, FILE* File);

void elf_add_to_var_table (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_create_new_var_table (SElfBack* Back);

void elf_create_param_var_table (ELF_BACK_FUNC_HEAD_PARAMETERS);

void elf_delete_var_table (SElfBack* Back);

int elf_find_var (ELF_BACK_FUNC_HEAD_PARAMETERS);

//===================================================================================================================================================================

void x86_push_imm (ELF_BACK_FUNC_HEAD_PARAMETERS, char Number);

void x86_push_imm (ELF_BACK_FUNC_HEAD_PARAMETERS, short Number);

void x86_push_imm (ELF_BACK_FUNC_HEAD_PARAMETERS, int Number);

void x86_push_r (ELF_BACK_FUNC_HEAD_PARAMETERS, int Register);

void x86_push_r64 (ELF_BACK_FUNC_HEAD_PARAMETERS, int Register);

void x86_pop_r (ELF_BACK_FUNC_HEAD_PARAMETERS, int Register);

void x86_pop_r64 (ELF_BACK_FUNC_HEAD_PARAMETERS, int Register);

//===================================================================================================================================================================


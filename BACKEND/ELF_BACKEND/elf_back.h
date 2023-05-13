#pragma once
//==================================================================================================================================================================
//29.04.2023
//==================================================================================================================================================================

#include "back.h"
#include <cstddef>
#include <cstdlib>

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

#define DELTA 10

enum ERegisters_64
{
    r8  = DELTA + 0,
    r9  = DELTA + 1,
    r10 = DELTA + 2,
    r11 = DELTA + 3,
    r12 = DELTA + 4,
    r13 = DELTA + 5,
    r14 = DELTA + 6,
    r15 = DELTA + 7
};

//DO NOT MAKE THEM RAX
#define A_REG  rbx
#define B_REG  rcx

#define eSHIFT_REG  r12
#define eTOP_REG    r14
#define eFUNC_REG   r15

#define eCOUNT_REG  rax //TODO maybe not

//==================================================================================================================================================================

typedef struct SElfBack
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

    public:

    SElfBack(FILE* ExFile, SNode* CurNode);

    ~SElfBack();

    SElfBack(SElfBack& Another) = delete;
    SElfBack(SElfBack&&  Another) = delete;

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_push_i (int Number);

    void x86_push_r (int Register);
    // void x86_push_r64 (int Register);

    void x86_push_IrI (int Register);
    // void x86_push_Ir64I (int Register);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_pop_r (int Register);
    // void x86_pop_r64 (int Register);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_nop ();

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_mov_r_r (int dstReg, int srcReg);
    // void x86_mov_r64_r (int dstReg, int srcReg);
    // void x86_mov_r_r64 (int dstReg, int srcReg);
    // void x86_mov_r64_r64 (int dstReg, int srcReg);

    void x86_mov_r_i (int dstReg, int Number);
    // void x86_mov_r64_i (int dstReg, int Number);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_add_stack ();
    void x86_sub_stack ();
    void x86_imul_stack ();
    void x86_idiv_stack ();

    void x86_add_i (int Register, int Number);
    void x86_sub_i (int Register, int Number);

    void x86_inc (int Reg);
    void x86_dec (int Reg);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void set_hex_int (const int Number);

    void x86___DstSrc_config (int& dstReg, int& srcReg);
    void x86___Dst_config (int& dstReg);

    void x86___Regs_config (const int dstReg, const int srcReg);
    void x86___Reg_config(const int Reg, const int ZeroPoint);

    void x86___End ();

    //==================================================================================================================================================================

    void generate_elf_array (SNode* Root);
    void elf_generate_code (SNode* Root);
    void generate_main ();

    void elf_generate_statement (SNode* CurNode);
    void elf_generate_function (SNode* CurNode);
    void elf_generate_node (SNode* CurNode);
    void elf_generate_op_node (SNode* CurNode);
    void elf_generate_input (SNode* CurNode);
    void elf_generate_output (SNode* CurNode);
    void elf_generate_if (SNode* CurNode);
    void elf_generate_while (SNode* CurNode);
    void elf_generate_call (SNode* CurNode);
    void elf_generate_return (SNode* CurNode);
    void elf_generate_announce (SNode* CurNode);
    void elf_generate_equation (SNode* CurNode);
    void elf_generate_expression (SNode* CurNode);
    void elf_generate_postorder (SNode* CurNode);

    void elf_generate_pop_var (SNode* CurNode);
    void elf_generate_push_var (SNode* CurNode);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void elf_push_parameters (SNode* CurNode);
    void elf_pop_parameters ();

    void elf_incr_top_reg ();

    void elf_standard_if_jump ();

    void elf_write_command (ECommandNums eCommand, FILE* File);

    void elf_add_to_var_table (SNode* CurNode);
    void elf_create_new_var_table ();
    void elf_create_param_var_table (SNode* CurNode);
    void elf_delete_var_table ();

    int elf_find_var (SNode* CurNode);
};

//==================================================================================================================================================================

#define ELF_BACK_FUNC_HEAD_PARAMETERS SNode* CurNode, SElfBack* Back

#define ELF_CLEAN_TABLE if (table_cond != none) { elf_delete_var_table (); table_cond = exist; }

//==================================================================================================================================================================

void make_elf_file (SNode* Root, FILE* ExFile);

//==================================================================================================================================================================

void create_elf_header (SNode* Root, FILE* ExFile);

void create_elf_body (SNode* Root, FILE* ExFile);

//===================================================================================================================================================================


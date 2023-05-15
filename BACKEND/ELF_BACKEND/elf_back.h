#pragma once
//==================================================================================================================================================================
//29.04.2023
//==================================================================================================================================================================

#include "back.h"
#include <cstddef>
#include <cstdlib>
#include <list>
#include <type_traits>
#include <unordered_map>

#define PUTLN(d_command)
#define PUT(d_command)


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

//DO NOT MAKE THEM RAX or RDX
#define A_REG  rbx
#define B_REG  rcx

#define eSHIFT_REG  r8
#define eCOUNT_REG  r9
#define eTOP_REG    r10
#define eFUNC_REG   r11


//==================================================================================================================================================================
#define MAX_CALLS_OF_ONE_FUNC 50
#define NULL_FINISH 0

typedef struct TLabel
{
    size_t  cnt = 0;
    size_t* start;
    // std::list<size_t> start;
    size_t finish = NULL_FINISH;

    TLabel()
    {
        start = (size_t*) calloc (MAX_CALLS_OF_ONE_FUNC, sizeof (size_t));
    }

    TLabel(size_t Finish)
    {
        finish = Finish;
        start = (size_t*) calloc (MAX_CALLS_OF_ONE_FUNC, sizeof (size_t));
        cnt = 0;
    }

    TLabel(size_t FirstStart, size_t Finish)
    {
        finish = Finish;
        start = (size_t*) calloc (MAX_CALLS_OF_ONE_FUNC, sizeof (size_t));

        start[0] = FirstStart;
        cnt = 1;
    }

//     TLabel (TLabel&& other)
//     {
//         finish = other.finish;
//         cnt = other.cnt;
//         start = other.start;
//
//         other.start = nullptr;
//     }

    ~TLabel()
    {
        free (start);
    }
};

typedef struct SElfBack
{
    EVarTableConditions table_cond      = none;
    int                 RAM_top_index   = 0;
    int                 label_cnt       = 0;
    FILE*               file            = NULL;
    SBackFuncTable*     Funcs           = NULL;
    SStack<SVarTable*>* VarStack        = NULL;

    char*               Array           = NULL;
    size_t              cnt             = 0;
    size_t              start_cnt       = 0;


    std::unordered_map<const wchar_t*, TLabel> Labels;

public:
    SElfBack(FILE* ExFile, SNode* CurNode);

    ~SElfBack();

    SElfBack(SElfBack& Another) = delete;
    SElfBack(SElfBack&&  Another) = delete;

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_push_i (const int Number);
    void x86_push_r (const int Register);
    void x86_push_IrI (const int Register);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_pop_r (int Register);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_nop ();

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_call       (int Shift);
    void x86_call_label (const wchar_t* Name);

    void x86_ret ();

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_syscall ();

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_cmp_r_r    (int dstReg, int srcReg);
    void x86_cmp_stack  ();

    //------------------------  -----------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_jump (int Shift, int JmpMode);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_mov_r_r    (const int dstReg, const int srcReg);
    void x86_mov_r_i    (const int dstReg, const int Number);
    void x86_mov_r_IrI  (const int dstReg, const int srcReg);
    void x86_mov_r_Ir_iI(const int dstReg, const int srcReg, int Shift);
    void x86_mov_IrI_r  (const int dstReg, const int srcReg);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_add_stack ();
    void x86_add_i (const int Register, const int Number);

    void x86_sub_stack ();
    void x86_sub_i (const int Register, const int Number);

    void x86_imul_stack ();
    void x86_idiv_stack ();

    void x86_inc (const int Reg);
    void x86_dec (const int Reg);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void set_hex_int    (const int Number);
    void set_hex_long   (const long Address);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86___DstSrc_config    (int& dstReg, int& srcReg);
    void x86___Dst_config       (int& dstReg);

    void x86___Regs_config  (const int dstReg, const int srcReg);
    void x86___Reg_config   (const int Reg, const int ZeroPoint);

    void x86___End ();

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86___paste_label (const wchar_t* Name);

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

    void elf_standard_if_jump (int JumpMode);

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


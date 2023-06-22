#pragma once
//==================================================================================================================================================================
//29.04.2023
//==================================================================================================================================================================

#include "elf_header_plus_tools.h"
#include "ARRAY/array.h"
#include "function_label.h"
#include "back.h"

#include <bits/types/FILE.h>
#include <cstddef>
#include <cstdlib>
#include <cwchar>
#include <functional>
#include <list>
#include <type_traits>
#include <unordered_map>

//=============================================================================================================================================================================

enum JumpModes
{
    x86_jmp    = 0,
    x86_ja     = 1,
    x86_jae    = 2,
    x86_jb     = 3,
    x86_jbe    = 5,
    x86_je     = 6,
    x86_jne    = 7,
    x86_jl     = 8,
    x86_jle    = 9,
    x86_jg     = 10,
    x86_jge    = 11
};

//==================================================================================================================================================================

const int QUADWORD_SIZE = 32;

const int MAX_JUMP_LABEL_SIZE = 50;

const int MAX_ELF_SIZE = 100000;

enum Registers
{
    rax = 0,
    rbx = 3,
    rcx = 1,
    rdx = 2,
    rsp = 4,
    rbp = 5,
    rsi = 6,
    rdi = 7,
    r8  = 8,
    r9  = 9,
    r10 = 10,
    r11 = 11,
    r12 = 12,
    r13 = 13,
    r14 = 14,
    r15 = 15
};

//DO NOT MAKE THEM RAX or RDX
const int FIRST_ARIPHMETIC_REG  = rbx; //beeing used in ariphmetic operations with stack
const int SECOND_ARIPHMETIC_REG = rcx; //beeing used in ariphmetic operations with stack
const int ELF_FUNC_RET_REG  = r11;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MAIN_LBL    L"main"
#define INP_LBL     L"inp"
#define OUT_LBL     L"out"

//==================================================================================================================================================================

struct ElfHead;
struct Patch;

// size_t hash_for_wchar_lines (const wchar_t* Line)

namespace
{
    struct hash_wchar
    {
        size_t operator()(const wchar_t* const& Line) const
        {
            size_t RetValue = 0;

            size_t length = wcslen(Line);

            for (size_t cnt = 0; cnt != length; ++cnt)
            {
                RetValue = ((RetValue >> 1) | (RetValue << 31)) ^ (unsigned int) Line[cnt];
            }

            #ifdef DEBUG
            printf (KRED  Kreverse "%lu\n" KNRM , RetValue);
            #endif

            return RetValue;
        }
    };

    struct equal_wchar
    {
        bool operator()(const wchar_t* const FirstLine, const wchar_t* const SecondLine) const
        {
            hash_wchar hash_function;
            return (hash_function(FirstLine) == hash_function(SecondLine));
        }
    };
}

struct ElfBack
{
    bool                exit_marker = false;
    FuncConditions      func_condition       = any_f;
    VarTableConditions  table_condition      = none;
    int                 delta_rbp       = 0;
    int                 label_cnt       = 0;
    FILE*               file            = NULL;
    char*               ex_file_name    = NULL;
    char*               ast_file_name   = NULL;
    char*               gv_file_name    = NULL;
    // char* a = NULL;
    SBackFuncTable*     Funcs           = NULL;
    SStack<SVarTable*>* VarStack        = NULL;

    const ElfHead*      head            = NULL;
    MyArray*            patches         = NULL;

    char*               ByteCodeArray   = NULL;
    size_t              cur_addr        = 0;

    std::unordered_map<const wchar_t*, JumpLabel, hash_wchar, equal_wchar> Labels;
};

//==================================================================================================================================================================


//==================================================================================================================================================================

void construct_elf_back (int argc, char** argv, ElfBack* Back, const ElfHead* ElfHeader);
void destruct_elf_back (ElfBack* Back);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void x86_push_imm (ElfBack* Back, const int Number);
void x86_push_reg (ElfBack* Back, const int Register);
void x86_push_from_addr_in_reg (ElfBack* Back, const int Register);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_pop_to_reg (ElfBack* Back, int Register);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_nop (ElfBack* Back);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_call (ElfBack* Back, int Shift);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_call_label (ElfBack* Back, const wchar_t* Name);

void x86_jump_label (ElfBack* Back, const wchar_t* Name, const int JumpMode);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_ret (ElfBack* Back);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_syscall (ElfBack* Back);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_cmp_reg_reg (ElfBack* Back, int dstReg, int srcReg);
void x86_cmp_stack   (ElfBack* Back);

//------------------------  -----------------------------------------------------------------------------------------------------------------------------------------------------

void x86_jump_any (ElfBack* Back, int Shift, int JmpMode);

void x86_jump_near (ElfBack* Back, int Shift, int JumpMode);
void x86_jump_norm (ElfBack* Back, int Shift, int JumpMode);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_mov_reg_reg    (ElfBack* Back, const int dstReg, const int srcReg);
void x86_mov_reg_imm    (ElfBack* Back, const int dstReg, const int Number);
void x86_mov_to_reg_from_addr_in_reg    (ElfBack* Back, const int dstReg, const int srcReg);
void x86_mov_to_reg_from_addr_in_reg_with_imm (ElfBack* Back, const int dstReg, const int srcReg, int Shift);
void x86_mov_to_addr_in_reg_from_reg    (ElfBack* Back, const int dstReg, const int srcReg);
void x86_mov_to_reg_from_imm_addr       (ElfBack* Back, int dstReg, int MemAddr);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_add_reg_reg (ElfBack* Back, int dstReg, int SrcReg);

void x86_add_stack (ElfBack* Back);
void x86_add_reg_imm (ElfBack* Back, const int Register, const int Number);

void x86_sub_reg_reg (ElfBack* Back, int dstReg, int SrcReg);

void x86_sub_stack (ElfBack* Back);
void x86_sub_reg_imm (ElfBack* Back, const int Register, const int Number);

void x86_imul_stack (ElfBack* Back);
void x86_idiv_stack (ElfBack* Back);

void x86_inc (ElfBack* Back, const int Reg);
void x86_dec (ElfBack* Back, const int Reg);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void set_hex_int    (ElfBack* Back, const int Number);
void set_hex_long   (ElfBack* Back, const long Address);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_dest_src_config      (ElfBack* Back, int& dstReg, int& srcReg);
void x86_destination_config   (ElfBack* Back, int& dstReg);

void x86_two_registers_config  (ElfBack* Back, const int dstReg, const int srcReg);
void x86_one_register_config   (ElfBack* Back, const int Reg, const int ZeroPoint);

void x86_exit (ElfBack* Back);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_paste_call_label (ElfBack* Back, const wchar_t* Name);
void x86_paste_jump_label (ElfBack* Back, const wchar_t* Name);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_make_input_func (ElfBack* Back);
void x86_make_input_func_body (ElfBack* Back);

void x86_make_output_func (ElfBack* Back);
void x86_make_output_func_body (ElfBack* Back);

void x86_write_new_line (ElfBack* Back);

//==================================================================================================================================================================

void elf_head_start (ElfBack* Back);
void elf_head_start_params (ElfBack* Back);
void elf_head_program_header_params (ElfBack* Back);

void elf_head_shstrtable (ElfBack* Back, size_t SegmentSizeSaved);

void generate_elf_array (ElfBack* Back, AstNode* Root);

void elf_generate_code (ElfBack* Back, AstNode* Root);

void generate_main ();

void elf_generate_statement (ElfBack* Back, AstNode* CurNode);
void elf_generate_function  (ElfBack* Back, AstNode* CurNode);
void elf_generate_node      (ElfBack* Back, AstNode* CurNode);
void elf_generate_op_node   (ElfBack* Back, AstNode* CurNode, bool RetValueMarker);
void elf_generate_call_input_function     (ElfBack* Back, AstNode* CurNode);
void elf_generate_call_output_function    (ElfBack* Back, AstNode* CurNode);
void elf_generate_if        (ElfBack* Back, AstNode* CurNode);
void elf_generate_while     (ElfBack* Back, AstNode* CurNode);
void elf_generate_call      (ElfBack* Back, AstNode* CurNode, bool RetValueMarker);
void elf_generate_return    (ElfBack* Back, AstNode* CurNode);
void elf_generate_announce  (ElfBack* Back, AstNode* CurNode);
void elf_generate_assignment  (ElfBack* Back, AstNode* CurNode);
void elf_generate_expression    (ElfBack* Back, AstNode* CurNode);
void elf_generate_postorder     (ElfBack* Back, AstNode* CurNode, bool RetValueMarker);

void elf_set_rax_var_value      (ElfBack* Back, AstNode* CurNode);
void elf_set_rax_var_address    (ElfBack* Back, AstNode* CurNode);

void elf_generate_pop_var   (ElfBack* Back, AstNode* CurNode);
void elf_generate_push_var  (ElfBack* Back, AstNode* CurNode);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void elf_push_function_parameters (ElfBack* Back, AstNode* CurNode);
void elf_pop_function_parameters (ElfBack* Back, AstNode* CurNode);

void elf_pop_parameters (ElfBack* Back);

void elf_incr_top_reg (ElfBack* Back);

void elf_standard_if_jump (ElfBack* Back, int JumpMode);

void elf_write_command (ElfBack* Back, ECommandNums eCommand, FILE* File);

void elf_add_to_var_table (ElfBack* Back, AstNode* CurNode, bool ParamMarker);
void elf_create_new_var_table (ElfBack* Back, bool ParamMarker);
void elf_create_param_var_table (ElfBack* Back, AstNode* CurNode);
void elf_delete_var_table (ElfBack* Back);

size_t elf_find_all_new_vars (ElfBack* Back, AstNode* CurNode);
size_t elf_find_new_var (ElfBack* Back, AstNode* CurNode);

AstNode* elf_find_parent_statement (AstNode* CurNode);

int elf_find_var (ElfBack* Back, AstNode* CurNode);

AstNode* elf_find_parent_statement (ElfBack* Back, AstNode* CurNode);
bool elf_find_in_table (ElfBack* Back, CharT* varName, SVarTable* Table, int* RetIndex, bool* ParamMarker);

const wchar_t* my_wchar_find (ElfBack* Back, const wchar_t* Name);

//==================================================================================================================================================================

void make_elf_file (ElfBack* Back);

//==================================================================================================================================================================

void create_elf_header (AstNode* Root, FILE* ExFile);

void create_elf_body (AstNode* Root, FILE* ExFile);

//===================================================================================================================================================================

void prepare_name_label_to_jump (wchar_t* Label_name, size_t Address);

//===================================================================================================================================================================

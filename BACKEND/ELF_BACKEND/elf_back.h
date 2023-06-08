#pragma once
//==================================================================================================================================================================
//29.04.2023
//==================================================================================================================================================================

#include "FuncLabel.h"
#include "back.h"

#include <cstddef>
#include <cstdlib>
#include <list>
#include <type_traits>
#include <unordered_map>

#define PUTLN(d_command)
#define PUT(d_command)


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

//==================================================================================================================================================================

#define SET_BYTE(x) Back->Array[Back->cur_addr] = (x); Back->cur_addr++;

#define SET_BYTE_2_BYTES(x)     \
    memcpy ((unsigned short int*) &(Back->Array[Back->cur_addr]),    \
                        &(x), sizeof (unsigned short int));  \
    Back->cur_addr += 2;

#define SET_BYTE_4_BYTES(x)     \
    memcpy ((unsigned int*) &(Back->Array[Back->cur_addr]),    \
                        &(x), sizeof (unsigned int));  \
    Back->cur_addr += 4;

#define SET_BYTE_8_BYTES(x)     \
    memcpy ((size_t*) &(Back->Array[Back->cur_addr]),    \
                        &(x), sizeof (size_t));  \
    Back->cur_addr += 8;

#define FILL_2  SET_BYTE (0x00);

#define FILL_4  for (int cnt = 0; cnt != 3; ++cnt) { SET_BYTE (0x00); }

#define FILL_8  for (int cnt = 0; cnt != 7; ++cnt) { SET_BYTE (0x00); }

#define PASTE_8(x,y)  \
    (x) = Back->cur_addr;    \
    memcpy ((size_t*) &(Back->Array[(y)]), \
    &(x), sizeof (size_t));

#define PASTE_4(x,y)  \
    (x) = Back->cur_addr;    \
    memcpy ((unsigned int*) &(Back->Array[(y)]), \
    &(x), sizeof (unsigned int));

#define SKIP_8(x,y)   \
    size_t (x) = 0; \
    size_t (y) = Back->cur_addr;    \
    Back->cur_addr += 8; //we ll skip it now

#define SKIP_4(x,y)   \
    unsigned int (x) = 0; \
    size_t (y) = Back->cur_addr;    \
    Back->cur_addr += 4; //we ll skip it now

//==================================================================================================================================================================

enum Registers
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

enum Registers_64
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
#define FIRST_REG  rbx
#define SECOND_REG  rcx

// #define ELF_SHIFT_REG  r8
// #define eCOUNT_REG  r9
// #define eTOP_REG    r10
#define ELF_FUNC_RET_REG   r11

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MAIN_LBL    L"main"
#define INP_LBL     L"inp"
#define OUT_LBL     L"out"

//==================================================================================================================================================================

typedef struct ElfBack
{
    EFuncConditions     func_cond       = any_f;
    EVarTableConditions table_cond      = none;
    int                 delta_rbp       = 0;
    int                 label_cnt       = 0;
    FILE*               file            = NULL;
    SBackFuncTable*     Funcs           = NULL;
    SStack<SVarTable*>* VarStack        = NULL;

    char*               Array           = NULL;
    size_t              cur_addr        = 0;
    size_t              start_cnt       = 0;

    size_t              buffer          = 0;

    std::unordered_map<const wchar_t*, FuncLabel> Labels;
};


//==================================================================================================================================================================

void construct_elf_back (ElfBack* Back, FILE* ExFile);
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

void x86_extra_dest_src_config      (ElfBack* Back, int& dstReg, int& srcReg);
void x86_extra_destination_config   (ElfBack* Back, int& dstReg);

void x86_extra_two_registers_config  (ElfBack* Back, const int dstReg, const int srcReg);
void x86_extra_one_register_config   (ElfBack* Back, const int Reg, const int ZeroPoint);

void x86_extra_exit (ElfBack* Back);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_extra_paste_call_label (ElfBack* Back, const wchar_t* Name);
void x86_extra_paste_jump_label (ElfBack* Back, const wchar_t* Name);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_extra_make_input_func (ElfBack* Back);
void x86_extra_make_input_func_body (ElfBack* Back);

void x86_extra_make_output_func (ElfBack* Back);
void x86_extra_make_output_func_body (ElfBack* Back);

void x86_write_new_line (ElfBack* Back);

//==================================================================================================================================================================

void elf_head_start (ElfBack* Back);
void elf_head_start_params (ElfBack* Back);
void elf_head_program_header_params (ElfBack* Back, const size_t FileVirtualAddress);
void elf_head_shstrtable (ElfBack* Back,
                            size_t& SegmentSize, size_t& addrSegmentSize,
                            size_t& SegmentFileSize, size_t& addrSegmentFileSize,
                            size_t& TableAddress, size_t& addrTableAddress,
                            size_t& TableLoadAddress, size_t& addrTableLoadAddress,
                            unsigned int& TextNameOffset, size_t& addrTextNameOffset,
                            unsigned int& TableNameOffset, size_t& addrTableNameOffset, const size_t FileVirtualAddress);

void generate_elf_array (ElfBack* Back, AstNode* Root);

void elf_generate_code (ElfBack* Back, AstNode* Root);

void generate_main ();

void elf_generate_statement (ElfBack* Back, AstNode* CurNode);
void elf_generate_function  (ElfBack* Back, AstNode* CurNode);
void elf_generate_node      (ElfBack* Back, AstNode* CurNode);
void elf_generate_op_node   (ElfBack* Back, AstNode* CurNode, bool RetValueMarker);
void elf_generate_input     (ElfBack* Back, AstNode* CurNode);
void elf_generate_output    (ElfBack* Back, AstNode* CurNode);
void elf_generate_if        (ElfBack* Back, AstNode* CurNode);
void elf_generate_while     (ElfBack* Back, AstNode* CurNode);
void elf_generate_call      (ElfBack* Back, AstNode* CurNode, bool RetValueMarker);
void elf_generate_return    (ElfBack* Back, AstNode* CurNode);
void elf_generate_announce  (ElfBack* Back, AstNode* CurNode);
void elf_generate_equation  (ElfBack* Back, AstNode* CurNode);
void elf_generate_expression    (ElfBack* Back, AstNode* CurNode);
void elf_generate_postorder     (ElfBack* Back, AstNode* CurNode, bool RetValueMarker);

void elf_set_rax_var_value      (ElfBack* Back, AstNode* CurNode);
void elf_set_rax_var_address    (ElfBack* Back, AstNode* CurNode);

void elf_generate_pop_var   (ElfBack* Back, AstNode* CurNode);
void elf_generate_push_var  (ElfBack* Back, AstNode* CurNode);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void elf_push_parameters (ElfBack* Back, AstNode* CurNode);
void elf_delete_function_parameters (ElfBack* Back, AstNode* CurNode);

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

int elf_find_var (ElfBack* Back, AstNode* CurNode);

AstNode* elf_find_parent_statement (ElfBack* Back, AstNode* CurNode);
bool elf_find_in_table (ElfBack* Back, CharT* varName, SVarTable* Table, int* RetIndex, bool* ParamMarker);

// bool my_find (const wchar_t* Name);
const wchar_t* my_wchar_find (ElfBack* Back, const wchar_t* Name);

//==================================================================================================================================================================

#define ELF_BACK_FUNC_HEAD_PARAMETERS SNode* CurNode, SElfBack* Back

#define ELF_CLEAN_TABLE() if (Back->table_cond != none) { elf_delete_var_table (Back); Back->table_cond = exist; }

//==================================================================================================================================================================

void make_elf_file (AstNode* Root, FILE* ExFile);

//==================================================================================================================================================================

void create_elf_header (AstNode* Root, FILE* ExFile);

void create_elf_body (AstNode* Root, FILE* ExFile);

//===================================================================================================================================================================

void prepare_name_label_to_jump (wchar_t* Label_name, size_t Address);

//===================================================================================================================================================================

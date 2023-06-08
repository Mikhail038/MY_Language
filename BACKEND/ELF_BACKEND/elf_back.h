#pragma once
//==================================================================================================================================================================
//29.04.2023
//==================================================================================================================================================================

#include "TLabel.h"
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
// #define QUADWORD_SIZE 32

const int MAX_JUMP_LABEL_SIZE = 50;
// #define MAX_JUMP_LABEL_SIZE 50

const int MAX_ELF_SIZE = 100000;
// #define MAX_ELF_SIZE 100000

//==================================================================================================================================================================

#define SET_BYTE(x) Array[cur_addr] = (x); cur_addr++;

#define SET_BYTE_2_BYTES(x)     \
    memcpy ((unsigned short int*) &(Array[cur_addr]),    \
                        &(x), sizeof (unsigned short int));  \
    cur_addr += 2;

#define SET_BYTE_4_BYTES(x)     \
    memcpy ((unsigned int*) &(Array[cur_addr]),    \
                        &(x), sizeof (unsigned int));  \
    cur_addr += 4;

#define SET_BYTE_8_BYTES(x)     \
    memcpy ((size_t*) &(Array[cur_addr]),    \
                        &(x), sizeof (size_t));  \
    cur_addr += 8;

#define FILL_2  SET_BYTE (0x00);

#define FILL_4  for (int cnt = 0; cnt != 3; ++cnt) { SET_BYTE (0x00); }

#define FILL_8  for (int cnt = 0; cnt != 7; ++cnt) { SET_BYTE (0x00); }

#define PASTE_8(x,y)  \
    (x) = cur_addr;    \
    memcpy ((size_t*) &(Array[(y)]), \
    &(x), sizeof (size_t));

#define PASTE_4(x,y)  \
    (x) = cur_addr;    \
    memcpy ((unsigned int*) &(Array[(y)]), \
    &(x), sizeof (unsigned int));

#define SKIP_8(x,y)   \
    size_t (x) = 0; \
    size_t (y) = cur_addr;    \
    cur_addr += 8; //we ll skip it now

#define SKIP_4(x,y)   \
    unsigned int (x) = 0; \
    size_t (y) = cur_addr;    \
    cur_addr += 4; //we ll skip it now

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

    std::unordered_map<const wchar_t*, TLabel> Labels;

public:
    ElfBack(FILE* ExFile, AstNode* CurNode);

    ~ElfBack();

    ElfBack(ElfBack& Another) = delete;
    ElfBack(ElfBack&&  Another) = delete;

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_push_imm (const int Number);
    void x86_push_reg (const int Register);
    void x86_push_from_addr_in_reg (const int Register);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_pop_to_reg (int Register);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_nop ();

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_call (int Shift);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_call_label (const wchar_t* Name);

    void x86_jump_label (const wchar_t* Name, const int JumpMode);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_ret ();

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_syscall ();

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_cmp_reg_reg    (int dstReg, int srcReg);
    void x86_cmp_stack  ();

    //------------------------  -----------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_jump_any (int Shift, int JmpMode);

    void x86_jump_near (int Shift, int JumpMode);
    void x86_jump_norm (int Shift, int JumpMode);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_mov_reg_reg    (const int dstReg, const int srcReg);
    void x86_mov_reg_imm    (const int dstReg, const int Number);
    void x86_mov_to_reg_from_addr_in_reg  (const int dstReg, const int srcReg);
    void x86_mov_to_reg_from_addr_in_reg_with_imm(const int dstReg, const int srcReg, int Shift);
    void x86_mov_to_addr_in_reg_from_reg  (const int dstReg, const int srcReg);
    void x86_mov_to_reg_from_imm_addr  (int dstReg, int MemAddr);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_add_reg_reg (int dstReg, int SrcReg);

    void x86_add_stack ();
    void x86_add_reg_imm (const int Register, const int Number);

    void x86_sub_reg_reg (int dstReg, int SrcReg);

    void x86_sub_stack ();
    void x86_sub_reg_imm (const int Register, const int Number);

    void x86_imul_stack ();
    void x86_idiv_stack ();

    void x86_inc (const int Reg);
    void x86_dec (const int Reg);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void set_hex_int    (const int Number);
    void set_hex_long   (const long Address);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_extra_dest_src_config    (int& dstReg, int& srcReg);
    void x86_extra_destination_config       (int& dstReg);

    void x86_extra_two_registers_config  (const int dstReg, const int srcReg);
    void x86_extra_one_register_config   (const int Reg, const int ZeroPoint);

    void x86_extra_exit ();

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_extra_paste_call_label (const wchar_t* Name);
    void x86_extra_paste_jump_label (const wchar_t* Name);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void x86_extra_make_input_func ();
    void x86_extra_make_input_func_body ();

    void x86_extra_make_output_func ();
    void x86_extra_make_output_func_body ();

    void x86_write_new_line ();

    //==================================================================================================================================================================

    void elf_head_start ();
    void elf_head_start_params ();
    void elf_head_program_header_params (const size_t FileVirtualAddress);
    void elf_head_shstrtable (  size_t& SegmentSize, size_t& addrSegmentSize,
                                size_t& SegmentFileSize, size_t& addrSegmentFileSize,
                                size_t& TableAddress, size_t& addrTableAddress,
                                size_t& TableLoadAddress, size_t& addrTableLoadAddress,
                                unsigned int& TextNameOffset, size_t& addrTextNameOffset,
                                unsigned int& TableNameOffset, size_t& addrTableNameOffset, const size_t FileVirtualAddress);

    void generate_elf_array (AstNode* Root);

    void elf_generate_code (AstNode* Root);

    void generate_main ();

    void elf_generate_statement (AstNode* CurNode);
    void elf_generate_function (AstNode* CurNode);
    void elf_generate_node (AstNode* CurNode);
    void elf_generate_op_node (AstNode* CurNode, bool RetValueMarker);
    void elf_generate_input (AstNode* CurNode);
    void elf_generate_output (AstNode* CurNode);
    void elf_generate_if (AstNode* CurNode);
    void elf_generate_while (AstNode* CurNode);
    void elf_generate_call (AstNode* CurNode, bool RetValueMarker);
    void elf_generate_return (AstNode* CurNode);
    void elf_generate_announce (AstNode* CurNode);
    void elf_generate_equation (AstNode* CurNode);
    void elf_generate_expression (AstNode* CurNode);
    void elf_generate_postorder (AstNode* CurNode, bool RetValueMarker);

    void elf_set_rax_var_value      (AstNode* CurNode);
    void elf_set_rax_var_address    (AstNode* CurNode);

    void elf_generate_pop_var   (AstNode* CurNode);
    void elf_generate_push_var  (AstNode* CurNode);

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void elf_push_parameters (AstNode* CurNode);
    void elf_delete_function_parameters (AstNode* CurNode);

    void elf_pop_parameters ();

    void elf_incr_top_reg ();

    void elf_standard_if_jump (int JumpMode);

    void elf_write_command (ECommandNums eCommand, FILE* File);

    void elf_add_to_var_table (AstNode* CurNode, bool ParamMarker);
    void elf_create_new_var_table (bool ParamMarker);
    void elf_create_param_var_table (AstNode* CurNode);
    void elf_delete_var_table ();

    size_t elf_find_all_new_vars    (AstNode* CurNode);
    size_t elf_find_new_var     (AstNode* CurNode);

    int elf_find_var (AstNode* CurNode);

    AstNode* elf_find_parent_statement (AstNode* CurNode);
    bool elf_find_in_table (CharT* varName, SVarTable* Table, int* RetIndex, bool* ParamMarker);

    // bool my_find (const wchar_t* Name);
    const wchar_t* my_find (const wchar_t* Name);
};


//==================================================================================================================================================================

#define ELF_BACK_FUNC_HEAD_PARAMETERS SNode* CurNode, SElfBack* Back

#define ELF_CLEAN_TABLE() if (table_cond != none) { elf_delete_var_table (); table_cond = exist; }

//==================================================================================================================================================================

void make_elf_file (AstNode* Root, FILE* ExFile);

//==================================================================================================================================================================

void create_elf_header (AstNode* Root, FILE* ExFile);

void create_elf_body (AstNode* Root, FILE* ExFile);

//===================================================================================================================================================================

void prepare_name_label_to_jump (wchar_t* Label_name, size_t Address);

//===================================================================================================================================================================

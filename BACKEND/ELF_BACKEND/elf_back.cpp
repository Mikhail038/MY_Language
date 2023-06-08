//==================================================================================================================================================================
//29.04.2023
//==================================================================================================================================================================

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <iterator>
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
#include "stackT.h"
#include <bits/types/FILE.h>
#include <cstdio>

//=============================================================================================================================================================================

ElfBack::ElfBack(FILE* ExFile, AstNode* Root)
{
    Funcs         = (SBackFuncTable*)     calloc (1, sizeof (SBackFuncTable));
    Funcs->Table  = (SBackFunc*)          calloc (MAX_FUNCS_ARRAY, sizeof (SBackFunc));
    Funcs->top_index = 0;

    VarStack      = (SStack<SVarTable*>*) calloc (1, sizeof (SStack<SVarTable*>));

    file = ExFile;

    table_cond = none;

    stack_constructor (VarStack, 4);

    Array = (char*) calloc(sizeof (char), MAX_ELF_SIZE);
    cur_addr = 0;
}

ElfBack::~ElfBack()
{
    free_tables (VarStack);

    free (Funcs->Table);
    free (Funcs);

    free (Array);
}

//=============================================================================================================================================================================


//=============================================================================================================================================================================

void make_elf_file (AstNode* Root, FILE* ExFile)
{
    MY_LOUD_ASSERT (ExFile != NULL);

    ElfBack Back {ExFile, Root};

    //generate_user_functions (Root, Back);

    Back.generate_elf_array(Root);

    fwrite (Back.Array, sizeof (char), Back.cur_addr, Back.file);

    // for (int i = 0; i < Back->Funcs->top_index; i++)
    // {
    //     printf ("'%ls'[%d]\n", Back->Funcs->Table[i]);
    // }

    return;
}

//=============================================================================================================================================================================
//Make ELF//
//=============================================================================================================================================================================

void ElfBack::elf_head_start ()
{
    SET_BYTE (0x7f); //MAGIC SIGNATURE

    SET_BYTE ('E');
    SET_BYTE ('L');
    SET_BYTE ('F');

    SET_BYTE (0x02); //Class (64-bit = 2)

    SET_BYTE (0x01); //Endian (little endian = 1)

    SET_BYTE (0x01); //Elf Version (only correct = 1)

    SET_BYTE (0x00); //OS ABI (System V (UNIX) = 0)

    SET_BYTE (0x00); //ABI version (none = 0)

    FILL_8;     //7 padding bytes

    SET_BYTE (0x02); //Type (executable = 2)
    FILL_2;     //nothing

    SET_BYTE (0x3e); //Machine (x86_64 = 3e)
    FILL_2;     //nothing

    SET_BYTE (0x01); //Version (only correct = 1)
    FILL_4;     //nothing
}

void ElfBack::elf_head_start_params ()
{
    SET_BYTE (0x00); //Flags (no flags = 0)
    FILL_4;     //nothing

    unsigned short int  HeaderSize = 64; //std
    SET_BYTE_2_BYTES (HeaderSize);

    unsigned short int  ProgramHeaderSize = 56; //std
    SET_BYTE_2_BYTES(ProgramHeaderSize);

    unsigned short int  ProgramHeadersCnt = 1;
    SET_BYTE_2_BYTES(ProgramHeadersCnt);

    unsigned short int  SectionHeadersSize = 64; //std
    SET_BYTE_2_BYTES(SectionHeadersSize);

    unsigned short int  SectionHeadersCnt = 3; // maybe not
    SET_BYTE_2_BYTES(SectionHeadersCnt);

    unsigned short int  ShstrTabIndex  = 2; // maybe not
    SET_BYTE_2_BYTES(ShstrTabIndex);
}

void ElfBack::elf_head_program_header_params (const size_t FileVirtualAddress)
{
    SET_BYTE (0x01); //Segment type (load = 0)
    FILL_4;     //nothing

    SET_BYTE (0x05); //Segment flag (r w x = 0 1 2)
    FILL_4;     //nothing

    SET_BYTE (0x00); //Segment offset (0)
    FILL_8;     //nothing

    memcpy ((size_t*) &(Array[cur_addr]),
            &FileVirtualAddress, sizeof (size_t));  //Virtual address
    cur_addr += 8;

    memcpy ((size_t*) &(Array[cur_addr]),
            &FileVirtualAddress, sizeof (size_t));  //Physical address (same as virtual)
    cur_addr += 8;
}

void ElfBack::elf_head_shstrtable ( size_t& SegmentSize, size_t& addrSegmentSize,
                                    size_t& SegmentFileSize, size_t& addrSegmentFileSize,
                                    size_t& TableAddress, size_t& addrTableAddress,
                                    size_t& TableLoadAddress, size_t& addrTableLoadAddress,
                                    unsigned int& TextNameOffset, size_t& addrTextNameOffset,
                                    unsigned int& TableNameOffset, size_t& addrTableNameOffset, const size_t FileVirtualAddress)
{
    PASTE_8(SegmentSize, addrSegmentSize);

    PASTE_8(SegmentFileSize, addrSegmentFileSize);

    PASTE_8(TableAddress, addrTableAddress);

    TableLoadAddress = TableAddress + FileVirtualAddress;
    memcpy ((size_t*) &(Array[addrTableLoadAddress]),
    &TableLoadAddress, sizeof (size_t));


    SET_BYTE (0x00); //begin section header string table

    TextNameOffset = cur_addr - SegmentSize;
    memcpy ((unsigned int*) &(Array[addrTextNameOffset]),
    &TextNameOffset, sizeof (unsigned int));

    SET_BYTE ('.');
    SET_BYTE ('t');
    SET_BYTE ('e');
    SET_BYTE ('x');
    SET_BYTE ('t');
    SET_BYTE (0x00); //end

    TableNameOffset = cur_addr - SegmentSize;
    memcpy ((unsigned int*) &(Array[addrTableNameOffset]),
    &TableNameOffset, sizeof (unsigned int));

    SET_BYTE ('.');
    SET_BYTE ('s');
    SET_BYTE ('h');
    SET_BYTE ('s');
    SET_BYTE ('t');
    SET_BYTE ('r');
    SET_BYTE ('t');
    SET_BYTE ('a');
    SET_BYTE ('b');
    SET_BYTE (0x00); //end
}

void ElfBack::generate_elf_array (AstNode* Root)
{
    elf_head_start ();

    size_t FileVirtualAddress = 4096 * 40; //40 is random number, 4096 is for alignment

    SKIP_8(EntryVA, addrEntryVA);

    SKIP_8(ProgramHeadersStart, addrProgramHeadersStart);

    SKIP_8(SectionHeadersStart, addrSectionHeadersStart);

    elf_head_start_params ();

    //=======================================================================

    PASTE_8(ProgramHeadersStart, addrProgramHeadersStart);

    elf_head_program_header_params (FileVirtualAddress);

    SKIP_8(SegmentSize, addrSegmentSize);

    SKIP_8(SegmentFileSize, addrSegmentFileSize);

    SET_BYTE (0x00);
    SET_BYTE (0x00);
    SET_BYTE (0x20); //Alignment
    SET_BYTE (0x00);
    SET_BYTE (0x00);
    SET_BYTE (0x00);
    SET_BYTE (0x00);
    SET_BYTE (0x00);

    //=======================================================================

    PASTE_8(SectionHeadersStart, addrSectionHeadersStart);

    //=======================================================================
    //null

    for (int cnt_null_header = 0; cnt_null_header < 64; cnt_null_header++)
    {
        SET_BYTE (0x00);
    }

    //=======================================================================
    //text

    SKIP_4(TextNameOffset, addrTextNameOffset);

    SET_BYTE (0x01); //Section type (bits = 1)
    FILL_4;     //nothing

    SET_BYTE (0x06); //Section flags (r w x )
    FILL_8;     //nothing

    size_t addrAnotherEntryVA = cur_addr;
    cur_addr += 8; //skip

    SKIP_8(TextOffset, addrTextOffset);

    SKIP_8(TextSize, addrTextSize);

    SET_BYTE (0x00); //Section index
    FILL_4;

    SET_BYTE (0x00); //Dop info
    FILL_4;

    SET_BYTE (0x10); //Alignment
    FILL_8;

    SET_BYTE (0x00); //Dop sizes
    FILL_8;

    //=======================================================================
    //strtable

    SKIP_4(TableNameOffset, addrTableNameOffset);

    SET_BYTE (0x03); //Section type (strtable = 1)
    FILL_4;     //nothing

    SET_BYTE (0x00);
    FILL_8;

    SKIP_8(TableLoadAddress, addrTableLoadAddress);

    SKIP_8(TableAddress, addrTableAddress);

    SKIP_8(TableSize, addrTableSize);

    SET_BYTE (0x00); //Section index
    FILL_4;

    SET_BYTE (0x00); //Dop info
    FILL_4;

    SET_BYTE (0x01); //Alignment
    FILL_8;

    SET_BYTE (0x00); //Dop sizes
    FILL_8;

    //=======================================================================
    //entry

    EntryVA = cur_addr + FileVirtualAddress;
    memcpy ((size_t*) &(Array[addrEntryVA]),
    &EntryVA, sizeof (size_t));

    memcpy ((size_t*) &(Array[addrAnotherEntryVA]),
    &EntryVA, sizeof (size_t));

    //=======================================================================
    //.code

    PASTE_8(TextOffset, addrTextOffset);

    //----------------------------------------------------------------------

    elf_generate_code (Root);

    //----------------------------------------------------------------------

    TextSize = cur_addr - TextOffset;
    memcpy ((size_t*) &(Array[addrTextSize]),
    &TextSize, sizeof (size_t));

    //=======================================================================
    //shstrtable

    elf_head_shstrtable (SegmentSize, addrSegmentSize,
                        SegmentFileSize, addrSegmentFileSize,
                        TableAddress, addrTableAddress,
                        TableLoadAddress, addrTableLoadAddress,
                        TextNameOffset, addrTextNameOffset,
                        TableNameOffset, addrTableNameOffset, FileVirtualAddress);

    //=======================================================================

    TableSize = cur_addr - SegmentSize;
    memcpy ((size_t*) &(Array[addrTableSize]),
    &TableSize, sizeof (size_t));

    return;
}

//=============================================================================================================================================================================
#define VAR_SIZE 8

#define BUFFER_SIZE 20

void ElfBack::elf_generate_code (AstNode* Root)
{
    x86_call_label(MAIN_LBL);

    x86_extra_exit();

    buffer = cur_addr;

    for (size_t cnt = 0; cnt < BUFFER_SIZE; ++cnt)
    {
        SET_BYTE(0x00);
    }

    x86_extra_make_input_func ();

    x86_extra_make_output_func ();

    elf_generate_statement(Root);

    return;
}

void ElfBack::elf_generate_statement (AstNode* CurNode)
{
    if (CurNode->left != NULL)
    {
        elf_generate_op_node (CurNode->left, false);
    }

    if (CurNode->right != NULL)
    {
        elf_generate_statement (CurNode->right);
    }

    return;
}

void ElfBack::elf_generate_function (AstNode* CurNode)
{

    if (wcscoll (CurNode->left->data.var, MAIN_LBL) == 0)
    {
        func_cond = main_f;
    }
    else
    {
        func_cond = any_f;
    }

    x86_extra_paste_call_label(CurNode->left->data.var);
    // fprintf (stdout, LABEL "|%s|:\n", CurNode->left->data.var);

    Funcs->Table[Funcs->top_index].Name = CurNode->left->data.var;

    table_cond = none;
    elf_create_param_var_table (CurNode->left->right);

    size_t AmountVars = 0;

    SVarTable* Table = NULL;
    if (VarStack->size != 0)
    {

        pop_from_stack (VarStack, &Table);

        AmountVars = elf_find_all_new_vars(CurNode->right);

        Table->amount = AmountVars;

        push_in_stack(VarStack, Table);

        // printf ("Vars Amount: [%d]\n", AmountVars);
    }

    x86_push_reg(rbp);
    x86_mov_reg_reg(rbp, rsp);

    x86_sub_reg_imm(rbp, AmountVars * VAR_SIZE);

    x86_mov_reg_reg(rsp, rbp);

    Funcs->top_index++;


    int old_delta_rbp = delta_rbp;
    delta_rbp = (AmountVars) * VAR_SIZE;


    elf_generate_statement (CurNode->right);
    ELF_CLEAN_TABLE ();

    delta_rbp = old_delta_rbp;



    x86_add_reg_imm(rbp, AmountVars * VAR_SIZE);

    x86_pop_to_reg(rbp);
    x86_mov_reg_reg(rsp, rbp);

    return;
}

void ElfBack::elf_generate_node (AstNode* CurNode)
{
    if (CurNode->category == CategoryOperation)
    {
        elf_generate_op_node (CurNode, true);
    }

    if (CurNode->category == CategoryValue)
    {
        x86_push_imm(CurNode->data.val);
    }

    if (CurNode->category == CategoryLine && CurNode->type == TypeVariable)
    {
        elf_generate_push_var (CurNode);
    }

    return;
}

void ElfBack::elf_generate_op_node (AstNode* CurNode, bool RetValueMarker)
{
    switch (CurNode->type)
    {
        #define DEF_OP(d_type, d_condition, d_tokenize, d_print, d_switch) \
        case d_type: \
            d_switch; \
            break;

        #include "ELF_Operations.h"

        #undef DEF_OP
    }

    return;
}

void ElfBack::elf_generate_announce (AstNode* CurNode)
{
    elf_generate_expression (CurNode->right);

    elf_add_to_var_table (CurNode->left, false);

    elf_generate_pop_var (CurNode->left);

    // elf_incr_top_reg ();

    return;
}

void ElfBack::elf_generate_equation (AstNode* CurNode)
{
    elf_generate_expression (CurNode->right);

    elf_generate_pop_var (CurNode->left);

    return;
}

void ElfBack::elf_generate_input (AstNode* CurNode)
{
    AstNode* Node = CurNode->left;

    do
    {
        elf_set_rax_var_address (Node->left);

        x86_call_label(INP_LBL);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void ElfBack::elf_generate_output (AstNode* CurNode)
{
    AstNode* Node = CurNode->left;

    do
    {
        elf_set_rax_var_value (Node->left);

        x86_call_label(OUT_LBL);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void ElfBack::elf_generate_if (AstNode* CurNode)
{
    elf_generate_expression (CurNode->left);

    x86_push_imm(MY_FALSE);

    wchar_t* Label_true_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    prepare_name_label_to_jump (Label_true_name, cur_addr);

    x86_cmp_stack();
    x86_jump_label(Label_true_name, x86_je);


    CurNode = CurNode->right;

    table_cond = none;
    elf_generate_statement (CurNode->left);
    ELF_CLEAN_TABLE ();

    wchar_t* Label_false_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    prepare_name_label_to_jump (Label_false_name, cur_addr);

    x86_jump_label(Label_false_name, x86_jmp);

    x86_extra_paste_jump_label(Label_true_name);

    CurNode = CurNode->right;

    if (CurNode != NULL)
    {
        if (CurNode->category == CategoryOperation && CurNode->type == TypeIf)
        {
            elf_generate_if (CurNode);
        }
        else
        {
            table_cond = none;
            elf_generate_statement (CurNode);
            ELF_CLEAN_TABLE ();
        }
    }

    x86_extra_paste_jump_label(Label_false_name);

    free (Label_true_name);
    free (Label_false_name);

    return;
}

void ElfBack::elf_generate_while (AstNode* CurNode)
{
    wchar_t* Label_body_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    prepare_name_label_to_jump (Label_body_name, cur_addr);

    x86_extra_paste_jump_label(Label_body_name);
    // int Label_1 = label_cnt;
    // fprintf (file,  LABEL "%d:\n", Label_1);
    // label_cnt++;

    // x86_nop();
    elf_generate_expression (CurNode->left);
    // x86_nop();

    x86_push_imm(MY_FALSE);

    wchar_t* Label_end_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    prepare_name_label_to_jump (Label_end_name, cur_addr);

    x86_cmp_stack();
    x86_jump_label(Label_end_name, x86_je);

    // PUT (je);
    // int Label_2 = label_cnt;
    // fprintf (file, " " LABEL "%d\n", Label_2);
    // label_cnt++;

    // x86_nop();
    table_cond = none;
    elf_generate_statement (CurNode->right);
    ELF_CLEAN_TABLE ();
    // x86_nop();

    x86_jump_label(Label_body_name, x86_jmp);

    x86_extra_paste_jump_label(Label_end_name);

    free (Label_body_name);
    free (Label_end_name);

    return;
}

void ElfBack::elf_generate_call (AstNode* CurNode, bool RetValueMarker)
{
    // x86_push_r(eTOP_REG);

    elf_push_parameters (CurNode->right);

    // x86_push_r(eTOP_REG);

    // x86_pop_r(ELF_SHIFT_REG);

    x86_call_label(CurNode->left->data.var);

    elf_delete_function_parameters (CurNode->right);

    // PUT (call);
    // fprintf (file, " " LABEL "%ls\n", CurNode->left->data.var);

    // x86_pop_r(eTOP_REG);

    if (RetValueMarker == true)
    {
        // printf (KRED "WAAGH!\n" KNRM);
        x86_push_reg(ELF_FUNC_RET_REG);
    }
    else
    {
        // printf (KGRN "no\n" KNRM);
    }

    return;
}

void ElfBack::elf_generate_return (AstNode* CurNode)
{
    elf_generate_expression (CurNode->left);

    x86_pop_to_reg(ELF_FUNC_RET_REG);

    size_t AmountVars = 0;

    SVarTable* Table = NULL;

    if (VarStack->size != 0)
    {
        peek_from_stack (VarStack, &Table);
        AmountVars = Table->amount;
    }
    x86_add_reg_imm(rbp, AmountVars * VAR_SIZE);

    x86_mov_reg_reg(rsp, rbp);

    x86_pop_to_reg(rbp);

    x86_ret();

    return;
}

void ElfBack::elf_generate_expression (AstNode* CurNode)
{
    elf_generate_postorder (CurNode, true);

    return;
}

void ElfBack::elf_generate_postorder (AstNode* CurNode, bool RetValueMarker)
{
    if (!(CurNode->category == CategoryOperation && CurNode->type == TypeLinkerCall) && CurNode->left != NULL)
    {
        elf_generate_postorder (CurNode->left, RetValueMarker);
    }

    if (!(CurNode->category == CategoryOperation && CurNode->type == TypeLinkerCall) && CurNode->right != NULL)
    {
        elf_generate_postorder (CurNode->right, RetValueMarker);
        // x86_push_r(ELF_FUNC_RET_REG);
    }

    elf_generate_node (CurNode);

    return;
}

//=============================================================================================================================================================================

void ElfBack::elf_set_rax_var_value (AstNode* CurNode)
{
    int Index = elf_find_var (CurNode);

    // printf("value param = ");
    if (Index < 0)
    {
        // printf("Index: %d Delta_rbp: %d Final: %d\n", Index, delta_rbp, delta_rbp - Index * VAR_SIZE);
        x86_mov_to_reg_from_addr_in_reg_with_imm(rax, rbp, delta_rbp - Index * VAR_SIZE);

        return;
    }

    // printf("Index: %d\n", Index);
    x86_mov_to_reg_from_addr_in_reg_with_imm(rax, rbp, Index);

    return;
}

void ElfBack::elf_set_rax_var_address (AstNode* CurNode)
{
    int Index = elf_find_var (CurNode);
    // printf("addr  param = ");

    x86_mov_reg_reg(rax, rbp);

    if (Index < 0)
    {
        // printf("Index: %d Delta_rbp: %d Final: %d\n", Index, delta_rbp, delta_rbp - Index * VAR_SIZE);
        x86_add_reg_imm(rax, delta_rbp - Index * VAR_SIZE);

        return;
    }

    // printf("Index: %d\n", Index);
    x86_add_reg_imm(rax, Index);

    return;
}

void ElfBack::elf_generate_pop_var (AstNode* CurNode)
{
    elf_set_rax_var_address(CurNode);

    x86_pop_to_reg(FIRST_REG);

    x86_mov_to_addr_in_reg_from_reg(rax, FIRST_REG);

//     int Index = elf_find_var (CurNode);
//
//     x86_mov_r_r(eCOUNT_REG, ELF_SHIFT_REG);
//     x86_add_i(eCOUNT_REG, Index);
//
//     x86_pop_r(FIRST_REG);
//     x86_mov_IrI_r(eCOUNT_REG, FIRST_REG);

    // PUT (pop);
    // fprintf (file, " [" COUNT_REG "]\n\n"); // this was

    return;
}

void ElfBack::elf_generate_push_var (AstNode* CurNode)
{
    elf_set_rax_var_value(CurNode);

    x86_push_reg(rax);

//     int Index = elf_find_var (CurNode);
//
//     x86_mov_r_r(eCOUNT_REG, ELF_SHIFT_REG);
//     x86_add_i(eCOUNT_REG, Index);
//
//     x86_push_IrI(eCOUNT_REG);

    return;
}

//=============================================================================================================================================================================

void ElfBack::elf_push_parameters (AstNode* CurNode)
{
    if (CurNode->right != NULL)
    {
        elf_push_parameters (CurNode->right);
    }

    if (CurNode != NULL && CurNode->left != NULL)
    {
        elf_generate_expression (CurNode->left);
    }

    return;
}

void ElfBack::elf_delete_function_parameters (AstNode* CurNode)
{
    if (CurNode->right != NULL)
    {
        elf_delete_function_parameters (CurNode->right);
    }

    if (CurNode != NULL && CurNode->left != NULL)
    {
        x86_add_reg_imm(rsp, VAR_SIZE);
    }

    return;
}

void ElfBack::elf_standard_if_jump (int JumpMode)
{
    const int STD_JUMP_FIRST_SHIFT  = 6;
    const int STD_JUMP_SECOND_SHIFT = 4;

    if (JumpMode != x86_jmp)
    {
        x86_cmp_stack ();
    }

    x86_jump_any(STD_JUMP_FIRST_SHIFT, JumpMode);

    // fprintf (file, " " LABEL "%d\n", label_cnt);
    // label_cnt++;

    x86_push_imm(MY_FALSE);

    // PUT (push);
    // fprintf (file, " %d\n", MY_FALSE);

    x86_jump_any(STD_JUMP_SECOND_SHIFT, x86_jmp);

    // PUT (jump);
    // fprintf (file, " " LABEL "%d\n", label_cnt);

    // fprintf (file, LABEL "%d:\n", label_cnt - 1);

    x86_push_imm(MY_TRUE);

    // PUT (push);
    // fprintf (file, " %d\n", MY_TRUE);

//     fprintf (file, LABEL "%d:\n", label_cnt);
//
//     label_cnt++;

    return;
}

void ElfBack::elf_write_command (ECommandNums eCommand, FILE* File)
{
    #define DEF_CMD(def_line, def_enum, ...) \
    else if (eCommand == def_enum) \
    { \
        fprintf (File, TAB  def_line); \
        return; \
    }

    if (false) {}

    #include "commands.h"

    else
    {
        MY_LOUD_ASSERT (false);
    }

    #undef DEF_CMD
}

//=============================================================================================================================================================================

void ElfBack:: elf_add_to_var_table (AstNode* CurNode, bool ParamMarker)
{
    if (table_cond == EVarTableConditions::none)
    {
        elf_create_new_var_table (ParamMarker);
    }

    MY_LOUD_ASSERT (CurNode->category == CategoryLine && CurNode->type == TypeVariable);

    SVarTable* Table = NULL;
    peek_from_stack (VarStack, &Table);

    Table->Arr[Table->cur_size].name  = CurNode->data.var; //copy address from node

    if (ParamMarker == false)
    {
        #ifdef VAR_OVERSEER
        printf (KYLW "Amount: %d CurSize: %d Param: %d Final: %d => " KNRM, Table->amount, Table->cur_size, Table->amount_param, Table->amount - (Table->cur_size - Table->amount_param) - 1);
        #endif

        Table->Arr[Table->cur_size].index = (Table->amount - (Table->cur_size - Table->amount_param) - 1) * VAR_SIZE;

        #ifdef VAR_OVERSEER
        printf (KYLW "index <%d>\n" KNRM, Table->Arr[Table->cur_size].index);
        #endif

        // Table->param_marker = false;
    }
    else
    {
        Table->Arr[Table->cur_size].index = - (Table->cur_size + 2); // another way to mark params of function

        #ifdef VAR_OVERSEER
        printf (KYLW "index |%d|\n" KNRM, Table->Arr[Table->cur_size].index);
        #endif

        Table->amount_param++;
    }

    Table->cur_size++;

    return;
}

void ElfBack::elf_create_new_var_table (bool ParamMarker)
{
    PRINT_DEBUG_INFO;

    delta_rbp = 0;

    SVarTable* NewTable = (SVarTable*)  calloc (1,                  sizeof (SVarTable));
    NewTable->Arr       = (SVarAccord*) calloc (VAR_TABLE_CAPACITY, sizeof (SVarAccord));
    NewTable->cur_size = 0;

    NewTable->param_marker = ParamMarker;

//     if (ParamMarker == true)
//     {
//         // SNode* Node = elf_find_parent_statement (CurNode);
//
//         // MY_LOUD_ASSERT (Node != NULL);
//
//         // NewTable->amount = elf_find_new_vars (Node);
//         MY_LOUD_ASSERT (CurNode->type != TypeLinkerParameter);
//         printf("amount %d\n", NewTable->amount);
//     }
//     else
//     {
//         NewTable->amount = elf_find_new_vars (CurNode);
//         printf("amount %d\n", NewTable->amount);
//     }

    if (ParamMarker == true)
    {
        // printf("func params\n");
    }

    push_in_stack (VarStack, NewTable);

    table_cond = exist;

    return;
}

size_t ElfBack::elf_find_all_new_vars (AstNode* CurNode)
{
    return elf_find_new_var(CurNode);
}

size_t ElfBack::elf_find_new_var (AstNode* CurNode)
{
    size_t OneMoreVar = 0;

    if (CurNode->left != NULL && CurNode->left->type == TypeLinkerAnnounce)
    {
        OneMoreVar = 1;
    }

    if (CurNode->right == NULL)
    {
        return OneMoreVar;
    }

    return OneMoreVar + elf_find_new_var(CurNode->right);
}


void ElfBack::elf_create_param_var_table (AstNode* CurNode)
{
    PRINT_DEBUG_INFO;

    bool TableCreationMark = false;

    do
    {
        if (CurNode->left != NULL)
        {
            elf_add_to_var_table (CurNode->left->left, true);
            Funcs->Table[Funcs->top_index].parameters++;
            TableCreationMark = true;
        }

        CurNode = CurNode->right;
    } while (CurNode != NULL);

    if (TableCreationMark == false)
    {
        // printf (KRED "forced\n" KNRM);
        elf_create_new_var_table(true);
    }

    return;
}

void ElfBack::elf_delete_var_table ()
{
    PRINT_DEBUG_INFO;

    SVarTable* Table = NULL;

    pop_from_stack (VarStack, &Table);

    free (Table->Arr);

    free (Table);

    return;
}

AstNode* ElfBack::elf_find_parent_statement (AstNode* CurNode)
{
    AstNode* Node = NULL;
    while (CurNode->parent != NULL)
    {
        CurNode = CurNode->parent;
        if (CurNode->right != NULL && CurNode->category == CategoryOperation && CurNode->type == TypeLinkerStatement)
        {
            Node = CurNode;
        }
    };

    return Node;
}

int ElfBack::elf_find_var (AstNode* CurNode)
{
    int RetIndex = WrongValue;
    bool ParamMarker = false;

    MY_LOUD_ASSERT (VarStack->size != 0);
    MY_LOUD_ASSERT (CurNode->category == CategoryLine && CurNode->type == TypeVariable);

    SVarTable* Table = NULL;

    int depth = 1;
    do
    {
        Table = VarStack->data[VarStack->size - depth];
        //printf ("=%p=%d/%d=\n", Table, depth, VarStack->size);
        depth++;
    } while ((elf_find_in_table (CurNode->data.var, Table, &RetIndex, &ParamMarker) == false) && (depth <= VarStack->size));

    if (RetIndex == WrongValue)
    {
        printf ("==ERROR==\n""No '%ls' found!\n", CurNode->data.var);
        MY_LOUD_ASSERT (0);
    }

    return RetIndex;
}

//=============================================================================================================================================================================

bool ElfBack::elf_find_in_table (CharT* varName, SVarTable* Table, int* RetIndex, bool* ParamMarker)
{
    MY_LOUD_ASSERT (Table != NULL);

    for (int counter = 0; counter < Table->cur_size; ++counter)
    {
        *ParamMarker = Table->param_marker;

        if (wcscmp (varName, Table->Arr[counter].name) == 0)
        {
            *RetIndex = Table->Arr[counter].index;

            return true;
        }
    }

    return false;
}

//=============================================================================================================================================================================

void prepare_name_label_to_jump (wchar_t* Label_name, size_t Address)
{
    // std::cout << Address << std::endl;

    Address += 1;

    size_t cnt = 0;
    while (Address >= 10)
    {
        Label_name[cnt] = (wchar_t) (Address % 10);

        Address /= 10;

        ++cnt;
    }

    Label_name[cnt] = (wchar_t) (Address % 10);
    cnt++;

    Label_name[cnt] = L'\0';


    Label_name = wcscat(Label_name, L"_w_1");

//     for (int i = 0; i < cnt + 4; ++i)
//     {
//         printf("[%x]", Label_name[i]);
//     }
//
//     std::cout << Address << std::endl;
//     std::cout << cnt << std::endl;
//     std::cout << Label_name << std::endl;
//     printf ("|%ls|\n", Label_name);
}

//=============================================================================================================================================================================

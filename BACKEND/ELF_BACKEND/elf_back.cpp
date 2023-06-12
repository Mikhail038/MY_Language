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
#include "elf_header_plus_tools.h"
#include "front.h"


#include "asm.h"
#include "proc.h"
#include "stackT.h"
#include <bits/types/FILE.h>
#include <cstdio>

//=============================================================================================================================================================================

void construct_elf_back (ElfBack* Back, FILE* ExFile)
{
    Back->Funcs         = (SBackFuncTable*)     calloc (1, sizeof (SBackFuncTable));
    Back->Funcs->Table  = (SBackFunc*)          calloc (MAX_FUNCS_ARRAY, sizeof (SBackFunc));
    Back->Funcs->top_index = 0;

    Back->VarStack      = (SStack<SVarTable*>*) calloc (1, sizeof (SStack<SVarTable*>));

    Back->file = ExFile;

    Back->table_cond = none;

    stack_constructor (Back->VarStack, 4);

    Back->Array = (char*) calloc(sizeof (char), MAX_ELF_SIZE);
    Back->cur_addr = 0;
}

void destruct_elf_back (ElfBack* Back)
{
    free_tables (Back->VarStack);

    free (Back->Funcs->Table);
    free (Back->Funcs);

    free (Back->Array);
}

//=============================================================================================================================================================================

void make_elf_file (AstNode* Root, FILE* ExFile)
{
    MY_LOUD_ASSERT (ExFile != NULL);

    ElfBack Back;

    construct_elf_back (&Back, ExFile);

    //generate_user_functions (Root, Back);

    generate_elf_array (&Back, Root);

    fwrite (Back.Array, sizeof (char), Back.cur_addr, Back.file);

    // for (int i = 0; i < Back->Funcs->top_index; i++)
    // {
    //     printf ("'%ls'[%d]\n", Back->Funcs->Table[i]);
    // }

    destruct_elf_back (&Back);

    return;
}

//=============================================================================================================================================================================
//Make ELF//
//=============================================================================================================================================================================

void elf_head_start (ElfBack* Back)
{
    set_one_byte(Back, 0x7f); //MAGIC SIGNATURE

    set_one_byte(Back, 'E');
    set_one_byte(Back, 'L');
    set_one_byte(Back, 'F');

    set_one_byte(Back, 0x02); //Class (64-bit = 2)

    set_one_byte(Back, 0x01); //Endian (little endian = 1)

    set_one_byte(Back, 0x01); //Elf Version (only correct = 1)

    set_one_byte(Back, 0x00); //OS ABI (System V (UNIX) = 0)

    set_one_byte(Back, 0x00); //ABI version (none = 0)
    align_one_byte(Back, 8);  //7 padding bytes

    set_one_byte(Back, 0x02); //Type (executable = 2)
    align_one_byte(Back, 2);  //1 padding byte

    set_one_byte(Back, 0x3e); //Machine (x86_64 = 3e)
    align_one_byte(Back, 2);  //1 padding byte

    set_one_byte(Back, 0x01); //Version (only correct = 1)
    align_one_byte(Back, 4);  //3 padding bytes
}

void elf_head_start_params (ElfBack* Back)
{
    set_one_byte(Back, 0x00); //Flags (no flags = 0)
    align_one_byte(Back, 4);

    unsigned short int  HeaderSize = 64; //std
    set_bytes(Back, HeaderSize);

    unsigned short int  ProgramHeaderSize = 56; //std
    set_bytes(Back, ProgramHeaderSize);

    unsigned short int  ProgramHeadersCnt = 1;
    set_bytes(Back, ProgramHeadersCnt);

    unsigned short int  SectionHeadersSize = 64; //std
    set_bytes(Back, SectionHeadersSize);

    unsigned short int  SectionHeadersCnt = 3; // maybe not
    set_bytes(Back, SectionHeadersCnt);

    unsigned short int  ShstrTabIndex  = 2; // maybe not
    set_bytes(Back, ShstrTabIndex);
}

void elf_head_program_header_params (ElfBack* Back, const size_t FileVirtualAddress)
{
    set_one_byte(Back, 0x01); //Segment type (load = 0)
    align_one_byte(Back, 4);

    set_one_byte(Back, 0x05); //Segment flag (r w x = 0 1 2)
    align_one_byte(Back, 4);

    set_one_byte(Back, 0x00); //Segment offset (0)
    align_one_byte(Back, 8);

    memcpy ((size_t*) &(Back->Array[Back->cur_addr]),
            &FileVirtualAddress, sizeof (size_t));  //Virtual address
    Back->cur_addr += 8;

    memcpy ((size_t*) &(Back->Array[Back->cur_addr]),
            &FileVirtualAddress, sizeof (size_t));  //Physical address (same as virtual)
    Back->cur_addr += 8;
}

void elf_head_shstrtable (ElfBack* Back, size_t& SegmentSize, size_t& addrSegmentSize,
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
    memcpy ((size_t*) &(Back->Array[addrTableLoadAddress]),
    &TableLoadAddress, sizeof (size_t));


    set_one_byte(Back, 0x00); //begin section header string table

    TextNameOffset = Back->cur_addr - SegmentSize;
    memcpy ((unsigned int*) &(Back->Array[addrTextNameOffset]),
    &TextNameOffset, sizeof (unsigned int));

    set_one_byte(Back, '.');
    set_one_byte(Back, 't');
    set_one_byte(Back, 'e');
    set_one_byte(Back, 'x');
    set_one_byte(Back, 't');
    set_one_byte(Back, 0x00); //end

    TableNameOffset = Back->cur_addr - SegmentSize;
    memcpy ((unsigned int*) &(Back->Array[addrTableNameOffset]),
    &TableNameOffset, sizeof (unsigned int));

    set_one_byte(Back, '.');
    set_one_byte(Back, 's');
    set_one_byte(Back, 'h');
    set_one_byte(Back, 's');
    set_one_byte(Back, 't');
    set_one_byte(Back, 'r');
    set_one_byte(Back, 't');
    set_one_byte(Back, 'a');
    set_one_byte(Back, 'b');
    set_one_byte(Back, 0x00); //end
}

void generate_elf_array (ElfBack* Back, AstNode* Root)
{
    elf_head_start (Back);

    size_t FileVirtualAddress = 4096 * 40; //40 is random number, 4096 is for alignment

    SKIP_8(EntryVA, addrEntryVA);

    SKIP_8(ProgramHeadersStart, addrProgramHeadersStart);

    SKIP_8(SectionHeadersStart, addrSectionHeadersStart);

    elf_head_start_params (Back);

    //=======================================================================

    PASTE_8(ProgramHeadersStart, addrProgramHeadersStart);

    elf_head_program_header_params (Back, FileVirtualAddress);

    SKIP_8(SegmentSize, addrSegmentSize);

    SKIP_8(SegmentFileSize, addrSegmentFileSize);

    set_one_byte(Back, 0x00);
    set_one_byte(Back, 0x00);
    set_one_byte(Back, 0x20); //Alignment
    set_one_byte(Back, 0x00);
    set_one_byte(Back, 0x00);
    set_one_byte(Back, 0x00);
    set_one_byte(Back, 0x00);
    set_one_byte(Back, 0x00);

    //=======================================================================

    PASTE_8(SectionHeadersStart, addrSectionHeadersStart);

    //=======================================================================
    //null

    for (int cnt_null_header = 0; cnt_null_header < 64; cnt_null_header++)
    {
        set_one_byte(Back, 0x00);
    }

    //=======================================================================
    //text

    SKIP_4(TextNameOffset, addrTextNameOffset);

    set_one_byte(Back, 0x01); //Section type (bits = 1)
    FILL_4;     //nothing

    set_one_byte(Back, 0x06); //Section flags (r w x )
    FILL_8;     //nothing

    size_t addrAnotherEntryVA = Back->cur_addr;
    Back->cur_addr += 8; //skip

    SKIP_8(TextOffset, addrTextOffset);

    SKIP_8(TextSize, addrTextSize);

    set_one_byte(Back, 0x00); //Section index
    FILL_4;

    set_one_byte(Back, 0x00); //Dop info
    FILL_4;

    set_one_byte(Back, 0x10); //Alignment
    FILL_8;

    set_one_byte(Back, 0x00); //Dop sizes
    FILL_8;

    //=======================================================================
    //strtable

    SKIP_4(TableNameOffset, addrTableNameOffset);

    set_one_byte(Back, 0x03); //Section type (strtable = 1)
    FILL_4;     //nothing

    set_one_byte(Back, 0x00);
    FILL_8;

    SKIP_8(TableLoadAddress, addrTableLoadAddress);

    SKIP_8(TableAddress, addrTableAddress);

    SKIP_8(TableSize, addrTableSize);

    set_one_byte(Back, 0x00); //Section index
    FILL_4;

    set_one_byte(Back, 0x00); //Dop info
    FILL_4;

    set_one_byte(Back, 0x01); //Alignment
    FILL_8;

    set_one_byte(Back, 0x00); //Dop sizes
    FILL_8;

    //=======================================================================
    //entry

    EntryVA = Back->cur_addr + FileVirtualAddress;
    memcpy ((size_t*) &(Back->Array[addrEntryVA]),
    &EntryVA, sizeof (size_t));

    memcpy ((size_t*) &(Back->Array[addrAnotherEntryVA]),
    &EntryVA, sizeof (size_t));

    //=======================================================================
    //.code

    PASTE_8(TextOffset, addrTextOffset);

    //----------------------------------------------------------------------

    elf_generate_code (Back, Root);

    //----------------------------------------------------------------------

    TextSize = Back->cur_addr - TextOffset;
    memcpy ((size_t*) &(Back->Array[addrTextSize]),
    &TextSize, sizeof (size_t));

    //=======================================================================
    //shstrtable

    elf_head_shstrtable (Back, SegmentSize, addrSegmentSize,
                        SegmentFileSize, addrSegmentFileSize,
                        TableAddress, addrTableAddress,
                        TableLoadAddress, addrTableLoadAddress,
                        TextNameOffset, addrTextNameOffset,
                        TableNameOffset, addrTableNameOffset, FileVirtualAddress);

    //=======================================================================

    TableSize = Back->cur_addr - SegmentSize;
    memcpy ((size_t*) &(Back->Array[addrTableSize]),
    &TableSize, sizeof (size_t));

    return;
}

//=============================================================================================================================================================================

const int VAR_SIZE_BYTES = 8;

void elf_generate_code (ElfBack* Back, AstNode* Root)
{
    x86_call_label(Back, MAIN_LBL);

    x86_extra_exit(Back);

    Back->buffer = Back->cur_addr;

    x86_extra_make_input_func (Back);

    x86_extra_make_output_func (Back);

    elf_generate_statement(Back, Root);

    return;
}

void elf_generate_statement (ElfBack* Back, AstNode* CurNode)
{
    if (CurNode->left != NULL)
    {
        elf_generate_op_node (Back, CurNode->left, false);
    }

    if (CurNode->right != NULL)
    {
        elf_generate_statement (Back, CurNode->right);
    }

    return;
}

void elf_generate_function (ElfBack* Back, AstNode* CurNode)
{

    if (wcscoll (CurNode->left->data.var, MAIN_LBL) == 0)
    {
        Back->func_cond = main_f;
    }
    else
    {
        Back->func_cond = any_f;
    }

    x86_extra_paste_call_label(Back, CurNode->left->data.var);

    #ifdef DEBUG_LABELS
    // fprintf (stdout, LABEL "|%s|:\n", CurNode->left->data.var);
    #endif

    Back->Funcs->Table[Back->Funcs->top_index].Name = CurNode->left->data.var;

    Back->table_cond = none;
    elf_create_param_var_table (Back, CurNode->left->right);

    size_t AmountVars = 0;

    SVarTable* Table = NULL;
    if (Back->VarStack->size != 0)
    {

        pop_from_stack (Back->VarStack, &Table);

        AmountVars = elf_find_all_new_vars(Back, CurNode->right);

        Table->amount = AmountVars;

        push_in_stack(Back->VarStack, Table);

        #ifdef DEBUG_VARS
        // printf ("Vars Amount: [%d]\n", AmountVars);
        #endif
    }

    x86_push_reg(Back, rbp);
    x86_mov_reg_reg(Back, rbp, rsp);

    x86_sub_reg_imm(Back, rbp, AmountVars * VAR_SIZE_BYTES);

    x86_mov_reg_reg(Back, rsp, rbp);

    Back->Funcs->top_index++;


    int old_delta_rbp = Back->delta_rbp;
    Back->delta_rbp = (AmountVars) * VAR_SIZE_BYTES;


    elf_generate_statement (Back, CurNode->right);
    ELF_CLEAN_TABLE ();

    Back->delta_rbp = old_delta_rbp;

    x86_add_reg_imm(Back, rbp, AmountVars * VAR_SIZE_BYTES);

    x86_pop_to_reg(Back, rbp);
    x86_mov_reg_reg(Back, rsp, rbp);

    return;
}

void elf_generate_node (ElfBack* Back, AstNode* CurNode)
{
    if (CurNode->category == CategoryOperation)
    {
        elf_generate_op_node (Back, CurNode, true);
    }

    if (CurNode->category == CategoryValue)
    {
        x86_push_imm(Back, CurNode->data.val);
    }

    if (CurNode->category == CategoryLine && CurNode->type == TypeVariable)
    {
        elf_generate_push_var (Back, CurNode);
    }

    return;
}

void elf_generate_op_node (ElfBack* Back, AstNode* CurNode, bool RetValueMarker)
{
    switch (CurNode->type)
    {
        #define DEF_OP(d_type, d_condition, d_tokenize, d_print, d_switch) \
        case d_type: \
            d_switch; \
            break;

        #include "ELF_Operations.h"

        #undef DEF_OP
        default:
            MY_LOUD_ASSERT(false);
    }

    return;
}

void elf_generate_announce (ElfBack* Back, AstNode* CurNode)
{
    elf_generate_expression (Back, CurNode->right);

    elf_add_to_var_table (Back, CurNode->left, false);

    elf_generate_pop_var (Back, CurNode->left);

    return;
}

void elf_generate_equation (ElfBack* Back, AstNode* CurNode)
{
    elf_generate_expression (Back, CurNode->right);

    elf_generate_pop_var (Back, CurNode->left);

    return;
}

void elf_generate_input (ElfBack* Back, AstNode* CurNode)
{
    AstNode* Node = CurNode->left;

    do
    {
        elf_set_rax_var_address (Back, Node->left);

        x86_call_label(Back, INP_LBL);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void elf_generate_output (ElfBack* Back, AstNode* CurNode)
{
    AstNode* Node = CurNode->left;

    do
    {
        elf_set_rax_var_value (Back, Node->left);

        x86_call_label(Back, OUT_LBL);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void elf_generate_if (ElfBack* Back, AstNode* CurNode)
{
    elf_generate_expression (Back, CurNode->left);

    x86_push_imm(Back, MY_FALSE);

    wchar_t* Label_true_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    prepare_name_label_to_jump (Label_true_name, Back->cur_addr);

    x86_cmp_stack(Back);
    x86_jump_label(Back, Label_true_name, x86_je);


    CurNode = CurNode->right;

    Back->table_cond = none;
    elf_generate_statement (Back, CurNode->left);
    ELF_CLEAN_TABLE ();

    wchar_t* Label_false_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    prepare_name_label_to_jump (Label_false_name, Back->cur_addr);

    x86_jump_label(Back, Label_false_name, x86_jmp);

    x86_extra_paste_jump_label(Back, Label_true_name);

    CurNode = CurNode->right;

    if (CurNode != NULL)
    {
        if (CurNode->category == CategoryOperation && CurNode->type == TypeIf)
        {
            elf_generate_if (Back, CurNode);
        }
        else
        {
            Back->table_cond = none;
            elf_generate_statement (Back, CurNode);
            ELF_CLEAN_TABLE ();
        }
    }

    x86_extra_paste_jump_label(Back, Label_false_name);

    free (Label_true_name);
    free (Label_false_name);

    return;
}

void elf_generate_while (ElfBack* Back, AstNode* CurNode)
{
    wchar_t* Label_body_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    prepare_name_label_to_jump (Label_body_name, Back->cur_addr);

    x86_extra_paste_jump_label(Back, Label_body_name);
    // int Label_1 = label_cnt;
    // fprintf (file,  LABEL "%d:\n", Label_1);
    // label_cnt++;

    // x86_nop();
    elf_generate_expression (Back, CurNode->left);
    // x86_nop();

    x86_push_imm(Back, MY_FALSE);

    wchar_t* Label_end_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    prepare_name_label_to_jump (Label_end_name, Back->cur_addr);

    x86_cmp_stack(Back);
    x86_jump_label(Back, Label_end_name, x86_je);

    // PUT (je);
    // int Label_2 = label_cnt;
    // fprintf (file, " " LABEL "%d\n", Label_2);
    // label_cnt++;

    // x86_nop();
    Back->table_cond = none;
    elf_generate_statement (Back, CurNode->right);
    ELF_CLEAN_TABLE ();
    // x86_nop();

    x86_jump_label(Back, Label_body_name, x86_jmp);

    x86_extra_paste_jump_label(Back, Label_end_name);

    free (Label_body_name);
    free (Label_end_name);

    return;
}

void elf_generate_call (ElfBack* Back, AstNode* CurNode, bool RetValueMarker)
{
    // x86_push_r(eTOP_REG);

    elf_push_parameters (Back, CurNode->right);

    // x86_push_r(eTOP_REG);

    // x86_pop_r(ELF_SHIFT_REG);

    x86_call_label(Back, CurNode->left->data.var);

    elf_delete_function_parameters (Back, CurNode->right);

    // PUT (call);
    // fprintf (file, " " LABEL "%ls\n", CurNode->left->data.var);

    // x86_pop_r(eTOP_REG);

    if (RetValueMarker == true)
    {
        // printf (KRED "WAAGH!\n" KNRM);
        x86_push_reg(Back, ELF_FUNC_RET_REG);
    }
    else
    {
        // printf (KGRN "no\n" KNRM);
    }

    return;
}

void elf_generate_return (ElfBack* Back, AstNode* CurNode)
{
    elf_generate_expression (Back, CurNode->left);

    x86_pop_to_reg(Back, ELF_FUNC_RET_REG);

    size_t AmountVars = 0;

    SVarTable* Table = NULL;

    if (Back->VarStack->size != 0)
    {
        peek_from_stack (Back->VarStack, &Table);
        AmountVars = Table->amount;
    }
    x86_add_reg_imm(Back, rbp, AmountVars * VAR_SIZE_BYTES);

    x86_mov_reg_reg(Back, rsp, rbp);

    x86_pop_to_reg(Back, rbp);

    x86_ret(Back);

    return;
}

void elf_generate_expression (ElfBack* Back, AstNode* CurNode)
{
    elf_generate_postorder (Back, CurNode, true);

    return;
}

void elf_generate_postorder (ElfBack* Back, AstNode* CurNode, bool RetValueMarker)
{
    if (!(CurNode->category == CategoryOperation && CurNode->type == TypeLinkerCall) && CurNode->left != NULL)
    {
        elf_generate_postorder (Back, CurNode->left, RetValueMarker);
    }

    if (!(CurNode->category == CategoryOperation && CurNode->type == TypeLinkerCall) && CurNode->right != NULL)
    {
        elf_generate_postorder (Back, CurNode->right, RetValueMarker);
        // x86_push_r(ELF_FUNC_RET_REG);
    }

    elf_generate_node (Back, CurNode);

    return;
}

//=============================================================================================================================================================================

void elf_set_rax_var_value (ElfBack* Back, AstNode* CurNode)
{
    int Index = elf_find_var (Back, CurNode);

    // printf("value param = ");
    if (Index < 0)
    {
        // printf("Index: %d Delta_rbp: %d Final: %d\n", Index, delta_rbp, delta_rbp - Index * VAR_SIZE);
        x86_mov_to_reg_from_addr_in_reg_with_imm(Back, rax, rbp, Back->delta_rbp - Index * VAR_SIZE_BYTES);

        return;
    }

    // printf("Index: %d\n", Index);
    x86_mov_to_reg_from_addr_in_reg_with_imm(Back, rax, rbp, Index);

    return;
}

void elf_set_rax_var_address (ElfBack* Back, AstNode* CurNode)
{
    int Index = elf_find_var (Back, CurNode);
    // printf("addr  param = ");

    x86_mov_reg_reg(Back, rax, rbp);

    if (Index < 0)
    {
        // printf("Index: %d Delta_rbp: %d Final: %d\n", Index, delta_rbp, delta_rbp - Index * VAR_SIZE);
        x86_add_reg_imm(Back, rax, Back->delta_rbp - Index * VAR_SIZE_BYTES);

        return;
    }

    // printf("Index: %d\n", Index);
    x86_add_reg_imm(Back, rax, Index);

    return;
}

void elf_generate_pop_var (ElfBack* Back, AstNode* CurNode)
{
    elf_set_rax_var_address(Back, CurNode);

    x86_pop_to_reg(Back, FIRST_REG);

    x86_mov_to_addr_in_reg_from_reg(Back, rax, FIRST_REG);

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

void elf_generate_push_var (ElfBack* Back, AstNode* CurNode)
{
    elf_set_rax_var_value(Back, CurNode);

    x86_push_reg(Back, rax);

//     int Index = elf_find_var (CurNode);
//
//     x86_mov_r_r(eCOUNT_REG, ELF_SHIFT_REG);
//     x86_add_i(eCOUNT_REG, Index);
//
//     x86_push_IrI(eCOUNT_REG);

    return;
}

//=============================================================================================================================================================================

void elf_push_parameters (ElfBack* Back, AstNode* CurNode)
{
    if (CurNode->right != NULL)
    {
        elf_push_parameters (Back, CurNode->right);
    }

    if (CurNode != NULL && CurNode->left != NULL)
    {
        elf_generate_expression (Back, CurNode->left);
    }

    return;
}

void elf_delete_function_parameters (ElfBack* Back, AstNode* CurNode)
{
    if (CurNode->right != NULL)
    {
        elf_delete_function_parameters (Back, CurNode->right);
    }

    if (CurNode != NULL && CurNode->left != NULL)
    {
        x86_add_reg_imm(Back, rsp, VAR_SIZE_BYTES);
    }

    return;
}

void elf_standard_if_jump (ElfBack* Back, int JumpMode)
{
    const int STD_JUMP_FIRST_SHIFT  = 6;
    const int STD_JUMP_SECOND_SHIFT = 4;

    if (JumpMode != x86_jmp)
    {
        x86_cmp_stack (Back);
    }

    x86_jump_any(Back, STD_JUMP_FIRST_SHIFT, JumpMode);

    // fprintf (file, " " LABEL "%d\n", label_cnt);
    // label_cnt++;

    x86_push_imm(Back, MY_FALSE);

    // PUT (push);
    // fprintf (file, " %d\n", MY_FALSE);

    x86_jump_any(Back, STD_JUMP_SECOND_SHIFT, x86_jmp);

    // PUT (jump);
    // fprintf (file, " " LABEL "%d\n", label_cnt);

    // fprintf (file, LABEL "%d:\n", label_cnt - 1);

    x86_push_imm(Back, MY_TRUE);

    // PUT (push);
    // fprintf (file, " %d\n", MY_TRUE);

//     fprintf (file, LABEL "%d:\n", label_cnt);
//
//     label_cnt++;

    return;
}

void elf_write_command (ElfBack* Back, ECommandNums eCommand, FILE* File)
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

void  elf_add_to_var_table (ElfBack* Back, AstNode* CurNode, bool ParamMarker)
{
    if (Back->table_cond == EVarTableConditions::none)
    {
        elf_create_new_var_table (Back, ParamMarker);
    }

    MY_LOUD_ASSERT (CurNode->category == CategoryLine && CurNode->type == TypeVariable);

    SVarTable* Table = NULL;
    peek_from_stack (Back->VarStack, &Table);

    Table->Arr[Table->cur_size].name  = CurNode->data.var; //copy address from node

    if (ParamMarker == false)
    {
        #ifdef VAR_OVERSEER
        printf (KYLW "Amount: %d CurSize: %d Param: %d Final: %d => " KNRM, Table->amount, Table->cur_size, Table->amount_param, Table->amount - (Table->cur_size - Table->amount_param) - 1);
        #endif

        Table->Arr[Table->cur_size].index = (Table->amount - (Table->cur_size - Table->amount_param) - 1) * VAR_SIZE_BYTES;

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

void elf_create_new_var_table (ElfBack* Back, bool ParamMarker)
{
    PRINT_DEBUG_INFO;

    Back->delta_rbp = 0;

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

    push_in_stack (Back->VarStack, NewTable);

    Back->table_cond = exist;

    return;
}

size_t elf_find_all_new_vars (ElfBack* Back, AstNode* CurNode)
{
    return elf_find_new_var(Back, CurNode);
}

size_t elf_find_new_var (ElfBack* Back, AstNode* CurNode)
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

    return OneMoreVar + elf_find_new_var(Back, CurNode->right);
}


void elf_create_param_var_table (ElfBack* Back, AstNode* CurNode)
{
    PRINT_DEBUG_INFO;

    bool TableCreationMark = false;

    do
    {
        if (CurNode->left != NULL)
        {
            elf_add_to_var_table (Back, CurNode->left->left, true);
            Back->Funcs->Table[Back->Funcs->top_index].parameters++;
            TableCreationMark = true;
        }

        CurNode = CurNode->right;
    } while (CurNode != NULL);

    if (TableCreationMark == false)
    {
        // printf (KRED "forced\n" KNRM);
        elf_create_new_var_table(Back, true);
    }

    return;
}

void elf_delete_var_table (ElfBack* Back)
{
    PRINT_DEBUG_INFO;

    SVarTable* Table = NULL;

    pop_from_stack (Back->VarStack, &Table);

    free (Table->Arr);

    free (Table);

    return;
}

AstNode* elf_find_parent_statement (AstNode* CurNode)
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

int elf_find_var (ElfBack* Back, AstNode* CurNode)
{
    int RetIndex = WrongValue;
    bool ParamMarker = false;

    MY_LOUD_ASSERT (Back->VarStack->size != 0);
    MY_LOUD_ASSERT (CurNode->category == CategoryLine && CurNode->type == TypeVariable);

    SVarTable* Table = NULL;

    int depth = 1;
    do
    {
        Table = Back->VarStack->data[Back->VarStack->size - depth];
        //printf ("=%p=%d/%d=\n", Table, depth, VarStack->size);
        depth++;
    } while ((elf_find_in_table (Back, CurNode->data.var, Table, &RetIndex, &ParamMarker) == false) && (depth <= Back->VarStack->size));

    if (RetIndex == WrongValue)
    {
        printf ("==ERROR==\n""No '%ls' found!\n", CurNode->data.var);
        MY_LOUD_ASSERT (0);
    }

    return RetIndex;
}

//=============================================================================================================================================================================

bool elf_find_in_table (ElfBack* Back, CharT* varName, SVarTable* Table, int* RetIndex, bool* ParamMarker)
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

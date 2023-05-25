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

ElfBack::ElfBack(FILE* ExFile, SNode* Root)
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

void make_elf_file (SNode* Root, FILE* ExFile)
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

void ElfBack::generate_elf_array (SNode* Root) // TODO too large function
{
    SET (0x7f); //MAGIC SIGNATURE

    SET ('E'); // TODO Maybe move to create ELF header func
    SET ('L');
    SET ('F');

    SET (0x02); //Class (64-bit = 2)

    SET (0x01); //Endian (little endian = 1)

    SET (0x01); //Elf Version (only correct = 1)

    SET (0x00); //OS ABI (System V (UNIX) = 0)

    SET (0x00); //ABI version (none = 0)

    FILL_8;     //7 padding bytes

    SET (0x02); //Type (executable = 2)
    FILL_2;     //nothing

    SET (0x3e); //Machine (x86_64 = 3e)
    FILL_2;     //nothing

    SET (0x01); //Version (only correct = 1)
    FILL_4;     //nothing


    size_t FileVirtualAddress = 4096 * 40; //40 is random number, 4096 is for alignment

    SKIP_8(EntryVA, addr_EntryVA); // TODO naming!

    SKIP_8(ProgramHeadersStart, addr_ProgramHeadersStart);

    SKIP_8(SectionHeadersStart, addr_SectionHeadersStart);

    SET (0x00); //Flags (no flags = 0)
    FILL_4;     //nothing

    unsigned short int  HeaderSize = 64; //std
    SET_2 (HeaderSize);

    unsigned short int  ProgramHeaderSize = 56; //std
    SET_2(ProgramHeaderSize);

    unsigned short int  ProgramHeadersCnt = 1;
    SET_2(ProgramHeadersCnt);

    unsigned short int  SectionHeadersSize = 64; //std
    SET_2(SectionHeadersSize);

    unsigned short int  SectionHeadersCnt = 3; // maybe not
    SET_2(SectionHeadersCnt);

    unsigned short int  ShstrTabIndex  = 2; // maybe not
    SET_2(ShstrTabIndex);

    //=======================================================================

    PASTE_8(ProgramHeadersStart, addr_ProgramHeadersStart);

    SET (0x01); //Segment type (load = 0)
    FILL_4;     //nothing

    SET (0x05); //Segment flag (r w x = 0 1 2)
    FILL_4;     //nothing

    SET (0x00); //Segment offset (0)
    FILL_8;     //nothing

    memcpy ((size_t*) &(Array[cur_addr]),
            &FileVirtualAddress, sizeof (size_t));  //Virtual address
    cur_addr += 8;

    memcpy ((size_t*) &(Array[cur_addr]),
            &FileVirtualAddress, sizeof (size_t));  //Physical address (same as virtual)
    cur_addr += 8;

    SKIP_8(SegmentSize, addr_SegmentSize);

    SKIP_8(SegmentFileSize, addr_SegmentFileSize);

    SET (0x00);
    SET (0x00);
    SET (0x20); //Alignment
    SET (0x00);
    SET (0x00);
    SET (0x00);
    SET (0x00);
    SET (0x00);

    //=======================================================================

    PASTE_8(SectionHeadersStart, addr_SectionHeadersStart);

    //=======================================================================
    //null

    for (int cnt_null_header = 0; cnt_null_header < 64; cnt_null_header++)
    {
        SET (0x00);
    }

    //=======================================================================
    //text

    SKIP_4(TextNameOffset, addr_TextNameOffset);

    SET (0x01); //Section type (bits = 1)
    FILL_4;     //nothing

    SET (0x06); //Section flags (r w x )
    FILL_8;     //nothing

    size_t addr_another_EntryVA = cur_addr;
    cur_addr += 8; //skip

    SKIP_8(TextOffset, addr_TextOffset);

    SKIP_8(TextSize, addr_TextSize);

    SET (0x00); //Section index
    FILL_4;

    SET (0x00); //Dop info
    FILL_4;

    SET (0x10); //Alignment
    FILL_8;

    SET (0x00); //Dop sizes
    FILL_8;

    //=======================================================================
    //strtable

    SKIP_4(TableNameOffset, addr_TableNameOffset);

    SET (0x03); //Section type (strtable = 1)
    FILL_4;     //nothing

    SET (0x00);
    FILL_8;

    SKIP_8(TableLoadAddress, addr_TableLoadAddress);

    SKIP_8(TableAddress, addr_TableAddress);

    SKIP_8(TableSize, addr_TableSize);

    SET (0x00); //Section index
    FILL_4;

    SET (0x00); //Dop info
    FILL_4;

    SET (0x01); //Alignment
    FILL_8;

    SET (0x00); //Dop sizes
    FILL_8;

    //=======================================================================
    //entry

    EntryVA = cur_addr + FileVirtualAddress;
    memcpy ((size_t*) &(Array[addr_EntryVA]),
    &EntryVA, sizeof (size_t));

    memcpy ((size_t*) &(Array[addr_another_EntryVA]),
    &EntryVA, sizeof (size_t)); //TODO maybe remove

    //=======================================================================
    //.code

    PASTE_8(TextOffset, addr_TextOffset);

    //----------------------------------------------------------------------

    elf_generate_code (Root);

    //----------------------------------------------------------------------

    TextSize = cur_addr - TextOffset;
    memcpy ((size_t*) &(Array[addr_TextSize]),
    &TextSize, sizeof (size_t));

    //=======================================================================
    //shstrtable

    PASTE_8(SegmentSize, addr_SegmentSize);

    PASTE_8(SegmentFileSize, addr_SegmentFileSize);

    PASTE_8(TableAddress, addr_TableAddress);

    TableLoadAddress = TableAddress + FileVirtualAddress;
    memcpy ((size_t*) &(Array[addr_TableLoadAddress]),
    &TableLoadAddress, sizeof (size_t));


    SET (0x00); //begin section header string table

    TextNameOffset = cur_addr - SegmentSize;
    memcpy ((unsigned int*) &(Array[addr_TextNameOffset]),
    &TextNameOffset, sizeof (unsigned int));

    SET ('.');
    SET ('t');
    SET ('e');
    SET ('x');
    SET ('t');
    SET (0x00); //end

    TableNameOffset = cur_addr - SegmentSize;
    memcpy ((unsigned int*) &(Array[addr_TableNameOffset]),
    &TableNameOffset, sizeof (unsigned int));

    SET ('.');
    SET ('s');
    SET ('h');
    SET ('s');
    SET ('t');
    SET ('r');
    SET ('t');
    SET ('a');
    SET ('b');
    SET (0x00); //end

    TableSize = cur_addr - SegmentSize;
    memcpy ((size_t*) &(Array[addr_TableSize]),
    &TableSize, sizeof (size_t));

    return;
}

//=============================================================================================================================================================================
#define VAR_SIZE 8

#define BUFFER_SIZE 20

void ElfBack::elf_generate_code (SNode* Root)
{
    x86_call_label(MAIN_LBL);

    x86_macro_exit();

    buffer = cur_addr;

    for (size_t cnt = 0; cnt < BUFFER_SIZE; ++cnt)
    {
        SET(0x00);
    }

    x86_macro_make_input_func ();

    x86_macro_make_output_func ();

    elf_generate_statement(Root);

    return;
}

void ElfBack::elf_generate_statement (SNode* CurNode)
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

void ElfBack::elf_generate_function (SNode* CurNode)
{

    if (wcscoll (CurNode->left->data.var, MAIN_LBL) == 0)
    {
        func_cond = main_f;
    }
    else
    {
        func_cond = any_f;
    }

    x86_macro_paste_call_label(CurNode->left->data.var);
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

void ElfBack::elf_generate_node (SNode* CurNode)
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

void ElfBack::elf_generate_op_node (SNode* CurNode, bool RetValueMarker)
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

void ElfBack::elf_generate_announce (SNode* CurNode)
{
    elf_generate_expression (CurNode->right);

    elf_add_to_var_table (CurNode->left, false);

    elf_generate_pop_var (CurNode->left);

    // elf_incr_top_reg ();

    return;
}

void ElfBack::elf_generate_equation (SNode* CurNode)
{
    elf_generate_expression (CurNode->right);

    elf_generate_pop_var (CurNode->left);

    return;
}

void ElfBack::elf_generate_input (SNode* CurNode)
{
    SNode* Node = CurNode->left;

    do
    {
        elf_set_rax_var_address (Node->left);

        x86_call_label(INP_LBL);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void ElfBack::elf_generate_output (SNode* CurNode)
{
    SNode* Node = CurNode->left;

    do
    {
        elf_set_rax_var_value (Node->left);

        x86_call_label(OUT_LBL);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void ElfBack::elf_generate_if (SNode* CurNode)
{
    elf_generate_expression (CurNode->left);

    x86_push_imm(MY_FALSE);

    wchar_t* Label_true_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    prepare_name_label_to_jump (Label_true_name, cur_addr);

    x86_cmp_stack();
    x86_jump_label(Label_true_name, je_); // TODO rename je_


    CurNode = CurNode->right;

    table_cond = none;
    elf_generate_statement (CurNode->left);
    ELF_CLEAN_TABLE ();

    wchar_t* Label_false_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    prepare_name_label_to_jump (Label_false_name, cur_addr);

    x86_jump_label(Label_false_name, jmp_);

    x86_macro_paste_jump_label(Label_true_name);

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

    x86_macro_paste_jump_label(Label_false_name);

    free (Label_true_name);
    free (Label_false_name);

    return;
}

void ElfBack::elf_generate_while (SNode* CurNode)
{
    wchar_t* Label_body_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    prepare_name_label_to_jump (Label_body_name, cur_addr);

    x86_macro_paste_jump_label(Label_body_name);
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
    x86_jump_label(Label_end_name, je_);

    // PUT (je);
    // int Label_2 = label_cnt;
    // fprintf (file, " " LABEL "%d\n", Label_2);
    // label_cnt++;

    // x86_nop();
    table_cond = none;
    elf_generate_statement (CurNode->right);
    ELF_CLEAN_TABLE ();
    // x86_nop();

    x86_jump_label(Label_body_name, jmp_);

    x86_macro_paste_jump_label(Label_end_name);

    free (Label_body_name);
    free (Label_end_name);

    return;
}

void ElfBack::elf_generate_call (SNode* CurNode, bool RetValueMarker)
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

void ElfBack::elf_generate_return (SNode* CurNode)
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

void ElfBack::elf_generate_expression (SNode* CurNode)
{
    elf_generate_postorder (CurNode, true);

    return;
}

void ElfBack::elf_generate_postorder (SNode* CurNode, bool RetValueMarker)
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

void ElfBack::elf_set_rax_var_value (SNode* CurNode)
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

void ElfBack::elf_set_rax_var_address (SNode* CurNode)
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

void ElfBack::elf_generate_pop_var (SNode* CurNode)
{
    elf_set_rax_var_address(CurNode);

    x86_pop_to_reg(A_REG);

    x86_mov_to_addr_in_reg_from_reg(rax, A_REG);

//     int Index = elf_find_var (CurNode);
//
//     x86_mov_r_r(eCOUNT_REG, ELF_SHIFT_REG);
//     x86_add_i(eCOUNT_REG, Index);
//
//     x86_pop_r(A_REG);
//     x86_mov_IrI_r(eCOUNT_REG, A_REG);

    // PUT (pop);
    // fprintf (file, " [" COUNT_REG "]\n\n"); // this was

    return;
}

void ElfBack::elf_generate_push_var (SNode* CurNode)
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

void ElfBack::elf_push_parameters (SNode* CurNode)
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

void ElfBack::elf_delete_function_parameters (SNode* CurNode)
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

    if (JumpMode != jmp_)
    {
        x86_cmp_stack ();
    }

    x86_jump_any(STD_JUMP_FIRST_SHIFT, JumpMode);

    // fprintf (file, " " LABEL "%d\n", label_cnt);
    // label_cnt++;

    x86_push_imm(MY_FALSE);

    // PUT (push);
    // fprintf (file, " %d\n", MY_FALSE);

    x86_jump_any(STD_JUMP_SECOND_SHIFT, jmp_);

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

void ElfBack:: elf_add_to_var_table (SNode* CurNode, bool ParamMarker)
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

size_t ElfBack::elf_find_all_new_vars (SNode* CurNode)
{
    return elf_find_new_var(CurNode);
}

size_t ElfBack::elf_find_new_var (SNode* CurNode)
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


void ElfBack::elf_create_param_var_table (SNode* CurNode)
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

SNode* ElfBack::elf_find_parent_statement (SNode* CurNode)
{
    SNode* Node = NULL;
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

int ElfBack::elf_find_var (SNode* CurNode)
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

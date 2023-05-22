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

SElfBack::SElfBack(FILE* ExFile, SNode* Root)
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

SElfBack::~SElfBack()
{
    free_tables (VarStack);

    free (Funcs->Table);
    free (Funcs);

    free (Array);
}

//=============================================================================================================================================================================

void make_elf_file (SNode* Root, FILE* ExFile)
{
    MLA (ExFile != NULL);

    SElfBack Back {ExFile, Root};

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

void SElfBack::generate_elf_array (SNode* Root)
{
    SET (0x7f); //MAGIC SIGNATURE

    SET ('E');
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

    SKIP_8(EntryVA, addr_EntryVA);

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

    unsigned short int  SectionHeadersCnt = 3; //TODO maybe not
    SET_2(SectionHeadersCnt);

    unsigned short int  ShstrTabIndex  = 2; //TODO maybe not
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

void SElfBack::elf_generate_code (SNode* Root)
{
    x86_mov_r_i(eSHIFT_REG, 0);

    x86_mov_r_i(eTOP_REG, cur_addr - start_cnt); // TODO maybe not

    x86_call_label(MAIN_LBL);

    x86___End();

    x86___make_inp_func ();

    x86___make_out_func ();

    elf_generate_statement(Root);

    return;
}

void SElfBack::elf_generate_statement (SNode* CurNode)
{
    if (CurNode->left != NULL)
    {
        elf_generate_op_node (CurNode->left);
    }

    if (CurNode->right != NULL)
    {
        elf_generate_statement (CurNode->right);
    }

    return;
}

void SElfBack::elf_generate_function (SNode* CurNode)
{

    if (wcscoll (CurNode->left->data.var, MAIN_LBL) == 0)
    {
        func_cond = main_f;
        x86___paste_call_label(MAIN_LBL);
    }
    else
    {
        func_cond = any_f;
        x86___paste_call_label(CurNode->left->data.var);
    }
    // x86___paste_call_label(Name);
    // fprintf (stdout, LABEL "|%s|:\n", CurNode->left->data.var);

    Funcs->Table[Funcs->top_index].Name = CurNode->left->data.var;

    table_cond = none;
    elf_create_param_var_table (CurNode->left->right);

    SVarTable* Table = NULL;
    peek_from_stack (VarStack, &Table);
    size_t AmountVars = Table->amount;

    // elf_pop_parameters ();

    x86_push_r(rbp);
    x86_mov_r_r(rbp, rsp);

    x86_sub_i(rbp, AmountVars * VAR_SIZE);

    Funcs->top_index++;

    elf_generate_statement (CurNode->right);
    ELF_CLEAN_TABLE;

    return;
}

void SElfBack::elf_generate_node (SNode* CurNode)
{
    if (CurNode->category == COperation)
    {
        elf_generate_op_node (CurNode);
    }

    if (CurNode->category == CValue)
    {
        x86_push_i(CurNode->data.val);
    }

    if (CurNode->category == CLine && CurNode->type == TVariable)
    {
        elf_generate_push_var (CurNode);
    }

    return;
}

void SElfBack::elf_generate_op_node (SNode* CurNode)
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

void SElfBack::elf_generate_announce (SNode* CurNode)
{
    elf_generate_expression (CurNode->right);

    elf_add_to_var_table (CurNode->left, false);

    elf_generate_pop_var (CurNode->left);

    // elf_incr_top_reg ();

    return;
}

void SElfBack::elf_generate_equation (SNode* CurNode)
{
    elf_generate_expression (CurNode->right);

    elf_generate_pop_var (CurNode->left);

    return;
}

void SElfBack::elf_generate_input (SNode* CurNode)
{
    SNode* Node = CurNode->left;

    do
    {
        elf_rax_var_address (Node->left);

        x86_call_label(INP_LBL);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void SElfBack::elf_generate_output (SNode* CurNode)
{
    SNode* Node = CurNode->left;

    do
    {
        elf_rax_var_value (Node->left);

        elf_generate_push_var (Node->left);

        x86_call_label(INP_LBL);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void SElfBack::elf_generate_if (SNode* CurNode)
{
    elf_generate_postorder (CurNode->left);

    x86_push_i(FALSE);

    wchar_t* Label_1_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    make_label_to_jump (Label_1_name, cur_addr);

    x86_cmp_stack();
    x86_jump_label(Label_1_name, je_);

    // PUT (je);
    // fprintf (file, " " LABEL "%d\n", label_cnt);
    // int Label_1 = label_cnt;
    // label_cnt++;

    CurNode = CurNode->right;

    table_cond = none;
    elf_generate_statement (CurNode->left);
    ELF_CLEAN_TABLE;

    wchar_t* Label_2_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    make_label_to_jump (Label_2_name, cur_addr);

    x86_jump_label(Label_2_name, jmp_);
    // PUT (jump);
    // fprintf (file, " " LABEL "%d\n", label_cnt);
    // int Label_2 = label_cnt;
    // label_cnt++;

    x86___paste_jump_label(Label_1_name);
    // fprintf (file,  LABEL "%d:\n", Label_1);

    CurNode = CurNode->right;

    x86_nop();
    x86_nop();
    if (CurNode != NULL)
    {
        if (CurNode->category == COperation && CurNode->type == TIf)
        {
            elf_generate_if (CurNode);
        }
        else
        {
            table_cond = none;
            elf_generate_statement (CurNode);
            ELF_CLEAN_TABLE;
        }
    }
    x86_nop();
    x86_nop();

    x86___paste_jump_label(Label_2_name);
    // fprintf (file,  LABEL "%d:\n", Label_2);

    free (Label_1_name);
    free (Label_2_name);

    return;
}

void SElfBack::elf_generate_while (SNode* CurNode)
{
    wchar_t* Label_1_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    make_label_to_jump (Label_1_name, cur_addr);

    x86___paste_jump_label(Label_1_name);
    // int Label_1 = label_cnt;
    // fprintf (file,  LABEL "%d:\n", Label_1);
    // label_cnt++;

    x86_nop();
    elf_generate_postorder (CurNode->left);
    x86_nop();

    x86_push_i(FALSE);

    wchar_t* Label_2_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    make_label_to_jump (Label_2_name, cur_addr);

    x86_cmp_stack();
    x86_jump_label(Label_2_name, je_);

    // PUT (je);
    // int Label_2 = label_cnt;
    // fprintf (file, " " LABEL "%d\n", Label_2);
    // label_cnt++;

    x86_nop();
    table_cond = none;
    elf_generate_statement (CurNode->right);
    ELF_CLEAN_TABLE;
    x86_nop();

    x86_jump_label(Label_1_name, jmp_);

    x86___paste_jump_label(Label_2_name);

    free (Label_1_name);
    free (Label_2_name);

    return;
}

void SElfBack::elf_generate_call (SNode* CurNode)
{
    x86_push_r(eTOP_REG);

    elf_push_parameters (CurNode->right);

    x86_push_r(eTOP_REG);

    x86_pop_r(eSHIFT_REG);

    x86_call_label(CurNode->left->data.var);

    // PUT (call);
    // fprintf (file, " " LABEL "%ls\n", CurNode->left->data.var);

    x86_pop_r(eTOP_REG);

    x86_push_r(eFUNC_REG);

    return;
}

void SElfBack::elf_generate_return (SNode* CurNode)
{
    elf_generate_expression (CurNode->left);

//     if (func_cond == main_f)
//     {
//         x86_pop_r(eFUNC_REG);
//
//         x86_ret();
//     }
//     else
//     {
        x86_pop_r(eFUNC_REG);

        x86_ret();
    // }

    return;
}

void SElfBack::elf_generate_expression (SNode* CurNode)
{
    elf_generate_postorder (CurNode);

    return;
}

void SElfBack::elf_generate_postorder (SNode* CurNode)
{
    if (!(CurNode->category == COperation && CurNode->type == T_Call) && CurNode->left != NULL)
    {
        elf_generate_postorder (CurNode->left);
    }

    if (!(CurNode->category == COperation && CurNode->type == T_Call) && CurNode->right != NULL)
    {
        elf_generate_postorder (CurNode->right);
    }

    elf_generate_node (CurNode);

    return;
}

//=============================================================================================================================================================================

void SElfBack::elf_rax_var_value (SNode* CurNode)
{
    int Index = elf_find_var (CurNode);

    if (Index < 0)
    {
        x86_mov_r_Ir_iI(rax, rbp, current_rbp_shift + Index * VAR_SIZE);

        return;
    }

    x86_mov_r_Ir_iI(rax, rbp, - Index);

    return;
}

void SElfBack::elf_rax_var_address (SNode* CurNode)
{
    int Index = elf_find_var (CurNode);

    x86_mov_r_r(rax, rbp);

    if (Index < 0)
    {
        x86_add_i(rax, current_rbp_shift + Index * VAR_SIZE);

        return;
    }

    x86_sub_i(rax, Index);

    return;
}

void SElfBack::elf_generate_pop_var (SNode* CurNode)
{
    elf_rax_var_address(CurNode);

    x86_pop_r(A_REG);

    x86_mov_IrI_r(rax, A_REG);

//     int Index = elf_find_var (CurNode);
//
//     x86_mov_r_r(eCOUNT_REG, eSHIFT_REG);
//     x86_add_i(eCOUNT_REG, Index);
//
//     x86_pop_r(A_REG);
//     x86_mov_IrI_r(eCOUNT_REG, A_REG);

    // PUT (pop);
    // fprintf (file, " [" COUNT_REG "]\n\n"); // this was

    return;
}

void SElfBack::elf_generate_push_var (SNode* CurNode)
{
    elf_rax_var_value(CurNode);

    x86_push_r(rax);

//     int Index = elf_find_var (CurNode);
//
//     x86_mov_r_r(eCOUNT_REG, eSHIFT_REG);
//     x86_add_i(eCOUNT_REG, Index);
//
//     x86_push_IrI(eCOUNT_REG);

    return;
}

//=============================================================================================================================================================================

void SElfBack::elf_push_parameters (SNode* CurNode)
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

// void SElfBack::elf_pop_parameters ()
// {
//     // x86_push_r(eSHIFT_REG);
//
//     // x86_pop_r(eCOUNT_REG);
//
//     // x86_mov_r_r(rax, rbp);
//
//     size_t AmountParams = Funcs->Table[Funcs->top_index].parameters;
//
//     for (int i = 0; i < AmountParams; i++)
//     {
//         x86_pop_r(rax);
//
//         x86_mov_r_Ir_iI(rsp, rax, - (AmountParams - ((i + 1) * VAR_SIZE)));
//
//         x86_inc(eCOUNT_REG);
//
//         x86_pop_r(A_REG);
//         x86_mov_IrI_r(eCOUNT_REG, A_REG);
//
//         // PUT (pop);
//         // fprintf (file, " [" COUNT_REG "]\n\n");  // this was
//     }
//
//     return;
// }

void SElfBack::elf_incr_top_reg ()
{
    x86_inc(eTOP_REG);

    return;
}

#define STD_JUMP_FIRST_SHIFT    6   //DO NOT change
#define STD_JUMP_SECOND_SHIFT   4   //DO NOT change

void SElfBack::elf_standard_if_jump (int JumpMode)
{
    if (JumpMode != jmp_)
    {
        x86_cmp_stack ();
    }

    x86_jump_any(STD_JUMP_FIRST_SHIFT, JumpMode);

    // fprintf (file, " " LABEL "%d\n", label_cnt);
    // label_cnt++;

    x86_push_i(FALSE);

    // PUT (push);
    // fprintf (file, " %d\n", FALSE);

    x86_jump_any(STD_JUMP_SECOND_SHIFT, jmp_);

    // PUT (jump);
    // fprintf (file, " " LABEL "%d\n", label_cnt);

    // fprintf (file, LABEL "%d:\n", label_cnt - 1);

    x86_push_i(TRUE);

    // PUT (push);
    // fprintf (file, " %d\n", TRUE);

//     fprintf (file, LABEL "%d:\n", label_cnt);
//
//     label_cnt++;

    return;
}

void SElfBack::elf_write_command (ECommandNums eCommand, FILE* File)
{
    #define DEF_CMD(def_line, def_enum, ...) \
    else if (eCommand == def_enum) \
    { \
        fprintf (File, TAB  def_line); \
        return; \
    }

    if (0) {}

    #include "commands.h"

    else
    {
        MLA (0);
    }

    #undef DEF_CMD
}

//=============================================================================================================================================================================

void SElfBack:: elf_add_to_var_table (SNode* CurNode, bool ParamMarker)
{
    if (table_cond == none)
    {
        elf_create_new_var_table (CurNode, ParamMarker);
    }

    MLA (CurNode->category == CLine && CurNode->type == TVariable);

    SVarTable* Table = NULL;
    peek_from_stack (VarStack, &Table);

    Table->Arr[Table->cur_size].name  = CurNode->data.var; //copy address from node

    if (ParamMarker == false)
    {
        Table->Arr[Table->cur_size].index = (Table->amount - Table->cur_size - 1) * VAR_SIZE;
    }
    else
    {
        Table->Arr[Table->cur_size].index = - (Table->cur_size + 1); // another way to mark params of function
    }

    // RAM_top_index++;
    Table->cur_size++;

    return;
}

void SElfBack::elf_create_new_var_table (SNode* CurNode, bool ParamMarker)
{
    ME;

    delta_rbp = 0;

    SVarTable* NewTable = (SVarTable*)  calloc (1,                  sizeof (SVarTable));
    NewTable->Arr       = (SVarAccord*) calloc (VAR_TABLE_CAPACITY, sizeof (SVarAccord));
    NewTable->cur_size = 0;

    NewTable->param_marker = ParamMarker;

    if (ParamMarker == false)
    {
        NewTable->amount = elf_find_new_vars (CurNode);
        printf("amount %d\n", NewTable->amount);
    }

    push_in_stack (VarStack, NewTable);

    table_cond = exist;

    return;
}

size_t SElfBack::elf_find_new_vars (SNode* CurNode)
{
    return elf_find_new_var(CurNode);
}

size_t SElfBack::elf_find_new_var (SNode* CurNode)
{
    size_t OneMoreVar = 0;

    if (CurNode->left->type == T_Announce)
    {
        OneMoreVar = 1;
    }

    if (CurNode->right == NULL)
    {
        return OneMoreVar;
    }

    return OneMoreVar + elf_find_new_var(CurNode->right);
}


void SElfBack::elf_create_param_var_table (SNode* CurNode)
{
    ME;

    // RAM_top_index = 1;

    do
    {
        if (CurNode->left != NULL)
        {
            elf_add_to_var_table (CurNode->left->left, true);
            Funcs->Table[Funcs->top_index].parameters++;
        }

        CurNode = CurNode->right;
    } while (CurNode != NULL);

    return;
}

void SElfBack::elf_delete_var_table ()
{
    ME;

    SVarTable* Table = NULL;

    pop_from_stack (VarStack, &Table);

    free (Table->Arr);

    free (Table);

    return;
}

int SElfBack::elf_find_var (SNode* CurNode)
{
    int RetIndex = JUNK;
    bool ParamMarker = false;

    MLA (VarStack->size != 0);
    MLA (CurNode->category == CLine && CurNode->type == TVariable);

    SVarTable* Table = NULL;

    int depth = 1;
    do
    {
        Table = VarStack->data[VarStack->size - depth];
        //printf ("=%p=%d/%d=\n", Table, depth, VarStack->size);
        depth++;
    } while ((elf_find_in_table (CurNode->data.var, Table, &RetIndex, &ParamMarker) == false) && (depth <= VarStack->size));

    if (RetIndex < 0)
    {
        printf ("==ERROR==\n""No '%ls' found!\n", CurNode->data.var);
        MLA (0);
    }

    if (ParamMarker == true)
    {
        RetIndex = -RetIndex;
    }

    return RetIndex;
}

//=============================================================================================================================================================================

bool SElfBack::elf_find_in_table (CharT* varName, SVarTable* Table, int* RetIndex, bool* ParamMarker)
{
    MLA (Table != NULL);

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

void make_label_to_jump (wchar_t* Label_name, size_t Address)
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

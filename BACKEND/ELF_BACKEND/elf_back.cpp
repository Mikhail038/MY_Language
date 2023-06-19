//==================================================================================================================================================================
//29.04.2023
//==================================================================================================================================================================

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <locale.h>
#include <wchar.h>
#include <string.h>

#include "ARRAY/array.h"
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

const ElfHead StandardElfHead = {};

void construct_elf_back (ElfBack* Back, FILE* ExFile)
{
    Back->Funcs         = (SBackFuncTable*)     calloc (1, sizeof (SBackFuncTable));
    Back->Funcs->Table  = (SBackFunc*)          calloc (MAX_FUNCS_ARRAY, sizeof (SBackFunc));
    Back->Funcs->top_index = 0;

    Back->VarStack      = (SStack<SVarTable*>*) calloc (1, sizeof (SStack<SVarTable*>));

    Back->head = &StandardElfHead;


    Back->file = ExFile;

    Back->table_condition = none;

    stack_constructor (Back->VarStack);

    Back->patches = (MyArray*) calloc(1, sizeof (MyArray));
    array_constructor(Back->patches, sizeof (Patch*));

    Back->ByteCodeArray = (char*) calloc(sizeof (char), MAX_ELF_SIZE);
    Back->cur_addr = 0;
}

void destruct_elf_back (ElfBack* Back)
{
    free_tables (Back->VarStack);

    free (Back->Funcs->Table);
    free (Back->Funcs);

    free (Back->ByteCodeArray);

    free_array(Back->patches);
    array_destructor(Back->patches);
}

//=============================================================================================================================================================================

void make_elf_file (AstNode* Root, FILE* ExFile)
{
    MY_LOUD_ASSERT (ExFile != NULL);

    ElfBack Back;

    construct_elf_back (&Back, ExFile);

    //generate_user_functions (Root, Back);

    generate_elf_array (&Back, Root);

    fwrite (Back.ByteCodeArray, sizeof (char), Back.cur_addr, Back.file);

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

#define ALIGN_4 align_one_byte(Back, 4)
#define ALIGN_8 align_one_byte(Back, 8)

void elf_head_start (ElfBack* Back)
{
    set_one_byte(Back, Back->head->magic_signature);
    set_bytes(Back, (void*) &(Back->head->elf_name), sizeof (Back->head->elf_name));
    set_one_byte(Back, Back->head->elf_class);
    set_one_byte(Back, Back->head->endian);
    set_one_byte(Back, Back->head->elf_version);
    set_one_byte(Back, Back->head->os_abi);

    set_one_byte(Back, Back->head->abi_version);
    align_one_byte(Back, 8);  //7 padding bytes

    set_one_byte(Back, Back->head->elf_type);
    align_one_byte(Back, 2);  //1 padding byte

    set_one_byte(Back, Back->head->machine);
    align_one_byte(Back, 2);  //1 padding byte

    set_one_byte(Back, Back->head->version);
    align_one_byte(Back, 4);  //3 padding bytes
}

void elf_head_start_params (ElfBack* Back)
{
    set_one_byte(Back, Back->head->flags);
    align_one_byte(Back, 4);

    set_bytes(Back, (void*) &(Back->head->header_size), sizeof (Back->head->header_size));
    set_bytes(Back, (void*) &(Back->head->program_header_size), sizeof (Back->head->program_header_size));
    set_bytes(Back, (void*) &(Back->head->program_headers_cnt), sizeof (Back->head->program_headers_cnt));
    set_bytes(Back, (void*) &(Back->head->section_headers_size), sizeof (Back->head->section_headers_size));
    set_bytes(Back, (void*) &(Back->head->section_headers_cnt), sizeof (Back->head->section_headers_cnt));
    set_bytes(Back, (void*) &(Back->head->shstr_table_index), sizeof (Back->head->shstr_table_index));
}

void elf_head_program_header_params (ElfBack* Back)
{
    set_one_byte(Back, Back->head->segment_type);
    ALIGN_4;

    set_one_byte(Back, Back->head->segment_flag);
    ALIGN_4;

    set_one_byte(Back, Back->head->segment_offset);
    ALIGN_8;

    set_bytes(Back, (void*) &(Back->head->file_virtual_address), sizeof (Back->head->file_virtual_address));
    set_bytes(Back, (void*) &(Back->head->file_virtual_address), sizeof (Back->head->file_virtual_address));
}

void elf_head_shstrtable (ElfBack* Back, size_t SegmentSizeSaved)
{
    UniversalNumber UnionBuffer;

    UnionBuffer.long_data = Back->cur_addr;
    paste_patch(Back, SegmentFileSize, UnionBuffer);

    UnionBuffer.long_data = Back->cur_addr;
    paste_patch(Back, TableAddress, UnionBuffer);
    size_t TableAddressSaved = UnionBuffer.long_data;

    UnionBuffer.long_data = TableAddressSaved + Back->head->file_virtual_address;
    paste_patch(Back, TableLoadAddress, UnionBuffer);

    set_one_byte(Back, 0x00); //begin section header string table

    UnionBuffer.int_data = Back->cur_addr - SegmentSizeSaved;
    paste_patch(Back, TextNameOffset, UnionBuffer);


    set_bytes(Back, (void*) &(Back->head->shstrtable_text_name), sizeof (Back->head->shstrtable_text_name));

    UnionBuffer.int_data = Back->cur_addr - SegmentSizeSaved;
    paste_patch(Back, TableNameOffset, UnionBuffer);

    set_bytes(Back, (void*) &(Back->head->shstrtable_table_name), sizeof (Back->head->shstrtable_table_name));
}

void generate_elf_array (ElfBack* Back, AstNode* Root)
{
    UniversalNumber UnionBuffer;

    elf_head_start (Back);

    create_and_skip_patch(Back, Back->cur_addr, SupportedPatches::FileVirtualAddress, sizeof (long int));
    create_and_skip_patch(Back, Back->cur_addr, SupportedPatches::ProgramHeadersStart, sizeof (long int));
    create_and_skip_patch(Back, Back->cur_addr, SupportedPatches::SectionHeadersStart, sizeof (long int));

    elf_head_start_params (Back);

    //=======================================================================

    UnionBuffer.long_data = Back->cur_addr;

    paste_patch(Back, SupportedPatches::ProgramHeadersStart, UnionBuffer);

    elf_head_program_header_params (Back);

    create_and_skip_patch(Back, Back->cur_addr, SupportedPatches::SegmentSize, sizeof (long int));

    create_and_skip_patch(Back, Back->cur_addr, SupportedPatches::SegmentFileSize, sizeof (long int));

    set_bytes(Back, (void*) &Back->head->alignment, sizeof (Back->head->alignment));

    //=======================================================================

    UnionBuffer.long_data = Back->cur_addr;
    paste_patch(Back, SupportedPatches::SectionHeadersStart, UnionBuffer);

    //=======================================================================
    //null

    for (int cnt_null_header = 0; cnt_null_header < 64; cnt_null_header++)
    {
        set_one_byte(Back, 0x00);
    }

    //=======================================================================
    //text

    create_and_skip_patch(Back, Back->cur_addr, SupportedPatches::TextNameOffset, sizeof (int));

    set_one_byte(Back, Back->head->text_section_type);
    ALIGN_4;
    set_one_byte(Back, Back->head->text_section_flags);
    ALIGN_8;

    create_and_skip_patch(Back, Back->cur_addr, SupportedPatches::EntryVirtualAddress, sizeof (size_t));

    create_and_skip_patch(Back, Back->cur_addr, SupportedPatches::TextOffset, sizeof (size_t));

    create_and_skip_patch(Back, Back->cur_addr, SupportedPatches::TextSize, sizeof (size_t));

    set_one_byte(Back, Back->head->text_section_index);
    ALIGN_4;
    set_one_byte(Back, Back->head->text_section_extra_info);
    ALIGN_4;
    set_one_byte(Back, Back->head->text_section_align);
    ALIGN_4;
    set_one_byte(Back, Back->head->text_section_extra_sizes);
    ALIGN_4;

    //=======================================================================
    //strtable

    create_and_skip_patch(Back, Back->cur_addr, SupportedPatches::TableNameOffset, sizeof (int));

    set_one_byte(Back, Back->head->shstrtable_section_type);
    ALIGN_4;
    set_one_byte(Back, Back->head->shstrtable_section_flags);
    ALIGN_8;

    create_and_skip_patch(Back, Back->cur_addr, SupportedPatches::TableLoadAddress, sizeof (size_t));

    create_and_skip_patch(Back, Back->cur_addr, SupportedPatches::TableAddress, sizeof (size_t));

    create_and_skip_patch(Back, Back->cur_addr, SupportedPatches::TableSize, sizeof (size_t));

    set_one_byte(Back, Back->head->shstrtable_section_index);
    ALIGN_4;
    set_one_byte(Back, Back->head->shstrtable_section_extra_info);
    ALIGN_4;
    set_one_byte(Back, Back->head->shstrtable_section_align);
    ALIGN_8;
    set_one_byte(Back, Back->head->shstrtable_section_extra_sizes);
    ALIGN_8;

    //=======================================================================
    //entry

    UnionBuffer.long_data = Back->cur_addr + Back->head->file_virtual_address;
    paste_patch(Back, FileVirtualAddress, UnionBuffer);

    paste_patch(Back, EntryVirtualAddress, UnionBuffer);

    //=======================================================================
    //.code

    UnionBuffer.long_data = Back->cur_addr;
    paste_patch(Back, TextOffset, UnionBuffer);
    size_t TextOffset_saved = UnionBuffer.long_data;

    //----------------------------------------------------------------------

    elf_generate_code (Back, Root);

    //----------------------------------------------------------------------

    UnionBuffer.long_data = Back->cur_addr - TextOffset_saved;
    paste_patch(Back, TextSize, UnionBuffer);

    //=======================================================================
    //shstrtable

    UnionBuffer.long_data = Back->cur_addr;
    paste_patch(Back, SegmentSize, UnionBuffer);
    size_t SegmentSizeSaved = UnionBuffer.long_data;

    elf_head_shstrtable (Back, SegmentSizeSaved);

    //=======================================================================

    UnionBuffer.long_data = Back->cur_addr - SegmentSizeSaved;
    paste_patch(Back, TableSize, UnionBuffer);

    return;
}

#undef ALIGN_4
#undef ALIGN_8

//=============================================================================================================================================================================

const int VAR_SIZE_BYTES = 8;

void elf_generate_code (ElfBack* Back, AstNode* Root)
{
    x86_call_label(Back, MAIN_LBL);

    x86_extra_exit(Back);

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
        Back->func_condition = main_f;
    }
    else
    {
        Back->func_condition = any_f;
    }

    x86_extra_paste_call_label(Back, CurNode->left->data.var);

    #ifdef DEBUG
    fprintf (stdout, LABEL "|%s|:\n", CurNode->left->data.var);
    #endif

    Back->Funcs->Table[Back->Funcs->top_index].Name = CurNode->left->data.var;

    Back->table_condition = none;
    elf_create_param_var_table (Back, CurNode->left->right);

    size_t AmountVars = 0;

    SVarTable* Table = NULL;
    if (Back->VarStack->size != 0)
    {

        pop_from_stack (Back->VarStack, &Table);

        AmountVars = elf_find_all_new_vars(Back, CurNode->right);

        Table->amount = AmountVars;

        push_in_stack(Back->VarStack, Table);

        #ifdef DEBUG
        printf ("Vars Amount: [%d]\n", AmountVars);
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

void elf_generate_assignment (ElfBack* Back, AstNode* CurNode)
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

    Back->table_condition = none;
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
            Back->table_condition = none;
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

    elf_generate_expression (Back, CurNode->left);

    x86_push_imm(Back, MY_FALSE);

    wchar_t* Label_end_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    prepare_name_label_to_jump (Label_end_name, Back->cur_addr);

    x86_cmp_stack(Back);
    x86_jump_label(Back, Label_end_name, x86_je);


    Back->table_condition = none;
    elf_generate_statement (Back, CurNode->right);
    ELF_CLEAN_TABLE ();

    x86_jump_label(Back, Label_body_name, x86_jmp);

    x86_extra_paste_jump_label(Back, Label_end_name);

    free (Label_body_name);
    free (Label_end_name);

    return;
}

void elf_generate_call (ElfBack* Back, AstNode* CurNode, bool RetValueMarker)
{

    elf_push_parameters (Back, CurNode->right);

    x86_call_label(Back, CurNode->left->data.var);

    elf_delete_function_parameters (Back, CurNode->right);

    if (RetValueMarker == true)
    {
        x86_push_reg(Back, ELF_FUNC_RET_REG);
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
    }

    elf_generate_node (Back, CurNode);

    return;
}

//=============================================================================================================================================================================

void elf_set_rax_var_value (ElfBack* Back, AstNode* CurNode)
{
    int Index = elf_find_var (Back, CurNode);

    if (Index < 0)
    {
        #ifdef DEBUG
        printf("Index: %d Delta_rbp: %d Final: %d\n", Index, Back->delta_rbp, Back->delta_rbp - Index * VAR_SIZE_BYTES);
        #endif

        x86_mov_to_reg_from_addr_in_reg_with_imm(Back, rax, rbp, Back->delta_rbp - Index * VAR_SIZE_BYTES);

        return;
    }

    #ifdef DEBUG
    printf("Index: %d\n", Index);
    #endif

    x86_mov_to_reg_from_addr_in_reg_with_imm(Back, rax, rbp, Index);

    return;
}

void elf_set_rax_var_address (ElfBack* Back, AstNode* CurNode)
{
    int Index = elf_find_var (Back, CurNode);

    x86_mov_reg_reg(Back, rax, rbp);

    if (Index < 0)
    {
        #ifdef DEBUG
        printf("Index: %d Delta_rbp: %d Final: %d\n", Index, Back->delta_rbp, Back->delta_rbp - Index * VAR_SIZE_BYTES);
        #endif

        x86_add_reg_imm(Back, rax, Back->delta_rbp - Index * VAR_SIZE_BYTES);

        return;
    }

    #ifdef DEBUG
    printf("Index: %d\n", Index);
    #endif

    x86_add_reg_imm(Back, rax, Index);

    return;
}

void elf_generate_pop_var (ElfBack* Back, AstNode* CurNode)
{
    elf_set_rax_var_address(Back, CurNode);

    x86_pop_to_reg(Back, FIRST_REG);

    x86_mov_to_addr_in_reg_from_reg(Back, rax, FIRST_REG);

    return;
}

void elf_generate_push_var (ElfBack* Back, AstNode* CurNode)
{
    elf_set_rax_var_value(Back, CurNode);

    x86_push_reg(Back, rax);

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
    const int STD_JUMP_TRUE_SHIFT  = 6;
    const int STD_JUMP_FALSE_SHIFT = 4;

    if (JumpMode != x86_jmp)
    {
        x86_cmp_stack (Back);
    }

    x86_jump_any(Back, STD_JUMP_TRUE_SHIFT, JumpMode); 

    x86_push_imm(Back, MY_FALSE);
    x86_jump_any(Back, STD_JUMP_FALSE_SHIFT, x86_jmp);

    x86_push_imm(Back, MY_TRUE);    

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
    if (Back->table_condition == VarTableConditions::none)
    {
        elf_create_new_var_table (Back, ParamMarker);
    }

    MY_LOUD_ASSERT (CurNode->category == CategoryLine && CurNode->type == TypeVariable);

    SVarTable* Table = NULL;
    peek_from_stack (Back->VarStack, &Table);

    Table->Arr[Table->cur_size].name  = CurNode->data.var; //copy address from node

    if (ParamMarker == false)
    {
        #ifdef DEBUG
        printf (KYLW "Amount: %d CurSize: %d Param: %d Final: %d => " KNRM, Table->amount, Table->cur_size, Table->amount_param, Table->amount - (Table->cur_size - Table->amount_param) - 1);
        #endif

        Table->Arr[Table->cur_size].index = (Table->amount - (Table->cur_size - Table->amount_param) - 1) * VAR_SIZE_BYTES;

        #ifdef DEBUG
        printf (KYLW "index <%d>\n" KNRM, Table->Arr[Table->cur_size].index);
        #endif

    }
    else
    {
        Table->Arr[Table->cur_size].index = - (Table->cur_size + 2); // another way to mark params of function

        #ifdef DEBUG
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

    #ifdef DEBUG_VARS
    if (ParamMarker == true)
    {
        printf("func params\n");
        NewTable->amount = elf_find_new_vars (Node);
        MY_LOUD_ASSERT (CurNode->type != TypeLinkerParameter);
        printf("amount %d\n", NewTable->amount);
    }
    else
    {
        NewTable->amount = elf_find_new_vars (CurNode);
        printf("amount %d\n", NewTable->amount);
    }
    #endif

    push_in_stack (Back->VarStack, NewTable);

    Back->table_condition = exist;

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

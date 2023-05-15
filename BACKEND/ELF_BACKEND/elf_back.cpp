//==================================================================================================================================================================
//29.04.2023
//==================================================================================================================================================================

#include <cstddef>
#include <cstdlib>
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
#include <bits/types/FILE.h>
#include <cstdio>

//=============================================================================================================================================================================

#define MAX_ELF_SIZE 100000

#define SET(x) Array[cnt] = (x); cnt++;

#define SET_2(x)     \
    memcpy ((unsigned short int*) &(Array[cnt]),    \
                        &(x), sizeof (unsigned short int));  \
    cnt += 2;

#define SET_4(x)     \
    memcpy ((unsigned int*) &(Array[cnt]),    \
                        &(x), sizeof (unsigned int));  \
    cnt += 4;

#define SET_8(x)     \
    memcpy ((size_t*) &(Array[cnt]),    \
                        &(x), sizeof (size_t));  \
    cnt += 8;

#define FILL_2  SET (0x00);

#define FILL_4  SET (0x00); SET (0x00); SET (0x00);

#define FILL_8  SET (0x00); SET (0x00); SET (0x00); SET (0x00); SET (0x00); SET (0x00); SET (0x00);

#define PASTE_8(x,y)  \
    (x) = cnt;    \
    memcpy ((size_t*) &(Array[(y)]), \
    &(x), sizeof (size_t));

#define PASTE_4(x,y)  \
    (x) = cnt;    \
    memcpy ((unsigned int*) &(Array[(y)]), \
    &(x), sizeof (unsigned int));

#define SKIP_8(x,y)   \
    size_t (x) = 0; \
    size_t (y) = cnt;    \
    cnt += 8; //we ll skip it now

#define SKIP_4(x,y)   \
    unsigned int (x) = 0; \
    size_t (y) = cnt;    \
    cnt += 4; //we ll skip it now

//=============================================================================================================================================================================

enum EJumpModes
{
    jmp_    = 0,
    ja_     = 1,
    jae_    = 2,
    jb_     = 3,
    jbe_    = 5,
    je_     = 6,
    jne_    = 7,
    jl_     = 8,
    jle_    = 9,
    jg_     = 10,
    jge_    = 11
};

#define QUADWORD_SIZE 32

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
    cnt = 0;
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

    fwrite (Back.Array, sizeof (char), Back.cnt, Back.file);

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

    memcpy ((size_t*) &(Array[cnt]),
    &FileVirtualAddress, sizeof (size_t));  //Virtual address
    cnt += 8;

    memcpy ((size_t*) &(Array[cnt]),
    &FileVirtualAddress, sizeof (size_t));  //Physical address (same as virtual)
    cnt += 8;

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

    size_t addr_another_EntryVA = cnt;
    cnt += 8; //skip

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

    EntryVA = cnt + FileVirtualAddress;
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

    TextSize = cnt - TextOffset;
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

    TextNameOffset = cnt - SegmentSize;
    memcpy ((unsigned int*) &(Array[addr_TextNameOffset]),
    &TextNameOffset, sizeof (unsigned int));

    SET ('.');
    SET ('t');
    SET ('e');
    SET ('x');
    SET ('t');
    SET (0x00); //end

    TableNameOffset = cnt - SegmentSize;
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

    TableSize = cnt - SegmentSize;
    memcpy ((size_t*) &(Array[addr_TableSize]),
    &TableSize, sizeof (size_t));

    return;
}

//=============================================================================================================================================================================

void SElfBack::elf_generate_code (SNode* Root)
{
    x86_mov_r_i(eSHIFT_REG, 0);

    x86_mov_r_i(eTOP_REG, cnt - start_cnt); // TODO maybe not

    // Labels["main"] = {cnt, 0};
    // x86_jump (8, jmp_);

    x86_call_label (L"main");

    x86_nop();
    x86_nop();
    x86_nop();
    x86_nop();

    x86___paste_label(L"main");

    x86___End();

    SET(0x00);
    FILL_8;

//     x86_push_i(38*13);
//     x86_push_i(13);
//     x86_idiv_stack();
//     x86_pop_r(r10);
//
//     x86_mov_IrI_r(r9, rdx);

    //TODO JUMP MAIN SOMEHOW

    //elf_generate_statement (Root);

//     SET (0xb8);
//     SET (0x01);
//     SET (0x00);
//     SET (0x00);
//     SET (0x00);
//     SET (0xbf);
//     SET (0x01);
//     SET (0x00);
//     SET (0x00);
//     SET (0x00);
//     SET (0x48);
//     SET (0xbe);
//     SET (0x5f);
//     SET (0x81);
//     SET (0x02);
//     SET (0x00);
//     SET (0x00);
//     SET (0x00);
//     SET (0x00);
//     SET (0x00);
//     SET (0xba);
//     SET (0x0f);
//     SET (0x00);
//     SET (0x00);
//     SET (0x00);
//     SET (0x0f);
//     SET (0x05);
//     SET (0xb8);
//     SET (0x3c);
//     SET (0x00);
//     SET (0x00);
//     SET (0x00);
//     SET (0xbf);
//     SET (0x00);
//     SET (0x00);
//     SET (0x00);
//     SET (0x00);
//     SET (0x0f);
//     SET (0x05);
//
//     SET (0x48);
//     SET (0x65);
//     SET (0x6c);
//     SET (0x6c);
//     SET (0x6f);
//     SET (0x2c);
//     SET (0x20);
//     SET (0x77);
//     SET (0x6f);
//     SET (0x72);
//     SET (0x6c);
//     SET (0x64);
//     SET (0x21);
//     SET (0x0a);
//     SET (0x00);

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
    x86___paste_label(CurNode->left->data.var);
    // fprintf (file, LABEL "%ls:\n", CurNode->left->data.var);

    Funcs->Table[Funcs->top_index].Name = CurNode->left->data.var;

    table_cond = none;
    elf_create_param_var_table (CurNode->left->right);

    elf_pop_parameters ();

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

    elf_add_to_var_table (CurNode->left);

    elf_generate_pop_var (CurNode->left);

    elf_incr_top_reg ();

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
        writeln_command (inp, file);

        elf_generate_pop_var (Node->left);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void SElfBack::elf_generate_output (SNode* CurNode)
{
    SNode* Node = CurNode->left;

    do
    {
        elf_generate_push_var (Node->left);

        writeln_command (out, file);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void SElfBack::elf_generate_if (SNode* CurNode)
{
    elf_generate_postorder (CurNode->left);

    x86_push_i(FALSE);

    PUT (je);
    fprintf (file, " " LABEL "%d\n", label_cnt);
    int Label_1 = label_cnt;
    label_cnt++;

    CurNode = CurNode->right;

    table_cond = none;
    elf_generate_statement (CurNode->left);
    ELF_CLEAN_TABLE;

    PUT (jump);
    fprintf (file, " " LABEL "%d\n", label_cnt);
    int Label_2 = label_cnt;
    label_cnt++;

    fprintf (file,  LABEL "%d:\n", Label_1);

    CurNode = CurNode->right;

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
    fprintf (file,  LABEL "%d:\n", Label_2);


    return;
}

void SElfBack::elf_generate_while (SNode* CurNode)
{
    int Label_1 = label_cnt;
    fprintf (file,  LABEL "%d:\n", Label_1);
    label_cnt++;

    elf_generate_postorder (CurNode->left);

    x86_push_i(FALSE);

    PUT (je);
    int Label_2 = label_cnt;
    fprintf (file, " " LABEL "%d\n", Label_2);
    label_cnt++;

    table_cond = none;
    elf_generate_statement (CurNode->right);
    ELF_CLEAN_TABLE;

    PUT (jump);
    fprintf (file, " " LABEL "%d\n", Label_1);

    fprintf (file,  LABEL "%d:\n", Label_2);

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

    x86_pop_r(eFUNC_REG);

    x86_ret();

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

void SElfBack::elf_generate_pop_var (SNode* CurNode)
{
    int Index = elf_find_var (CurNode);

    x86_mov_r_r(eCOUNT_REG, eSHIFT_REG);
    x86_add_i(eCOUNT_REG, Index);

    x86_pop_r(A_REG);
    x86_mov_IrI_r(eCOUNT_REG, A_REG);

    // PUT (pop);
    // fprintf (file, " [" COUNT_REG "]\n\n"); // this was

    return;
}

void SElfBack::elf_generate_push_var (SNode* CurNode)
{
    int Index = elf_find_var (CurNode);

    x86_mov_r_r(eCOUNT_REG, eSHIFT_REG);
    x86_add_i(eCOUNT_REG, Index);

    x86_push_IrI(eCOUNT_REG);

    return;
}

//=============================================================================================================================================================================

void SElfBack::elf_push_parameters (SNode* CurNode)
{
    if (CurNode->right != NULL)
    {
        elf_push_parameters (CurNode->right);
    }

    if (CurNode != NULL)
    {
        elf_generate_expression (CurNode->left);
    }

    return;
}

void SElfBack::elf_pop_parameters ()
{
    x86_push_r(eSHIFT_REG);

    x86_pop_r(eCOUNT_REG);

    for (int i = 0; i < Funcs->Table[Funcs->top_index].parameters ; i++)
    {
        x86_inc(eCOUNT_REG);

        x86_pop_r(A_REG);
        x86_mov_IrI_r(eCOUNT_REG, A_REG);

        // PUT (pop);
        // fprintf (file, " [" COUNT_REG "]\n\n");  // this was
    }

    return;
}

void SElfBack::elf_incr_top_reg ()
{
    x86_inc(eTOP_REG);

    return;
}

#define STD_JUMP_FIRST_SHIFT    4   //DO NOT change
#define STD_JUMP_SECOND_SHIFT   2   //DO NOT change

void SElfBack::elf_standard_if_jump (int JumpMode)
{
    if (JumpMode != jmp_)
    {
        x86_cmp_stack ();
    }

    x86_jump(STD_JUMP_FIRST_SHIFT, JumpMode);

    // fprintf (file, " " LABEL "%d\n", label_cnt);
    // label_cnt++;

    x86_push_i(FALSE);

    // PUT (push);
    // fprintf (file, " %d\n", FALSE);

    x86_jump(STD_JUMP_SECOND_SHIFT, jmp_);

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

void SElfBack::elf_add_to_var_table (SNode* CurNode)
{
    if (table_cond == none)
    {
        elf_create_new_var_table ();
    }

    MLA (CurNode->category == CLine && CurNode->type == TVariable);

    SVarTable* Table = NULL;
    peek_from_stack (VarStack, &Table);

    Table->Arr[Table->size].name  = CurNode->data.var; //copy address from node
    Table->Arr[Table->size].index = RAM_top_index;

    RAM_top_index++;
    Table->size++;

    return;
}

void SElfBack::elf_create_new_var_table ()
{
    ME;

    SVarTable* NewTable = (SVarTable*)  calloc (1,                  sizeof (SVarTable));
    NewTable->Arr       = (SVarAccord*) calloc (VAR_TABLE_CAPACITY, sizeof (SVarAccord));
    NewTable->size = 0;

    push_in_stack (VarStack, NewTable);

    table_cond = exist;

    return;
}

void SElfBack::elf_create_param_var_table (SNode* CurNode)
{
    ME;

    RAM_top_index = 1;

    do
    {
        if (CurNode->left != NULL)
        {
            elf_add_to_var_table (CurNode->left->left);
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

    MLA (VarStack->size != 0);
    MLA (CurNode->category == CLine && CurNode->type == TVariable);

    SVarTable* Table = NULL;

    int depth = 1;
    do
    {
        Table = VarStack->data[VarStack->size - depth];
        //printf ("=%p=%d/%d=\n", Table, depth, VarStack->size);
        depth++;
    } while ((find_in_table (CurNode->data.var, Table, &RetIndex) == false) && (depth <= VarStack->size));

    if (RetIndex < 0)
    {
        printf ("==ERROR==\n""No '%ls' found!\n", CurNode->data.var);
        MLA (0);
    }

    return RetIndex;
}

//=============================================================================================================================================================================

void SElfBack::set_hex_int (int Number)
{
    SET_4(Number);
}

void SElfBack::set_hex_long (long Address)
{
    SET_8(Address);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_push_i (int Number)
{
    if (Number <= 127)
    {
        SET(0x6a);

        SET((char) Number);

        return;
    }

    SET(0x68);

    set_hex_int (Number);
}

void SElfBack::x86_push_r (int Register)
{
    if (Register >= DELTA)
    {
        SET(0x41);
        Register-= DELTA;
    }

    x86___Reg_config(Register, 0x50);
}

void SElfBack::x86_push_IrI (int Register)
{
    if (Register >= DELTA)
    {
        SET(0x41);
        Register-= DELTA;
    }

    SET(0xff);

    x86___Reg_config(Register, 0x30);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_pop_r (int Register)
{
    if (Register >= DELTA)
    {
        SET(0x41);
        Register-= DELTA;
    }

    x86___Reg_config(Register, 0x58);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_nop ()
{
    SET(0x90);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_call (int Shift)
{
    SET(0xe8);

    set_hex_int(Shift);
}

void SElfBack::x86_call_label (const wchar_t* Name)
{
    if (Labels.find(Name) != Labels.end())
    {
        if (Labels[Name].finish != NULL_FINISH)
        {
            x86_call (Labels[Name].finish - cnt);
        }
        else
        {
            Labels[Name].start[Labels[Name].cnt] = cnt;
            Labels[Name].cnt++;

            for (size_t i = 0; i < 5; ++i)
            {
                x86_nop();
            }
        }
    }
    else
    {
        Labels.insert({Name, {cnt, 0}});
    }
}


void SElfBack::x86_ret ()
{
    SET (0xc3);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_syscall ()
{
    SET (0x0f);
    SET (0x05);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_cmp_r_r (int dstReg, int srcReg)
{
    x86___DstSrc_config(dstReg, srcReg);

    SET(0x39);

    x86___Regs_config(dstReg, srcReg);
}

void SElfBack::x86_cmp_stack ()
{
    x86_pop_r(A_REG); //TODO maybe other order
    x86_pop_r(B_REG);

    x86_cmp_r_r (A_REG, B_REG);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_jump (int Shift, int JumpMode)
{
    switch (JumpMode)
    {
        case jmp_:
            SET(0xeb);
            break;

        case ja_:
            SET(0x77);
            break;

        case jae_:
            SET(0x73);
            break;

        case jb_:
            SET(0x72);
            break;

        case jbe_:
            SET(0x76);
            break;

        case je_:
            SET(0x74);
            break;

        case jne_:
            SET(0x75);
            break;

        case jl_:
            SET(0x7c);
            break;

        case jle_:
            SET(0x7e);
            break;

        case jg_:
            SET(0x7f);
            break;

        case jge_:
            SET(0x7d);
            break;

        default:
            MLA(0);
    }


    char offset = 0x00;

    offset += Shift;

    SET(offset);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86___End ()
{
    x86_mov_r_i(rax, 0x3c);
    x86_mov_r_i(rdi, 0);

    x86_syscall ();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86___paste_label (const wchar_t* Name)
{
    if (Labels.find(Name) != Labels.end())
    {
        Labels[Name].finish = cnt;

        for (size_t i = 0; i < Labels[Name].cnt; ++i)
        {
            cnt = Labels[Name].start[i];   // go there

            x86_call (Labels[Name].finish - Labels[Name].start[i]); //paste code

            cnt = Labels[Name].finish;  // go back
        }
    }
    else
    {
        Labels.insert({Name, {cnt}});   //TODO fix this
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86___DstSrc_config (int& dstReg, int& srcReg)
{
    if ((srcReg >= DELTA) && (dstReg >= DELTA))
    {
        SET(0x4d);
        srcReg-= DELTA;
        dstReg-= DELTA;
    }
    else if (srcReg >= DELTA)
    {
        SET(0x4c);
        srcReg-= DELTA;
    }
    else if (dstReg >= DELTA)
    {
        SET(0x49);
        dstReg-= DELTA;
    }
    else
    {
        SET(0x48);
    }
}

void SElfBack::x86___Dst_config (int& dstReg)
{
    if (dstReg >= DELTA)
    {
        SET(0x49);
        dstReg-= DELTA;
    }
    else
    {
        SET(0x48);
    }
}

void SElfBack::x86___Regs_config(const int dstReg, const int srcReg)
{
    char opcode = 0xc0;
    opcode += (char) dstReg;
    opcode += (char) srcReg * 8;

    SET(opcode);
}

void SElfBack::x86___Reg_config(const int Reg, const int ZeroPoint)
{
    char opcode = ZeroPoint;
    opcode += (char) Reg;

    SET(opcode);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_mov_r_r (int dstReg, int srcReg)
{
    x86___DstSrc_config(dstReg, srcReg);

    SET(0x89);

    x86___Regs_config(dstReg, srcReg);
}

void SElfBack::x86_mov_r_i (int dstReg, int Number)
{
    x86___Dst_config(dstReg);

    SET(0xc7);

    x86___Reg_config(dstReg, 0xc0);

    set_hex_int(Number);
}

void SElfBack::x86_mov_r_IrI (int dstReg, int srcReg)
{
    x86___DstSrc_config(dstReg, srcReg);

    SET(0x8b);

    if ((srcReg == rsp) || (srcReg == r12))
    {
        char opcode = 0x04;
        opcode += (char) dstReg * 8;

        SET(opcode);

        SET(0x24);

        return;
    }
    if ((srcReg == rbp) || (srcReg == r13))
    {
        char opcode = 0x45;
        opcode += (char) dstReg * 8;

        SET(opcode);

        SET(0x00);

        return;
    }

    char opcode = 0x00;
    opcode += (char) dstReg;
    opcode += (char) srcReg * 8;

    SET(opcode);
}

void SElfBack::x86_mov_IrI_r (int dstReg, int srcReg)
{
    x86___DstSrc_config(dstReg, srcReg);

    SET(0x89);

    char opcode = 0x00;
    opcode += (char) dstReg;
    opcode += (char) srcReg * 8;

    SET(opcode);
}

void SElfBack::x86_mov_r_Ir_iI (int dstReg, int srcReg, int Shift)
{
    x86___DstSrc_config(dstReg, srcReg);

    SET(0x8b);

    if ((srcReg == rsp) || (srcReg == r12))
    {
        char opcode = 0x44;
        opcode += (char) dstReg * 8;

        SET(opcode);

        SET(0x24);

        set_hex_int(Shift);

        return;
    }
    if ((srcReg == rbp) || (srcReg == r13))
    {
        char opcode = 0x45;
        opcode += (char) dstReg * 8;

        SET(opcode);

        set_hex_int(Shift);

        return;
    }

    if (Shift != 0)
    {
        char opcode = 0x40;
        opcode += (char) dstReg;
        opcode += (char) srcReg * 8;

        SET(opcode);

        set_hex_int(Shift);
    }
    else
    {
        char opcode = 0x00;
        opcode += (char) dstReg;
        opcode += (char) srcReg * 8;

        SET(opcode);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_add_stack ()
{
    int a_reg = A_REG;
    int b_reg = B_REG;

    x86_pop_r(b_reg);
    x86_pop_r(a_reg);

    x86___DstSrc_config(a_reg, b_reg);

    SET(0x01);

    x86___Regs_config(a_reg, b_reg);

    x86_push_i(a_reg);
}

void SElfBack::x86_sub_stack ()
{
    int a_reg = A_REG;
    int b_reg = B_REG;

    x86_pop_r(b_reg);
    x86_pop_r(a_reg);

    x86___DstSrc_config(a_reg, b_reg);

    SET(0x29);

    x86___Regs_config(a_reg, b_reg);

    x86_push_i(a_reg);
}

void SElfBack::x86_imul_stack ()
{
    int a_reg = A_REG;

    x86_pop_r(a_reg);
    x86_pop_r(rax);

    x86___Dst_config(a_reg);

    SET(0xf7);

    x86___Reg_config(a_reg, 0xe8);

    x86_push_i(rax);
}

void SElfBack::x86_idiv_stack ()
{
    x86_mov_r_i(rdx, 0);

    int a_reg = A_REG;

    x86_pop_r(a_reg);
    x86_pop_r(rax);

    x86___Dst_config(a_reg);

    SET(0xf7);

    x86___Reg_config(a_reg, 0xf8);

    x86_push_r(rax);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_add_i (int Register, int Number)
{
    x86___Dst_config(Register);

    SET(0x83);

    x86___Reg_config(Register, 0xc0);

    set_hex_int(Number);
}

void SElfBack::x86_sub_i (int Register, int Number)
{
    x86___Dst_config(Register);

    SET(0x83);

    x86___Reg_config(Register, 0xe8);

    set_hex_int(Number);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_inc (int Register)
{
    x86___Dst_config(Register);

    SET(0xff);

    x86___Reg_config(Register, 0xc0);
}

void SElfBack::x86_dec (int Register)
{
    x86___Dst_config(Register);

    SET(0xff);

    x86___Reg_config(Register, 0xc8);
}

//=============================================================================================================================================================================


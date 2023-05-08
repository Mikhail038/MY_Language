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

#define MAX_ELF_SIZE 10000

#define SET(x) Back->Array[Back->cnt] = (x); Back->cnt++;

#define SET_2(x)     \
    memcpy ((unsigned short int*) &(Back->Array[Back->cnt]),    \
                        &(x), sizeof (unsigned short int));  \
    Back->cnt += 2;

#define SET_4(x)     \
    memcpy ((unsigned int*) &(Back->Array[Back->cnt]),    \
                        &(x), sizeof (unsigned int));  \
    Back->cnt += 4;

#define FILL_2  SET (0x00);

#define FILL_4  SET (0x00); SET (0x00); SET (0x00);

#define FILL_8  SET (0x00); SET (0x00); SET (0x00); SET (0x00); SET (0x00); SET (0x00); SET (0x00);

#define PASTE_8(x,y)  \
    (x) = Back->cnt;    \
    memcpy ((size_t*) &(Back->Array[(y)]), \
    &(x), sizeof (size_t));

#define PASTE_4(x,y)  \
    (x) = Back->cnt;    \
    memcpy ((unsigned int*) &(Back->Array[(y)]), \
    &(x), sizeof (unsigned int));

#define SKIP_8(x,y)   \
    size_t (x) = 0; \
    size_t (y) = Back->cnt;    \
    Back->cnt += 8; //we ll skip it now

#define SKIP_4(x,y)   \
    unsigned int (x) = 0; \
    size_t (y) = Back->cnt;    \
    Back->cnt += 4; //we ll skip it now

//=============================================================================================================================================================================

#define eSHIFT_REG   r12
#define eCOUNT_REG   r13
#define eTOP_REG     r14
#define eFUNC_REG    r15

//=============================================================================================================================================================================

SElfBack* elf_back_constructor (FILE* ExFile)
{
    SElfBack* Back = (SElfBack*) calloc (1, sizeof (SElfBack));

    Back->Funcs         = (SBackFuncTable*)     calloc (1, sizeof (SBackFuncTable));
    Back->Funcs->Table  = (SBackFunc*)          calloc (MAX_FUNCS_ARRAY, sizeof (SBackFunc));
    Back->Funcs->top_index = 0;

    Back->VarStack      = (SStack<SVarTable*>*) calloc (1, sizeof (SStack<SVarTable*>));

    Back->file = ExFile;

    Back->table_cond = none;

    stack_constructor (Back->VarStack, 4);

    Back->Array = (char*) calloc(sizeof (char), MAX_ELF_SIZE);
    Back->cnt = 0;

    return Back;
}

void elf_back_destructor (SElfBack* Back)
{
    free_tables (Back->VarStack);

    free (Back->Funcs->Table);
    free (Back->Funcs);

    free (Back->Array);

    free (Back);

    return;
}

//=============================================================================================================================================================================


void make_elf_file (SNode* Root, FILE* ExFile)
{
    MLA (ExFile != NULL);

    SElfBack* Back = elf_back_constructor (ExFile);

    //generate_user_functions (Root, Back);

    generate_elf_array (Root, Back);

    fwrite (Back->Array, sizeof (char), Back->cnt, Back->file);

    elf_back_destructor (Back);

    // for (int i = 0; i < Back->Funcs->top_index; i++)
    // {
    //     printf ("'%ls'[%d]\n", Back->Funcs->Table[i]);
    // }

    return;
}

//=============================================================================================================================================================================
//Make ELF//
//=============================================================================================================================================================================

void generate_elf_array (SNode* Root, SElfBack* Back)
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

    memcpy ((size_t*) &(Back->Array[Back->cnt]),
    &FileVirtualAddress, sizeof (size_t));  //Virtual address
    Back->cnt += 8;

    memcpy ((size_t*) &(Back->Array[Back->cnt]),
    &FileVirtualAddress, sizeof (size_t));  //Physical address (same as virtual)
    Back->cnt += 8;

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

    size_t addr_another_EntryVA = Back->cnt;
    Back->cnt += 8; //skip

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

    EntryVA = Back->cnt + FileVirtualAddress;
    memcpy ((size_t*) &(Back->Array[addr_EntryVA]),
    &EntryVA, sizeof (size_t));

    memcpy ((size_t*) &(Back->Array[addr_another_EntryVA]),
    &EntryVA, sizeof (size_t)); //TODO maybe remove

    //=======================================================================
    //.code

    PASTE_8(TextOffset, addr_TextOffset);

    //----------------------------------------------------------------------

    elf_generate_code (Root, Back);

    //----------------------------------------------------------------------

    TextSize = Back->cnt - TextOffset;
    memcpy ((size_t*) &(Back->Array[addr_TextSize]),
    &TextSize, sizeof (size_t));

    //=======================================================================
    //shstrtable

    PASTE_8(SegmentSize, addr_SegmentSize);

    PASTE_8(SegmentFileSize, addr_SegmentFileSize);

    PASTE_8(TableAddress, addr_TableAddress);

    TableLoadAddress = TableAddress + FileVirtualAddress;
    memcpy ((size_t*) &(Back->Array[addr_TableLoadAddress]),
    &TableLoadAddress, sizeof (size_t));


    SET (0x00); //begin section header string table

    TextNameOffset = Back->cnt - SegmentSize;
    memcpy ((unsigned int*) &(Back->Array[addr_TextNameOffset]),
    &TextNameOffset, sizeof (unsigned int));

    SET ('.');
    SET ('t');
    SET ('e');
    SET ('x');
    SET ('t');
    SET (0x00); //end

    TableNameOffset = Back->cnt - SegmentSize;
    memcpy ((unsigned int*) &(Back->Array[addr_TableNameOffset]),
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

    TableSize = Back->cnt - SegmentSize;
    memcpy ((size_t*) &(Back->Array[addr_TableSize]),
    &TableSize, sizeof (size_t));

    return;
}

//=============================================================================================================================================================================

void elf_generate_code (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
//     x86_push_imm (BACK_FUNC_PARAMETERS, 0);
//
//     fprintf (Back->file, " 0\n"); //TODO ask Danya maybe %d 0
//     PUT (pop);
//     fprintf (Back->file, " " SHIFT_REG "\n\n");
//
//
//     PUT (push);
//     fprintf (Back->file, " 0\n");
//     PUT (pop);
//     fprintf (Back->file, " " TOP_REG "\n\n");
//
//     PUT (jump);
//     fprintf (Back->file, " " LABEL MAIN_JUMP "\n\n");
//
//     fprintf (Back->file, SEP_LINE "\n\n");

    // elf_generate_statement (CurNode, Back);

    x86_push_r(BACK_FUNC_PARAMETERS, rcx);
    x86_pop_r(BACK_FUNC_PARAMETERS, rcx);

    SET (0xb8);
    SET (0x01);
    SET (0x00);
    SET (0x00);
    SET (0x00);
    SET (0xbf);
    SET (0x01);
    SET (0x00);
    SET (0x00);
    SET (0x00);
    SET (0x48);
    SET (0xbe);
    SET (0x5f);
    SET (0x81);
    SET (0x02);
    SET (0x00);
    SET (0x00);
    SET (0x00);
    SET (0x00);
    SET (0x00);
    SET (0xba);
    SET (0x0f);
    SET (0x00);
    SET (0x00);
    SET (0x00);
    SET (0x0f);
    SET (0x05);
    SET (0xb8);
    SET (0x3c);
    SET (0x00);
    SET (0x00);
    SET (0x00);
    SET (0xbf);
    SET (0x00);
    SET (0x00);
    SET (0x00);
    SET (0x00);
    SET (0x0f);
    SET (0x05);

    SET (0x48);
    SET (0x65);
    SET (0x6c);
    SET (0x6c);
    SET (0x6f);
    SET (0x2c);
    SET (0x20);
    SET (0x77);
    SET (0x6f);
    SET (0x72);
    SET (0x6c);
    SET (0x64);
    SET (0x21);
    SET (0x0a);
    SET (0x00);

/*
    fprintf (Back->file, SEP_LINE "\n\n");

    PUT (push);
    fprintf (Back->file, " 0\n"); //TODO ask Danya maybe %d 0
    PUT (pop);
    fprintf (Back->file, " " SHIFT_REG "\n\n");


    PUT (push);
    fprintf (Back->file, " 0\n");
    PUT (pop);
    fprintf (Back->file, " " TOP_REG "\n\n");

    PUT (jump);
    fprintf (Back->file, " " LABEL MAIN_JUMP "\n\n");

    fprintf (Back->file, SEP_LINE "\n\n");

    elf_generate_statement (BACK_FUNC_PARAMETERS);

    //delete_var_table (Back);
*/
    return;
}

void elf_generate_statement (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    if (CurNode->left != NULL)
    {
        elf_generate_op_node (BACK_LEFT_SON_FUNC_PARAMETERS);
    }

    if (CurNode->right != NULL)
    {
        elf_generate_statement (BACK_RIGHT_SON_FUNC_PARAMETERS);
    }

    return;
}

void elf_generate_function (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    if (wcscmp (CurNode->left->data.var, MAIN_WORD) == 0)
    {
        Back->func_cond = main_f;
    }
    else
    {
        Back->func_cond = any_f;
    }

    fprintf (Back->file, LABEL "%ls:\n", CurNode->left->data.var);

    Back->Funcs->Table[Back->Funcs->top_index].Name = CurNode->left->data.var;

    Back->table_cond = none;
    elf_create_param_var_table (CurNode->left->right, Back);

    elf_pop_parameters (Back);

    Back->Funcs->top_index++;


    elf_generate_statement (BACK_RIGHT_SON_FUNC_PARAMETERS);
    ELF_CLEAN_TABLE;

    fprintf (Back->file, "\n\n" SEP_LINE "\n\n\n");

    return;
}

void elf_generate_node (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    if (CurNode->category == COperation)
    {
        elf_generate_op_node (BACK_FUNC_PARAMETERS);
    }

    if (CurNode->category == CValue)
    {
        elf_write_command (push, Back->file);

        fprintf (Back->file, " %lg\n", CurNode->data.val);
    }

    if (CurNode->category == CLine && CurNode->type == TVariable)
    {
        elf_generate_push_var (BACK_FUNC_PARAMETERS);
//         write_command (push, Back->file);
//
//         int Index = find_var (BACK_FUNC_PARAMETERS);
//
//         fprintf (Back->file, " [%d]\n", Index);
    }

    return;
}

void elf_generate_op_node (ELF_BACK_FUNC_HEAD_PARAMETERS)
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

void elf_generate_announce (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    elf_generate_expression (BACK_RIGHT_SON_FUNC_PARAMETERS);

    elf_add_to_var_table (BACK_LEFT_SON_FUNC_PARAMETERS);

    elf_generate_pop_var (BACK_LEFT_SON_FUNC_PARAMETERS);

    elf_incr_top_reg (Back);
//     write_command (pop, Back->file);
//
//     fprintf (Back->file, " [%d]\n\n", Back->RAM_top_index - 1);

    return;
}

void elf_generate_equation (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    elf_generate_expression (BACK_RIGHT_SON_FUNC_PARAMETERS);

    elf_generate_pop_var (BACK_LEFT_SON_FUNC_PARAMETERS);

    return;
}

void elf_generate_input (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    SNode* Node = CurNode->left;

    do
    {
        writeln_command (inp, Back->file);

        elf_generate_pop_var (Node->left, Back);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void elf_generate_output (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    SNode* Node = CurNode->left;

    do
    {
        elf_generate_push_var (Node->left, Back);

        writeln_command (out, Back->file);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void elf_generate_if (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    elf_generate_postorder (BACK_LEFT_SON_FUNC_PARAMETERS);

    PUT (push);
    fprintf (Back->file, " %d\n", FALSE);

    PUT (je);
    fprintf (Back->file, " " LABEL "%d\n", Back->label_cnt);
    int Label_1 = Back->label_cnt;
    Back->label_cnt++;

    CurNode = CurNode->right;

    Back->table_cond = none;
    elf_generate_statement (BACK_LEFT_SON_FUNC_PARAMETERS);
    ELF_CLEAN_TABLE;

    PUT (jump);
    fprintf (Back->file, " " LABEL "%d\n", Back->label_cnt);
    int Label_2 = Back->label_cnt;
    Back->label_cnt++;

    fprintf (Back->file,  LABEL "%d:\n", Label_1);

    CurNode = CurNode->right;

    if (CurNode != NULL)
    {
        if (CurNode->category == COperation && CurNode->type == TIf)
        {
            elf_generate_if (BACK_FUNC_PARAMETERS);
        }
        else
        {
            Back->table_cond = none;
            elf_generate_statement (BACK_FUNC_PARAMETERS);
            ELF_CLEAN_TABLE;
        }
    }
    fprintf (Back->file,  LABEL "%d:\n", Label_2);


    return;
}

void elf_generate_while (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    int Label_1 = Back->label_cnt;
    fprintf (Back->file,  LABEL "%d:\n", Label_1);
    Back->label_cnt++;

    elf_generate_postorder (BACK_LEFT_SON_FUNC_PARAMETERS);

    PUT (push);
    fprintf (Back->file, " %d\n", FALSE);

    PUT (je);
    int Label_2 = Back->label_cnt;
    fprintf (Back->file, " " LABEL "%d\n", Label_2);
    Back->label_cnt++;

    Back->table_cond = none;
    elf_generate_statement (BACK_RIGHT_SON_FUNC_PARAMETERS);
    ELF_CLEAN_TABLE;

    PUT (jump);
    fprintf (Back->file, " " LABEL "%d\n", Label_1);

    fprintf (Back->file,  LABEL "%d:\n", Label_2);

    return;
}

void elf_generate_call (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    fprintf (Back->file, ";++" "\n\n");

    PUT (push);
    fprintf (Back->file, " " TOP_REG "\n");

    elf_push_parameters (BACK_RIGHT_SON_FUNC_PARAMETERS);

    PUT (push);
    fprintf (Back->file, " " TOP_REG "\n");

    PUT (pop);
    fprintf (Back->file, " " SHIFT_REG "\n\n");

    PUT (call);
    fprintf (Back->file, " " LABEL "%ls\n", CurNode->left->data.var);

    PUT (pop);
    fprintf (Back->file, " " TOP_REG "\n");

    fprintf (Back->file, ";++" "\n\n");

    PUT (push);
    fprintf (Back->file, " " FUNC_REG "\n");

    return;
}

void elf_generate_return (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    if (Back->func_cond == main_f)
    {
        writeln_command (hlt, Back->file);
    }
    else
    {
        elf_generate_expression (BACK_LEFT_SON_FUNC_PARAMETERS);

        PUT (pop);
        fprintf (Back->file, " " FUNC_REG "\n");

        PUTLN (ret);
    }

    return;
}

void elf_generate_expression (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    elf_generate_postorder (BACK_FUNC_PARAMETERS);

    return;
}

void elf_generate_postorder (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    if (!(CurNode->category == COperation && CurNode->type == T_Call) && CurNode->left != NULL)
    {
        elf_generate_postorder (BACK_LEFT_SON_FUNC_PARAMETERS);
    }

    if (!(CurNode->category == COperation && CurNode->type == T_Call) && CurNode->right != NULL)
    {
        elf_generate_postorder (BACK_RIGHT_SON_FUNC_PARAMETERS);
    }

    elf_generate_node (BACK_FUNC_PARAMETERS);

    return;
}

void elf_generate_pop_var (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    int Index = elf_find_var (BACK_FUNC_PARAMETERS);

    PUT (push);
    fprintf (Back->file, " %d\n", Index);

    PUT (push);
    fprintf (Back->file, " " SHIFT_REG "\n");

    PUTLN (add);

    PUT (pop);
    fprintf (Back->file, " " COUNT_REG "\n");


    PUT (pop);
    fprintf (Back->file, " [" COUNT_REG "]\n\n");

    return;
}

void elf_generate_push_var (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    int Index = elf_find_var (BACK_FUNC_PARAMETERS);

    PUT (push);
    fprintf (Back->file, " %d\n", Index);

    PUT (push);
    fprintf (Back->file, " " SHIFT_REG "\n");

    PUTLN (add);

    PUT (pop);
    fprintf (Back->file, " " COUNT_REG "\n");

    PUT (push);
    fprintf (Back->file, " [" COUNT_REG "]\n\n");

    return;
}

//=============================================================================================================================================================================

void elf_push_parameters (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    if (CurNode->right != NULL)
    {
        elf_push_parameters (BACK_RIGHT_SON_FUNC_PARAMETERS);
    }

    if (CurNode != NULL)
    {
        elf_generate_expression (BACK_LEFT_SON_FUNC_PARAMETERS);
    }

    // PUT (push);
    // fprintf (Back->file, " " TOP_REG "\n");

    return;
}

void elf_pop_parameters (SElfBack* Back)
{
    PUT (push);
    fprintf (Back->file, " " SHIFT_REG "\n");

    // PUTLN (m_dup);
    // PUTLN (out);

    PUT (pop);
    fprintf (Back->file, " " COUNT_REG "\n\n");

    for (int i = 0; i < Back->Funcs->Table[Back->Funcs->top_index].parameters ; i++)
    {
        PUT (push);
        fprintf (Back->file, " " COUNT_REG "\n");

        PUT (push);
        fprintf (Back->file, " 1\n");

        PUTLN (add);

        PUT (pop);
        fprintf (Back->file, " " COUNT_REG "\n");

        PUT (pop);
        fprintf (Back->file, " [" COUNT_REG "]\n\n");
    }

//     PUT (push);
//     fprintf (Back->file, " " COUNT_REG "\n");
//
//     PUT (pop);
//     fprintf (Back->file, " " SHIFT_REG "\n");

    return;
}

void elf_incr_top_reg (SElfBack* Back)
{
    PUT (push);
    fprintf (Back->file, " " TOP_REG "\n");

    PUT (push);
    fprintf (Back->file, " 1\n");

    PUTLN (add);

    PUT (pop);
    fprintf (Back->file, " " TOP_REG "\n\n");

    return;
}

void elf_standard_if_jump (SElfBack* Back)
{
    fprintf (Back->file, " " LABEL "%d\n", Back->label_cnt);
    Back->label_cnt++;

    PUT (push);
    fprintf (Back->file, " %d\n", FALSE);

    PUT (jump);
    fprintf (Back->file, " " LABEL "%d\n", Back->label_cnt);

    fprintf (Back->file, LABEL "%d:\n", Back->label_cnt - 1);

    PUT (push);
    fprintf (Back->file, " %d\n", TRUE);

    fprintf (Back->file, LABEL "%d:\n", Back->label_cnt);

    Back->label_cnt++;

    return;
}

// SNode* find_main (SNode* Node)
// {
//     if (NODE_IS_OP_AND__ T_Function)
//     {
//         if (Node->left != NULL &&
//         Node->left->category == CLine && (wcscmp (MAIN_WORD, Node->left->data.var) == 0))
//         {
//             return Node->right->right;
//         }
//     }
//
//     SNode* Main = NULL;
//
//     if (Node->left != NULL)
//     {
//         Main = find_main (Node->left);
//     }
//
//     if ((Node->right != NULL) && (Main == NULL))
//     {
//         Main = find_main (Node->right);
//     }
//
//     return Main;
// }

void elf_write_command (ECommandNums eCommand, FILE* File)
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

void elf_add_to_var_table (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    if (Back->table_cond == none)
    {
        elf_create_new_var_table (Back);
    }

    MLA (CurNode->category == CLine && CurNode->type == TVariable);

    SVarTable* Table = NULL;
    peek_from_stack (Back->VarStack, &Table);

    Table->Arr[Table->size].name  = CurNode->data.var; //copy address from node
    Table->Arr[Table->size].index = Back->RAM_top_index;

    Back->RAM_top_index++;
    Table->size++;

    return;
}

void elf_create_new_var_table (SElfBack* Back)
{
    ME;

    SVarTable* NewTable = (SVarTable*)  calloc (1,                  sizeof (SVarTable));
    NewTable->Arr       = (SVarAccord*) calloc (VAR_TABLE_CAPACITY, sizeof (SVarAccord));
    NewTable->size = 0;

    push_in_stack (Back->VarStack, NewTable);

    Back->table_cond = exist;

    return;
}

void elf_create_param_var_table (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    //ME;

    Back->RAM_top_index = 1;

    do
    {
        if (CurNode->left != NULL)
        {
            elf_add_to_var_table (CurNode->left->left, Back);
            Back->Funcs->Table[Back->Funcs->top_index].parameters++;
        }

        CurNode = CurNode->right;
    } while (CurNode != NULL);

    return;
}

void elf_delete_var_table (SElfBack* Back)
{
    ME;

    SVarTable* Table = NULL;

    pop_from_stack (Back->VarStack, &Table);

    free (Table->Arr);

    free (Table);

    return;
}

int elf_find_var (ELF_BACK_FUNC_HEAD_PARAMETERS)
{
    int RetIndex = JUNK;

    MLA (Back->VarStack->size != 0);
    MLA (CurNode->category == CLine && CurNode->type == TVariable);

    SVarTable* Table = NULL;

    int depth = 1;
    do
    {
        Table = Back->VarStack->data[Back->VarStack->size - depth];
        //printf ("=%p=%d/%d=\n", Table, depth, Back->VarStack->size);
        depth++;
    } while ((find_in_table (CurNode->data.var, Table, &RetIndex) == false) && (depth <= Back->VarStack->size));

    if (RetIndex < 0)
    {
        printf ("==ERROR==\n""No '%ls' found!\n", CurNode->data.var);
        MLA (0);
    }

    return RetIndex;
}

// bool find_in_table (CharT* varName, SVarTable* Table, int* RetIndex)
// {
//     MLA (Table != NULL);
//
//     for (int counter = 0; counter < Table->size; ++counter)
//     {
//         if (wcscmp (varName, Table->Arr[counter].name) == 0)
//         {
//             *RetIndex = Table->Arr[counter].index;
//
//             return true;
//         }
//     }
//
//     return false;
// }

// void free_tables (SStack<SVarTable*>* VarStack)
// {
//     for (int i = 0; VarStack->size - i > 0; i++)
//     {
//         free (VarStack->data[i]->Arr);
//         VarStack->data[i]->Arr = NULL;
//
//         free (VarStack->data[i]);
//         VarStack->data[i] = NULL;
//     }
//
//     stack_destructor (VarStack);
//
//     return;
// }

//=============================================================================================================================================================================

void x86_push_imm (ELF_BACK_FUNC_HEAD_PARAMETERS, char Number)
{
    SET(0x6a);
    SET(Number);
}

void x86_push_imm (ELF_BACK_FUNC_HEAD_PARAMETERS, short Number)
{
    SET(0x68);
    SET_2(Number);
}

void x86_push_imm (ELF_BACK_FUNC_HEAD_PARAMETERS, int Number)
{
    SET(0x68);
    SET_4(Number);
}

void x86_push_r (ELF_BACK_FUNC_HEAD_PARAMETERS, int Register)
{
    char opcode = 0x50;
    opcode += (char) Register;

    SET(opcode);
}

void x86_pop_r (ELF_BACK_FUNC_HEAD_PARAMETERS, int Register)
{
    char opcode = 0x58;
    opcode += (char) Register;

    SET(opcode);
}

//=============================================================================================================================================================================


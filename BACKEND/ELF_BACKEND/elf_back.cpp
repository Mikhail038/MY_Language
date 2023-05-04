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

#define FILL_2  SET (0x00);

#define FILL_4  SET (0x00); SET (0x00); SET (0x00);

#define FILL_8  SET (0x00); SET (0x00); SET (0x00); SET (0x00); SET (0x00); SET (0x00); SET (0x00);



//=============================================================================================================================================================================

void make_elf_file (SNode* Root, FILE* ExFile)
{
    MLA (ExFile != NULL);

    SBack* Back = back_constructor (ExFile, MAX_ELF_SIZE);

    //generate_user_functions (Root, Back);

    generate_elf_array (Root, Back);

    fwrite (Back->Array, sizeof (char), Back->cnt, Back->file);

    back_destructor(Back);

    // for (int i = 0; i < Back->Funcs->top_index; i++)
    // {
    //     printf ("'%ls'[%d]\n", Back->Funcs->Table[i]);
    // }

    return;
}

//=============================================================================================================================================================================
//Make ELF//
//=============================================================================================================================================================================

void generate_elf_array (SNode* Root, SBack* Back)
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

    size_t EntryVA  = 0;
    size_t addr_EntryVA = Back->cnt;
    Back->cnt += 8; //we ll skip it now

    size_t ProgramHeadersStart = 0;
    size_t addr_ProgramHeadersStart = Back->cnt;
    Back->cnt += 8; //we ll skip it now

    size_t SectionHeadersStart = 0;
    size_t addr_SectionHeadersStart = Back->cnt;
    Back->cnt += 8; //we ll skip it now

    SET (0x00); //Flags (no flags = 0)
    FILL_4;     //nothing

    unsigned short int  HeaderSize = 64; //std
    memcpy ((unsigned short int*) &(Back->Array[Back->cnt]),
                        &HeaderSize, sizeof (unsigned short int));
    Back->cnt +=2;

    unsigned short int  ProgramHeaderSize = 56; //std
    memcpy ((unsigned short int*) &(Back->Array[Back->cnt]),
                        &ProgramHeaderSize, sizeof (unsigned short int));
    Back->cnt +=2;

    unsigned short int  ProgramHeadersCnt = 1;
    memcpy ((unsigned short int*) &(Back->Array[Back->cnt]),
                        &ProgramHeadersCnt, sizeof (unsigned short int));
    Back->cnt +=2;

    unsigned short int  SectionHeadersSize = 64; //std
    memcpy ((unsigned short int*) &(Back->Array[Back->cnt]),
                        &SectionHeadersSize, sizeof (unsigned short int));
    Back->cnt +=2;

    unsigned short int  SectionHeadersCnt = 3; //TODO maybe not
    memcpy ((unsigned short int*) &(Back->Array[Back->cnt]),
                        &SectionHeadersCnt, sizeof (unsigned short int));
    Back->cnt +=2;

    unsigned short int  ShstrTabIndex  = 2; //TODO maybe not
    memcpy ((unsigned short int*) &(Back->Array[Back->cnt]),
                        &ShstrTabIndex, sizeof (unsigned short int));
    Back->cnt +=2;

    //=======================================================================

    ProgramHeadersStart = Back->cnt;
    memcpy ((size_t*) &(Back->Array[addr_ProgramHeadersStart]),
    &ProgramHeadersStart, sizeof (size_t));

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

    size_t SegmentSize = 0;
    size_t addr_SegmentSize = Back->cnt;
    Back->cnt += 8; //we ll skip it now

    size_t addr_SegmentFileSize = Back->cnt;
    Back->cnt += 8; //we ll skip it now

    SET (0x00);
    SET (0x00);
    SET (0x20); //Alignment
    SET (0x00);
    SET (0x00);
    SET (0x00);
    SET (0x00);
    SET (0x00);

    //=======================================================================

    SectionHeadersStart = Back->cnt;
    memcpy ((size_t*) &(Back->Array[addr_SectionHeadersStart]),
    &SectionHeadersStart, sizeof (size_t));

    for (int cnt_null_header = 0; cnt_null_header < 64; cnt_null_header++)
    {
        SET (0x00);
    }

    //=======================================================================
    //text

    unsigned int TextNameOffset = 0;
    size_t addr_TextNameOffset = Back->cnt;
    Back->cnt += 4; //we ll skip it now

    SET (0x01); //Section type (bits = 1)
    FILL_4;     //nothing

    SET (0x06); //Section flags (r w x )
    FILL_8;     //nothing

    size_t addr_another_EntryVA = Back->cnt;
    //memcpy ((size_t*) &(Back->Array[Back->cnt]),
    //&EntryVA, sizeof (size_t));  //Virtual address
    Back->cnt += 8; //skip

    size_t TextOffset = 0;
    size_t addr_TextOffset = Back->cnt;
    //SET (0x00); //Offset
    //FILL_8;
    Back->cnt += 8; //we ll skip it now


    size_t TextSize = 0;
    size_t addr_TextSize = Back->cnt;
    Back->cnt += 8; //we ll skip it now

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

    unsigned int TableNameOffset = 0;
    size_t addr_TableNameOffset = Back->cnt;
    Back->cnt += 4; //we ll skip it now

    SET (0x03); //Section type (strtable = 1)
    FILL_4;     //nothing

    SET (0x00);
    FILL_8;

    size_t TableLoadAddress = 0;
    size_t addr_TableLoadAddress = Back->cnt;
    Back->cnt += 8; //we ll skip it now

    size_t TableAddress = 0;
    size_t addr_TableAddress = Back->cnt;
    Back->cnt += 8; //we ll skip it now

    size_t TableSize = 0;
    size_t addr_TableSize = Back->cnt;
    Back->cnt += 8; //we ll skip it now

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

    TextOffset = Back->cnt;
    memcpy ((size_t*) &(Back->Array[addr_TextOffset]),
    &TextOffset, sizeof (size_t));

    elf_generate_code (Root, Back);


    TextSize = Back->cnt - TextOffset;
    memcpy ((size_t*) &(Back->Array[addr_TextSize]),
    &TextSize, sizeof (size_t));

    //=======================================================================

    SegmentSize = Back->cnt;
    memcpy ((size_t*) &(Back->Array[addr_SegmentSize]),
    &SegmentSize, sizeof (size_t));

    memcpy ((size_t*) &(Back->Array[addr_SegmentFileSize]),
    &SegmentSize, sizeof (size_t));

    TableAddress = Back->cnt;
    memcpy ((size_t*) &(Back->Array[addr_TableAddress]),
    &TableAddress, sizeof (size_t));

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

    TableSize = (size_t) (Back->cnt - SegmentSize);
    memcpy ((size_t*) &(Back->Array[addr_TableSize]),
    &TableSize, sizeof (size_t));

    return;
}

//=============================================================================================================================================================================

void elf_generate_code (BACK_FUNC_HEAD_PARAMETERS)
{
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

void elf_generate_statement (BACK_FUNC_HEAD_PARAMETERS)
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

void elf_generate_function (BACK_FUNC_HEAD_PARAMETERS)
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
    create_param_var_table (CurNode->left->right, Back);

    elf_pop_parameters (Back);

    Back->Funcs->top_index++;


    elf_generate_statement (BACK_RIGHT_SON_FUNC_PARAMETERS);
    CLEAN_TABLE;

    fprintf (Back->file, "\n\n" SEP_LINE "\n\n\n");

    return;
}

void elf_generate_node (BACK_FUNC_HEAD_PARAMETERS)
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

void elf_generate_op_node (BACK_FUNC_HEAD_PARAMETERS)
{
    switch (CurNode->type)
    {
        #define DEF_OP(d_type, d_condition, d_tokenize, d_print, d_switch) \
        case d_type: \
            d_switch; \
            break;

        #include "Operations.h"

        #undef DEF_OP
    }

    return;
}

void elf_generate_announce (BACK_FUNC_HEAD_PARAMETERS)
{
    elf_generate_expression (BACK_RIGHT_SON_FUNC_PARAMETERS);

    add_to_var_table (BACK_LEFT_SON_FUNC_PARAMETERS);

    elf_generate_pop_var (BACK_LEFT_SON_FUNC_PARAMETERS);

    elf_incr_top_reg (Back);
//     write_command (pop, Back->file);
//
//     fprintf (Back->file, " [%d]\n\n", Back->RAM_top_index - 1);

    return;
}

void elf_generate_equation (BACK_FUNC_HEAD_PARAMETERS)
{
    elf_generate_expression (BACK_RIGHT_SON_FUNC_PARAMETERS);

    elf_generate_pop_var (BACK_LEFT_SON_FUNC_PARAMETERS);

    return;
}

void elf_generate_input (BACK_FUNC_HEAD_PARAMETERS)
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

void elf_generate_output (BACK_FUNC_HEAD_PARAMETERS)
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

void elf_generate_if (BACK_FUNC_HEAD_PARAMETERS)
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
    CLEAN_TABLE;

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
            CLEAN_TABLE;
        }
    }
    fprintf (Back->file,  LABEL "%d:\n", Label_2);


    return;
}

void elf_generate_while (BACK_FUNC_HEAD_PARAMETERS)
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
    CLEAN_TABLE;

    PUT (jump);
    fprintf (Back->file, " " LABEL "%d\n", Label_1);

    fprintf (Back->file,  LABEL "%d:\n", Label_2);

    return;
}

void elf_generate_call (BACK_FUNC_HEAD_PARAMETERS)
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

void elf_generate_return (BACK_FUNC_HEAD_PARAMETERS)
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

void elf_generate_expression (BACK_FUNC_HEAD_PARAMETERS)
{
    elf_generate_postorder (BACK_FUNC_PARAMETERS);

    return;
}

void elf_generate_postorder (BACK_FUNC_HEAD_PARAMETERS)
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

void elf_generate_pop_var (BACK_FUNC_HEAD_PARAMETERS)
{
    int Index = find_var (BACK_FUNC_PARAMETERS);

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

void elf_generate_push_var (BACK_FUNC_HEAD_PARAMETERS)
{
    int Index = find_var (BACK_FUNC_PARAMETERS);

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

void elf_push_parameters (BACK_FUNC_HEAD_PARAMETERS)
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

void elf_pop_parameters (SBack* Back)
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

void elf_incr_top_reg (SBack* Back)
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

void elf_standard_if_jump (SBack* Back)
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

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

//==================================================================================================================================================================

#define ELF_BACK_FUNC_HEAD_PARAMETERS SNode* CurNode, SElfBack* Back

#define ELF_CLEAN_VAR_TABLE() if (Back->table_condition != table_none) { elf_delete_var_table (Back); Back->table_condition = table_exist; }

//=============================================================================================================================================================================

void users_tree (LinePlusElfBack Argument)
{
    Argument.back->ast_file_name = (char*) Argument.Line;

    //TODO thing like that are usually made with defines like:
    // #ifdef DEBUG
    // #define DEBUG_PRINT(...) do {printf(VA_ARGS);} while (0);
    // #else
    // #define DEBUG_PRINT(...)
    // #endif
    //
    //usage:
    //
    // DEBUG_PRINT(KMGN "|ast choosed| [%s]\n" KNRM, Argument.Line);
    //

    #ifdef DEBUG
    printf(KMGN "|ast choosed| [%s]\n" KNRM, Argument.Line);
    #endif
}

void users_executable (LinePlusElfBack Argument)
{
    Argument.back->ex_file_name = (char*) Argument.Line;

    #ifdef DEBUG
    printf(KMGN "|exec choosed| [%s]\n" KNRM, Argument.Line);
    #endif
}

void users_graph_viz (LinePlusElfBack Argument)
{
    Argument.back->gv_file_name = (char*) Argument.Line;

    #ifdef DEBUG
    printf(KMGN "|graph viz choosed| [%s]\n" KNRM, Argument.Line);
    #endif
}

void users_help (LinePlusElfBack Argument)
{
    //TODO combine several printf calls to one
    printf (KGRN Kbright "-t" KNRM " + " KRED "_FILENAME_    " KNRM Kunderscore "choose custom AST tree\n" KNRM);
    printf (KGRN Kbright "-e" KNRM " + " KRED "_FILENAME_    " KNRM Kunderscore "create custom execuatable file\n" KNRM);
    printf (KGRN Kbright "-g" KNRM " + " KRED "_FILENAME_    " KNRM Kunderscore "put graph_viz dump in custom file\n" KNRM);
    printf (KGRN Kbright "-h" KNRM "   "      "              " KNRM Kunderscore "show to user this information\n" KNRM);

    Argument.back->exit_marker = true;

    #ifdef DEBUG
    printf(KMGN "|helped|\n" KNRM);
    #endif
}

void construct_elf_back (int argc, char** argv, ElfBack* Back, const ElfHead* ElfHeader)
{
    //default values
    Back->exit_marker   = false;
    Back->ast_file_name = "AST/ParsedSrc.tr";
    Back->ex_file_name = "a.elf";
    Back->gv_file_name = "LOGS/BACKEND/GraphVizASTTree"; // TODO Well, calling flag-pasrser is not "construct_elf_back" job. 
                                                         //      It should get flags as parameters
    //they may choose after flags parsing

    const ElfBackFlagFunction FlagsArray[] =
    {
        {'t', users_tree},
        {'e', users_executable},
        {'g', users_graph_viz},
        {'h', users_help},
    };
    size_t FlagsAmount = sizeof (FlagsArray);
    parse_elf_back_flags(Back, argc, argv, (ElfBackFlagFunction*) &FlagsArray, FlagsAmount);

    Back->inp_func_file_name = "BACKEND/ELF_BACKEND/input_function";
    Back->out_func_file_name = "BACKEND/ELF_BACKEND/output_function";

/*
    TODO cat is sad of you not aligning "calloc"s 

            ／＞　  フ
            | 　_　_| 
          ／` ミ＿xノ 
         /　　　　 |
        /　 ヽ　　 ﾉ
        │　　|　|　|
    ／￣|　　 |　|　|
    (￣ヽ＿_ヽ_)__)
    ＼二)
*/
    Back->Funcs         = (BackFuncTable*)     calloc (1, sizeof (BackFuncTable));
    Back->Funcs->Table  = (SBackFunc*)          calloc (MAX_FUNCS_ARRAY, sizeof (SBackFunc));
    Back->Funcs->top_index = 0;

    Back->VarStack      = (SStack<VarTable*>*) calloc (1, sizeof (SStack<VarTable*>));

    Back->head = ElfHeader;

    Back->table_condition = table_none;

    stack_constructor (Back->VarStack);

    Back->patches = (MyArray*) calloc(1, sizeof (MyArray));
    array_constructor(Back->patches, sizeof (Patch*));

    Back->ByteCodeArray = (char*) calloc(sizeof (char), MAX_ELF_SIZE);
    Back->cur_addr = 0;

    //TODO I like it, it has became much better
    /*
                            ⢀⣀⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣤⡄⠀⠀⠀⠀⢀⡴⠚⠉⠀⠀⠀⠈⠓⢤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣏⣿⠟⡄⠀⠀⣠⡟⣡⣄⡀⠀⠀⠀⠀⠀⠀⠑⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠙⠛⢾⣦⣴⢋⡞⠋⠙⠻⣦⡀⠀⠀⠀⠀⢀⣼⡀⠀⠀⢠⣾⣿⡀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⡿⢼⣧⣾⣽⡿⣿⣿⡤⣤⣴⣶⣿⠟⠻⠤⣶⡿⠿⠿⠃⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡟⠀⠀⠈⠛⠛⠒⡟⠁⠀⠀⠻⣿⣿⣱⢠⡞⠁⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠃⢠⣶⣤⣤⣾⣇⣶⣀⠀⠀⠀⠀⣍⠁⠀⠘⡄⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⡾⡖⢺⠻⣧⣀⠀⠉⠛⠛⠿⠶⠚⠛⠙⢷⣤⡀⡇⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠴⢶⣤⣤⣾⣿⡇⡅⠈⣧⣿⣎⢙⡒⠢⠤⢄⣀⣀⣀⣠⠤⢹⠀⣷⡄⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⡠⠖⠀⠀⠀⠀⠻⣿⣿⣿⣷⡙⠶⣧⠀⠈⢧⠀⠁⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⣦⣤⣤⣤⡀
⠀⠀⠀⠀⢀⡾⠁⠀⠀⠀⠀⠀⠀⢹⣿⣿⣿⣿⣦⣿⠀⠀⢸⣄⣀⠄⠀⠉⠠⣄⣰⠀⢀⣼⣿⣿⣿⠟⠃⠀⠉
⠀⠀⠀⠀⡼⠁⠀⠀⠀⠀⠀⠀⠀⠀⢹⣿⣿⣿⡿⠁⠀⠀⠈⠀⠀⠀⠀⠀⣤⠞⠋⣠⣿⣿⣿⣿⠋⠀⠀⠀⠀
⠀⠀⠀⢀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀
⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⣿⠟⢋⡅⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢺⣿⣿⣿⣿⠃⠀⠀⠀⠀⠀
⠀⠀⠀⡞⠀⠀⠀⠀⡠⠄⣀⠴⠛⠉⠁⠀⠀⢨⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣽⣿⣾⣿⣷⡄⠀⠀⠀⠀
⠀⠀⢸⠁⠀⡠⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣿⣿⣿⣿⣿⠟⠀⠀⠀⠀
⠀⢀⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠠⠄⠈⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣴⠿⠿⠿⣿⣷⠤⠶⠦⣄⡀
⢠⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⠄⠂⢹⡀⠀⠀⠀⠀⢀⣀⣠⠾⠟⠋⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉
⠸⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢳⣤⡴⠾⠛⠋⢹⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⢻⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠓⠚⢸⣿⣦⡙⢣⠀⠀⠀⢸⡧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠈⠛⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠛⠛⠛⠛⠓⠒⠒⠒⠚⠛⠒⠒⠂⠐⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀
    */

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

void make_elf_file (ElfBack* Back)
{
    AstNode* Root = read_tree (Back->ast_file_name);

    // TODO same as DEBUG_PRINT (look at elf_back.cpp:51)
    #ifdef DEBUG
    make_graf_viz_tree (Root, Back->gv_file_name, false);
    #endif

    MY_LOUD_ASSERT(Root != NULL);

    generate_elf_array (Back, Root);

    FILE* ExFile = fopen (Back->ex_file_name, "w");
    MY_LOUD_ASSERT (ExFile != NULL);
    fwrite (Back->ByteCodeArray, sizeof (char), Back->cur_addr, ExFile);
    fclose (ExFile);

    delete_tree (&Root);

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
    ALIGN_8; // here and later it means 7 padding bytes

    set_one_byte(Back, Back->head->elf_type);
    align_one_byte(Back, 2);  //1 padding byte

    set_one_byte(Back, Back->head->machine);
    align_one_byte(Back, 2);  //1 padding byte

    set_one_byte(Back, Back->head->version);
    ALIGN_4;      // here and later it means 3 padding bytes
}

void elf_head_start_params (ElfBack* Back)
{
    create_empty_space_for_patch(Back, SupportedPatches::FileVirtualAddress, sizeof (long int));
    create_empty_space_for_patch(Back, SupportedPatches::ProgramHeadersStart, sizeof (long int));
    create_empty_space_for_patch(Back, SupportedPatches::SectionHeadersStart, sizeof (long int));

    set_one_byte(Back, Back->head->flags);
    ALIGN_4;

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
    UniversalNumber PatchData;

    PatchData.long_data = Back->cur_addr;
    paste_patch(Back, SegmentFileSize, PatchData);

    PatchData.long_data = Back->cur_addr;
    paste_patch(Back, TableAddress, PatchData);
    size_t TableAddressSaved = PatchData.long_data;

    PatchData.long_data = TableAddressSaved + Back->head->file_virtual_address;
    paste_patch(Back, TableLoadAddress, PatchData);

    set_one_byte(Back, 0x00); //begin section header string table

    PatchData.int_data = Back->cur_addr - SegmentSizeSaved;
    paste_patch(Back, TextNameOffset, PatchData);


    set_bytes(Back, (void*) &(Back->head->shstrtable_text_name), sizeof (Back->head->shstrtable_text_name));

    PatchData.int_data = Back->cur_addr - SegmentSizeSaved;
    paste_patch(Back, TableNameOffset, PatchData);

    set_bytes(Back, (void*) &(Back->head->shstrtable_table_name), sizeof (Back->head->shstrtable_table_name));
}

void generate_elf_array (ElfBack* Back, AstNode* Root)
{
    UniversalNumber PatchData; // TODO Struct's like that are usually called "NumericType" or like that, 'coz it's not an universal number
                               //       It doesn't support fractions, complex, quaternions and other numbers))

    elf_head_start (Back);

    elf_head_start_params (Back); // TODO please use verbs in function names. for ex.: elf_reserve_space_for_head_start_params or else

    //=======================================================================

    PatchData.long_data = Back->cur_addr;

    paste_patch(Back, SupportedPatches::ProgramHeadersStart, PatchData);

    elf_head_program_header_params (Back); // TODO same, use verbs

    create_empty_space_for_patch(Back, SupportedPatches::SegmentSize, sizeof (long int)); // TODO this name is cool!

    create_empty_space_for_patch(Back, SupportedPatches::SegmentFileSize, sizeof (long int));

    set_bytes(Back, (void*) &Back->head->alignment, sizeof (Back->head->alignment));

    //=======================================================================

    PatchData.long_data = Back->cur_addr;
    paste_patch(Back, SupportedPatches::SectionHeadersStart, PatchData);

    //=======================================================================
    //null

    for (int cnt_null_header = 0; cnt_null_header < 64; cnt_null_header++) // TODO make 64 a named constant for better understanding 
    {
        set_one_byte(Back, 0x00);
    }

    //=======================================================================
    //text

    create_empty_space_for_patch(Back, SupportedPatches::TextNameOffset, sizeof (int));

    set_one_byte(Back, Back->head->text_section_type);
    ALIGN_4;
    set_one_byte(Back, Back->head->text_section_flags);
    ALIGN_8;

    create_empty_space_for_patch(Back, SupportedPatches::EntryVirtualAddress, sizeof (size_t));

    create_empty_space_for_patch(Back, SupportedPatches::TextOffset, sizeof (size_t));

    create_empty_space_for_patch(Back, SupportedPatches::TextSize, sizeof (size_t));

    set_one_byte(Back, Back->head->text_section_index);
    ALIGN_4;
    set_one_byte(Back, Back->head->text_section_extra_info);
    ALIGN_4;
    set_one_byte(Back, Back->head->text_section_align);
    ALIGN_8;
    set_one_byte(Back, Back->head->text_section_extra_sizes);
    ALIGN_8;

    //=======================================================================
    //strtable

    create_empty_space_for_patch(Back, SupportedPatches::TableNameOffset, sizeof (int));

    set_one_byte(Back, Back->head->shstrtable_section_type);
    ALIGN_4;
    set_one_byte(Back, Back->head->shstrtable_section_flags);
    ALIGN_8;

    create_empty_space_for_patch(Back, SupportedPatches::TableLoadAddress, sizeof (size_t));

    create_empty_space_for_patch(Back, SupportedPatches::TableAddress, sizeof (size_t));

    create_empty_space_for_patch(Back, SupportedPatches::TableSize, sizeof (size_t));

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

    PatchData.long_data = Back->cur_addr + Back->head->file_virtual_address;
    paste_patch(Back, FileVirtualAddress, PatchData);

    paste_patch(Back, EntryVirtualAddress, PatchData);

    //=======================================================================
    //.code

    PatchData.long_data = Back->cur_addr;
    paste_patch(Back, TextOffset, PatchData);
    size_t TextOffset_saved = PatchData.long_data;

    //----------------------------------------------------------------------

    elf_generate_code (Back, Root);

    //----------------------------------------------------------------------

    PatchData.long_data = Back->cur_addr - TextOffset_saved;
    paste_patch(Back, TextSize, PatchData);

    //=======================================================================
    //shstrtable

    PatchData.long_data = Back->cur_addr;
    paste_patch(Back, SegmentSize, PatchData);
    size_t SegmentSizeSaved = PatchData.long_data;

    elf_head_shstrtable (Back, SegmentSizeSaved);

    //=======================================================================

    PatchData.long_data = Back->cur_addr - SegmentSizeSaved;
    paste_patch(Back, TableSize, PatchData);

    return;
}       // TODO function is quite big, but if you can't find the way to split it, that's ok

#undef ALIGN_4
#undef ALIGN_8

//=============================================================================================================================================================================

const int VAR_SIZE_BYTES = 8; // TODO define constants in the upper part of file, also either add "static" keyword or define it in header 

void elf_generate_code (ElfBack* Back, AstNode* Root)
{
    x86_call_label(Back, MAIN_LBL); 
/*      (no verb)
           /\
            \
             \----------\
                         |
// TODO He wants to see verbs in function names
        |
        |
        |   ░░░░░░░░░░▄▄█▀▀▀▀▀▀▀▀█▄▄░░░░░░░░
        |   ░░░░░░░▄▄▀▀░░░░░░░░░░░░▀▀▄▄░░░░░
        |   ░░░░░▄█▀░░░░▄▀░░░░▄░░░░░░░▀█░░░░
        |   ░░░░██▄▄████░░░░░░▀▄░░░░░░░░█▄░░
        |   ░░▄████▀███▄▀▄░░░░░░███▄▄▄▄░░█░░
        |   ░▄█████▄████░██░░░▄███▄▄░▀█▀░░█░
        |   ▄███████▀▀░█░▄█░▄███▀█████░█░░▀▄
        |   █░█▀██▄▄▄▄█▀░█▀█▀██████▀░██▀█░░█
        \-> █░█░▀▀▀▀▀░░░█▀░█░███▀▀░░▄█▀░█░░█
            █░░█▄░░░░▄▄▀░░░█░▀██▄▄▄██▀░░█▄░█
            █░░░░▀█▀▀▀░░░░░░█░░▀▀▀▀░░░░▄█░░█
            █░░░░░░░░░░░░░░░░▀▄░░░░░░▄█▀░░░█
            ░█░░░░░░░░░░░░░░░░▀▀▀▀▀▀▀▄░░░░█░
            ░░█░░░░░░▄▄▄▄▄▄▄░░░░░░░░░░░░░▄▀░
            ░░░▀▄░░░░░▀█▄░░░▀▀██▄░░░░░░░▄▀░░
            ░░░░░▀▄▄░░░░░▀▀▀▀▀░░░░░░░░▄▀░░░░
            ░░░░░░░░▀▀▄▄▄░░░░░░░░▄▄▄▀▀█░░░░░
            ░░░░░░░░░░▄▀▀█████▀▀▀▀░░░░██░░░░
            ░░░░░░░░░█░░░██░░░█▀▀▀▀▀▀▀▀█░░░░
*/
    x86_exit(Back);

    FILE* InpFile = fopen(Back->inp_func_file_name, "r");
    MY_LOUD_ASSERT(InpFile != NULL);

    x86_paste_call_label(Back, INP_LBL);
    Back->cur_addr += paste_functions_bytes_from_file(&(Back->ByteCodeArray[Back->cur_addr]), InpFile);

    fclose(InpFile);

    FILE* OutFile = fopen(Back->out_func_file_name, "r");
    MY_LOUD_ASSERT(OutFile != NULL);

    x86_paste_call_label(Back, OUT_LBL);
    Back->cur_addr += paste_functions_bytes_from_file(&(Back->ByteCodeArray[Back->cur_addr]), OutFile);

    fclose(OutFile);

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

    x86_paste_call_label(Back, CurNode->left->data.var);

    #ifdef DEBUG
    fprintf (stdout, LABEL "|%ls|:\n", CurNode->left->data.var);
    #endif

    Back->Funcs->Table[Back->Funcs->top_index].Name = CurNode->left->data.var;

    Back->table_condition = table_none;
    elf_create_param_var_table (Back, CurNode->left->right);

    size_t AmountVars = 0;

    VarTable* Table = NULL;
    if (Back->VarStack->size != 0)
    {

        pop_from_stack (Back->VarStack, &Table);

        AmountVars = elf_find_new_vars(Back, CurNode->right);

        Table->amount = AmountVars;

        push_in_stack(Back->VarStack, Table);

        #ifdef DEBUG
        printf ("Vars Amount: [%lu]\n", AmountVars);
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
    ELF_CLEAN_VAR_TABLE ();

    Back->delta_rbp = old_delta_rbp;

    x86_add_reg_imm(Back, rbp, AmountVars * VAR_SIZE_BYTES);

    x86_pop_to_reg(Back, rbp);
    x86_mov_reg_reg(Back, rsp, rbp);

    return;
}

void elf_generate_node (ElfBack* Back, AstNode* CurNode)
{
    if (CurNode->category == OperationNode)
    {
        elf_generate_op_node (Back, CurNode, true);
    }

    if (CurNode->category == ValueNode)
    {
        x86_push_imm(Back, CurNode->data.val);
    }

    if (CurNode->category == NameNode && CurNode->type == TypeVariable)
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

void elf_generate_call_input_function (ElfBack* Back, AstNode* CurNode)
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

void elf_generate_call_output_function (ElfBack* Back, AstNode* CurNode)
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

    Back->table_condition = table_none;
    elf_generate_statement (Back, CurNode->left);
    ELF_CLEAN_VAR_TABLE ();

    wchar_t* Label_false_name = (wchar_t*) calloc(MAX_JUMP_LABEL_SIZE, sizeof (wchar_t));
    prepare_name_label_to_jump (Label_false_name, Back->cur_addr);

    x86_jump_label(Back, Label_false_name, x86_jmp);

    x86_paste_jump_label(Back, Label_true_name);

    CurNode = CurNode->right;

    if (CurNode != NULL)
    {
        if (CurNode->category == OperationNode && CurNode->type == TypeIf)
        {
            elf_generate_if (Back, CurNode);
        }
        else
        {
            Back->table_condition = table_none;
            elf_generate_statement (Back, CurNode);
            ELF_CLEAN_VAR_TABLE ();
        }
    }

    x86_paste_jump_label(Back, Label_false_name);

    free (Label_true_name);
    free (Label_false_name);

    return;
}

void elf_generate_while (ElfBack* Back, AstNode* CurNode)
{
    wchar_t BodyLabelName[MAX_JUMP_LABEL_SIZE];
    prepare_name_label_to_jump (BodyLabelName, Back->cur_addr);

    x86_paste_jump_label(Back, BodyLabelName);

    elf_generate_expression (Back, CurNode->left);

    x86_push_imm(Back, MY_FALSE);

    wchar_t EndLabelName[MAX_JUMP_LABEL_SIZE];
    prepare_name_label_to_jump (EndLabelName, Back->cur_addr);

    x86_cmp_stack(Back);
    x86_jump_label(Back, EndLabelName, x86_je);


    Back->table_condition = table_none;
    elf_generate_statement (Back, CurNode->right);
    ELF_CLEAN_VAR_TABLE ();

    x86_jump_label(Back, BodyLabelName, x86_jmp);

    x86_paste_jump_label(Back, EndLabelName);

    return;
}

void elf_generate_call (ElfBack* Back, AstNode* CurNode, bool RetValueMarker)
{
    elf_push_function_parameters (Back, CurNode->right);

    x86_call_label(Back, CurNode->left->data.var);

    elf_pop_function_parameters (Back, CurNode->right);

    if (RetValueMarker == true) // TODO usually bool vars are not compared explicitly, for ex:  
    {                           //      if (RetValueMarker) ...
                                //      But if you want to, that's ok
        x86_push_reg(Back, ELF_FUNC_RET_REG);
    }

    return;
}

void elf_generate_return (ElfBack* Back, AstNode* CurNode)
{
    elf_generate_expression (Back, CurNode->left);

    x86_pop_to_reg(Back, ELF_FUNC_RET_REG);

    size_t AmountVars = 0;

    VarTable* Table = NULL;

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
    if (CurNode->category == OperationNode && CurNode->type == TypeCall)
    {
        elf_generate_node (Back, CurNode);

        return;
    }

    if (CurNode->left != NULL)
    {
        elf_generate_postorder (Back, CurNode->left, RetValueMarker);
    }

    if (CurNode->right != NULL)
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

    x86_pop_to_reg(Back, FIRST_ARIPHMETIC_REG);

    x86_mov_to_addr_in_reg_from_reg(Back, rax, FIRST_ARIPHMETIC_REG);

    return;
}

void elf_generate_push_var (ElfBack* Back, AstNode* CurNode)
{
    elf_set_rax_var_value(Back, CurNode);

    x86_push_reg(Back, rax);

    return;
}

//=============================================================================================================================================================================

void elf_push_function_parameters (ElfBack* Back, AstNode* CurNode)
{
    if (CurNode->right != NULL)
    {
        elf_push_function_parameters (Back, CurNode->right);
    }

    if (CurNode != NULL && CurNode->left != NULL)
    {
        elf_generate_expression (Back, CurNode->left);
    }

    return;
}

void elf_pop_function_parameters (ElfBack* Back, AstNode* CurNode)
{
    if (CurNode->right != NULL)
    {
        elf_pop_function_parameters (Back, CurNode->right);
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

//=============================================================================================================================================================================

void  elf_add_to_var_table (ElfBack* Back, AstNode* CurNode, bool ParamMarker)
{
    if (Back->table_condition == VarTableConditions::table_none)
    {
        elf_create_new_var_table (Back, ParamMarker);
    }

    MY_LOUD_ASSERT (CurNode->category == NameNode && CurNode->type == TypeVariable);

    VarTable* Table = NULL;
    peek_from_stack (Back->VarStack, &Table);

    Table->Arr[Table->cur_size].name  = CurNode->data.var; //copy address from node

    if (ParamMarker == false)
    {
        elf_param_var_set_index (Table);
    }
    else
    {
        elf_var_set_index (Table);
    }

    Table->cur_size++;

    return;
}

void elf_param_var_set_index (VarTable* Table)
{
    #ifdef DEBUG
    printf (KYLW "Amount: %lu CurSize: %d Param: %lu Final: %lu => " KNRM, Table->amount, Table->cur_size, Table->amount_param, Table->amount - (Table->cur_size - Table->amount_param) - 1);
    #endif

    Table->Arr[Table->cur_size].index = (Table->amount - (Table->cur_size - Table->amount_param) - 1) * VAR_SIZE_BYTES;

    #ifdef DEBUG
    printf (KYLW "index <%d>\n" KNRM, Table->Arr[Table->cur_size].index);
    #endif
}

void elf_var_set_index (VarTable* Table)
{
    Table->Arr[Table->cur_size].index = - (Table->cur_size + 2); // another way to mark params of function

    #ifdef DEBUG
    printf (KYLW "index |%d|\n" KNRM, Table->Arr[Table->cur_size].index);
    #endif

    Table->amount_param++;
}
void elf_create_new_var_table (ElfBack* Back, bool ParamMarker)
{
    PRINT_DEBUG_INFO;

    Back->delta_rbp = 0;
    // TODO UnAlIgNeD!11!!11!!!! !!11!
    // ))
    VarTable* NewTable = (VarTable*)  calloc (1,                  sizeof (VarTable));
    NewTable->Arr       = (SVarAccord*) calloc (VAR_TABLE_CAPACITY, sizeof (SVarAccord));
    NewTable->cur_size = 0;

    NewTable->param_marker = ParamMarker;

    #ifdef DEBUG_VARS
    if (ParamMarker == true)
    {
        printf("func params\n");
        NewTable->amount = elf_find_new_vars (Node);
        MY_LOUD_ASSERT (CurNode->type != TypeParameter);
        printf("amount %d\n", NewTable->amount);
    }
    else
    {
        NewTable->amount = elf_find_new_vars (CurNode);
        printf("amount %d\n", NewTable->amount);
    }
    #endif

    push_in_stack (Back->VarStack, NewTable);

    Back->table_condition = table_exist;

    return;
}

size_t elf_find_new_vars (ElfBack* Back, AstNode* CurNode)
{
    size_t OneMoreVar = 0;

    if (CurNode->left != NULL && CurNode->left->type == TypeAnnounce)
    {
        OneMoreVar = 1;
    }

    if (CurNode->right == NULL)
    {
        return OneMoreVar;
    }

    return OneMoreVar + elf_find_new_vars(Back, CurNode->right);
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
        #ifdef DEBUG_2
        printf (KRED "forced to create vat_table\n" KNRM);
        #endif

        elf_create_new_var_table(Back, true);
    }

    return;
}

void elf_delete_var_table (ElfBack* Back)
{
    PRINT_DEBUG_INFO;

    VarTable* Table = NULL;

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
        if (CurNode->right != NULL && CurNode->category == OperationNode && CurNode->type == TypeStatement)
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
    MY_LOUD_ASSERT (CurNode->category == NameNode && CurNode->type == TypeVariable);

    VarTable* Table = NULL;

    int Depth = 1;
    do
    {
        Table = Back->VarStack->data[Back->VarStack->size - Depth];

        #ifdef DEBUG_2
        printf ("=%p=%d/%d=\n", Table, depth, VarStack->size);
        #endif

        Depth++;
    } while ((elf_find_in_table (CurNode->data.var, Table, &RetIndex, &ParamMarker) == false) && (Depth <= Back->VarStack->size));

    if (RetIndex == WrongValue)
    {
        printf ("==ERROR==\n""No '%ls' found!\n", CurNode->data.var);
        MY_LOUD_ASSERT (0);
    }

    return RetIndex;
}

//=============================================================================================================================================================================

bool elf_find_in_table (CharT* VarName, VarTable* Table, int* RetIndex, bool* ParamMarker)
{
    MY_LOUD_ASSERT (Table != NULL);

    for (int counter = 0; counter < Table->cur_size; ++counter)
    {
        *ParamMarker = Table->param_marker;

        if (wcscmp (VarName, Table->Arr[counter].name) == 0)
        {
            *RetIndex = Table->Arr[counter].index;

            return true;
        }
    }

    return false;
}

//=============================================================================================================================================================================

void prepare_name_label_to_jump (wchar_t* LabelName, size_t Address)
{
    Address += 1;

    size_t cnt = 0;
    while (Address >= 10)
    {
        LabelName[cnt] = (wchar_t) (Address % 10);

        Address /= 10;

        ++cnt;
    }

    LabelName[cnt] = (wchar_t) (Address % 10);
    cnt++;

    LabelName[cnt] = L'\0';


    LabelName = wcscat(LabelName, L"_w_1");
}
//=============================================================================================================================================================================

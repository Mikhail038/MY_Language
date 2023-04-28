#ifndef ASM_H
#define ASM_H

//=================================================================================================================================================================================================================

#include "stackT.h"
#include "config.h"
#include "colors.h"

//=================================================================================================================================================================================================================

#include <unistd.h>

#define JUNK -1

//=================================================================================================================================================================================================================

#define DEF_CMD(name, e_num, num, ...) \
    e_num = num,

enum ECommandNums
{
    #include "commands.h"
};
#undef DEF_CMD

typedef struct
{
    ECommandNums  num;
    char          name[10];
} StructCommands;

typedef struct
{
    char* Buffer;
    int   amnt_symbols;
    int   amnt_lines;
    int   pointer;
} StructSource;

typedef struct
{
    int            ip;
    unsigned char  signature;
    unsigned char  version;
    unsigned char* ArrCode;
    int            vram_size_x;
    int            vram_size_y;
    int            size;
    FILE*          listing_file;
} StructMachineCode;

typedef struct
{
    int   num = -1;
    char* name;
} StructLabels;

#define DEF_CMD(name, e_num, ...) \
    {e_num, name},

const StructCommands ArrCommands[AmntCommands] =
{
    #include "commands.h"
};
#undef DEF_CMD

//=================================================================================================================================================================================================================

int check_passport (FILE* Bin, int* Size, int* VramSize);

//=================================================================================================================================================================================================================
//Buffer//
//=================================================================================================================================================================================================================

int count_lines_in_file (FILE* stream);

int count_symbols_in_file (FILE* stream);

void separate_buffer_on_lines (char** ArrLinePtrs, const char* Buffer, int AmntLines);

void make_array_of_code (int Amnt_lines, StructSource* Source, StructMachineCode* Code, const char* Filename);

void skip_line (StructSource* Source);

int seek (StructSource* Source);

//=================================================================================================================================================================================================================
//parsing//
//=================================================================================================================================================================================================================

int parse (StructSource* Source, StructMachineCode* Code);

int parse_push_or_pop (StructSource* Source, StructMachineCode* Code, const char* Command);

int parse_jump (StructSource* Source, StructMachineCode* Code, const char* Command);

void parse_op (StructSource* Source, StructMachineCode* Code, const char* Command);

void parse_int (StructSource* Source, StructMachineCode* Code,  const char* Command);

void parse_double (StructSource* Source, StructMachineCode* Code, const char* Command);

void parse_str (StructSource* Source, StructMachineCode* Code, const char* Command);

//=================================================================================================================================================================================================================
//jump_parse//
//=================================================================================================================================================================================================================

void jump_int (StructSource* Source, StructMachineCode* Code);

void jump_label (StructSource* Source, StructMachineCode* Code);

//=================================================================================================================================================================================================================

void print_command (StructMachineCode* Code, const char* Name);

void print_element (StructMachineCode* Code);

void make_bin_file (FILE* Bin, StructMachineCode* Code);

//=================================================================================================================================================================================================================

int clear_comments (char* Name);

void free_labels (StructMachineCode* Code);

#endif

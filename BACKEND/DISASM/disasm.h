#ifndef DISASM_H
#define DISASM_H

#include "stack.h"
#include "asm.h"

typedef struct
{
    unsigned char* Buffer;
    int            pointer;
} StructDisasm;


void change_line_end_to_newline (StructDisasm* Array);

int check_passport (FILE* Bin, StructMachineCode* Code);

int read_array_of_code (FILE* Bin, StructMachineCode* Code);



int make_text_from_code (StructDisasm* Array, StructMachineCode* Code);

int read_command (StructDisasm* Array, StructMachineCode* Code);


int reparse_push_or_pop (StructDisasm* Array, StructMachineCode* Code, const char* line);

int reparse_jump (StructDisasm* Array, StructMachineCode* Code, const char* line);

void add_to_array (StructDisasm* Array, const char* line);


void print_array (StructDisasm* Array);

int reparse_label (StructDisasm* Array, StructMachineCode* Code);

void reparse_reg (StructDisasm* Array, StructMachineCode* Code);

void reparse_int (StructDisasm* Array, StructMachineCode* Code);

void reparse_double (StructDisasm* Array, StructMachineCode* Code);

void print_text_in_file (FILE* Text, StructDisasm* Array);


#endif

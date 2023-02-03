
#ifndef PROC_H
#define PROC_H

#include <math.h>
#include "stack.h"
#include "asm.h"

typedef struct
{
    int            num  = 0;
    unsigned char* line = NULL;
} StructDebug;

typedef struct
{
    int            ip;
    int            size;
    int            size_RAM;
    int            size_VRAM;
    EModes         mode;
    StructStack*   stack;
    StructStack*   addres_stack;
    unsigned char* Array;
    double*        Regs;
    double*        RAM;
} StructCPU;

void do_dump (StructCPU* CPU);

int dis_command (StructCPU* CPU, int num);

int cpu_constructor (FILE* Bin, StructCPU* CPU);

int check_passport (FILE* Bin, StructCPU* CPU);

void init_CPU (StructCPU* CPU);

int read_array_of_code (FILE* Bin, StructCPU* CPU);

int execute_code (StructCPU* CPU);

//=============================================

//=============================================

int execute_command (StructCPU* CPU);

void cpu_destructor (StructCPU* CPU);

void run_push (StructCPU* CPU);

void run_pop (StructCPU* CPU);

void run_jump (StructCPU* CPU);

int check_passport (FILE* Bin, int* Size, int* VramSize);

#endif


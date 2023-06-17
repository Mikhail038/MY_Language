#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "proc.h"
#include "disasm.h"

#include "float_equality.h"

#define STACKCTOR(d_stack, d_capacity) stack_constructor (d_stack, d_capacity)

static const int REGS_CONST = 5;

static const int dmpsz = 12;

static int start = 100;

static int width = 0;
static int height = 0;

StructDisasm* Array;
StructMachineCode* Code;

StructDebug* ArrDbg;

int dis_command (StructCPU* CPU, int num)
{
    int n = Junk;

    for (int counter = 0; counter < CPU->size; counter++)
    {
        if (num == ArrDbg[counter].num)
        {
            n = counter;
            break;
        }
    }

    if (n != Junk)
    {
        printf (Kbright KYLW "%s" KNRM, ArrDbg[n].line);
    }

    return 0;
}

#undef DEF_CMD

void dump_code (StructCPU* CPU)
{
    int min = (CPU->ip > dmpsz) ? CPU->ip - dmpsz : 0;
    int max = (CPU->ip < (CPU->size - dmpsz)) ? CPU->ip + dmpsz : CPU->size;

    printf (Kunderscore "Code line:\n" KNRM);
    for (int i = min; i < max; i++)
    {
        printf (KWHT "%04d|         0x%02X" , i, CPU->Array[i]);


        if (i == CPU->ip)
        {
            printf (Kreverse);
        }

        dis_command (CPU, i);

        if (i == CPU->ip)
        {
            printf (KYLW " <" KNRM);
        }
        printf ("\n");
    }
    printf ("\n");

    return;
}

void do_dump (StructCPU* CPU)
{
    if (CPU->mode != DBG_mode)
    {
        return;
    }

    system ("clear");

    printf ("\n");
    printf (Kbright "====DUMP LOG========\n" KNRM);
    printf ("%s %s\n\n", __DATE__, __TIME__);

    dump_code (CPU);

    printf (KNRM Kunderscore "Registers:\n" KNRM);
    int counter = 0;
    for (; counter < REGS_CONST - 1; counter++)
    {
        printf (KCYN "r""%c""x| %13lg\n" KNRM , counter + (int) 'a', CPU->Regs[counter]);
    }

    printf ("\n");


    printf (Kunderscore "Stack:\n" KNRM);

    counter = 0;
    for (; counter < CPU->stack->size; counter++)
    {
        printf (KBLU "%04d| %13lg\n" KNRM , counter, CPU->stack->data[counter]);
    }

    if (counter == 0)
    {
        printf (KGRN "empty\n" KNRM);
    }

    printf ("\n");

    printf (Kunderscore "Address stack:\n" KNRM);

    counter = 0;
    for (; counter < CPU->address_stack->size; counter++)
    {
        printf (KMGN "%04d| %13lg\n" KNRM, counter, CPU->address_stack->data[counter]);
    }

    if (counter == 0)
    {
        printf (KGRN "empty\n" KNRM);
    }

    printf ("\n");

    printf (Kunderscore "RAM:\n" KNRM);

    counter = 0;
    int ram_elems_cnt = 0;
    for (; counter < CPU->size_RAM; counter++)
    {
        if (FLOAT_IS_NOT_EQUAL(CPU->RAM[counter], 0))
        {
            if ((ram_elems_cnt % 10) > 8)
            {
                printf ("\n");
            }

            printf (KWHT "|[%02d]" KRED Kbright" %4lg" KWHT "| " KNRM, counter, CPU->RAM[counter]);
            ram_elems_cnt++;
        }
    }

    if (ram_elems_cnt == 0)
    {
        printf (KGRN "empty\n" KNRM);
    }

    printf ("\n");


    printf (Kbright"====================\n" KNRM);

    // char* buff_str = (char*) calloc (sizeof (buff_str), 10);
    // scanf ("%s", buff_str);
    // if ((strcmp (buff_str, "n")) != 0)
    // {
    //     printf ("!no! '%s' \n", buff_str);
    //     exit (0);
    // }

    char cx = getchar ();
    if (cx != '\n')
    {
        printf ("!no way!\n");
        return;
    }

}

int cpu_constructor (FILE* Bin, StructCPU* CPU)
{
    check_passport (Bin, &(CPU->size), &(CPU->size_VRAM));

    read_array_of_code (Bin, CPU);

    CPU->Regs = (double*) calloc (REGS_CONST, sizeof (*CPU->RAM));
    CPU->size_RAM = start + CPU->size_VRAM;
    CPU->RAM  = (double*) calloc (CPU->size_RAM, sizeof (*CPU->RAM));

    CPU->stack = (SStack<double>*) calloc (1, sizeof (SStack<double>));
    CPU->address_stack = (SStack<double>*) calloc (1, sizeof (SStack<double>));

    STACKCTOR (CPU->stack, StartStackSize);
    STACKCTOR (CPU->address_stack, StartAddressStackSize);

    if (CPU->mode == DBG_mode)
        {
            Code = (StructMachineCode*) calloc (1, sizeof (*Code));
            Code->size = CPU->size;
            Code->ArrCode = CPU->Array;
            Code->ip = 0;

            Array = (StructDisasm*) calloc (1, sizeof (*Array));
            Array->Buffer = (unsigned char*) calloc (CPU->size, TEMPORARY_CONST_4);
            Array->pointer = 0;

            ArrDbg = (StructDebug*) calloc (CPU->size * TEMPORARY_CONST_4, sizeof (*ArrDbg));

            //make_text_from_code (Array, Code); //printf ("'%s'", Array->Buffer); exit (0);

            int counter = 0;

            for (; Code->ip < Code->size;)
            {
                ArrDbg[counter].num = Code->ip;
                int inline_counter = Array->pointer;

                if (read_command (Array, Code) == StdError)
                {
                    printf ("crush!\n");
                    return StdError;
                }

                if (strncmp ((const char*) &(Array->Buffer[inline_counter]), "label", SimpleLabelLineSize) == 0)
                {
                    int backward_counter = 2;

                    while (Array->Buffer[Array->pointer - backward_counter] != '\0')
                    {
                        //printf ("'%c'", Array->Buffer[Array->pointer - x]);
                        backward_counter++;
                    }
                    //printf ("=%s=", &(Array->Buffer[Array->pointer - x + 1]));

                    ArrDbg[counter].line = &(Array->Buffer[Array->pointer - backward_counter + 1]);

                    counter++;

                    continue;
                }

                ArrDbg[counter].line = &(Array->Buffer[inline_counter]);

                counter++;
            }

            //  for (int i = 0; i < 30; i++)
            // {
            //     printf ("%d '%s'\n", ArrDbg[i].counter, ArrDbg[i].line);
            // }
            // exit (0);
        }

    return 0;
}

/*
int check_passport (FILE* Bin, StructCPU* CPU)
{
    MCA (Bin != NULL, StdError);
    MCA (CPU != NULL, StdError);

    fread (&signature, 1, sizeof (signature), Bin);
    fread (&version,   1, sizeof (version),   Bin);
    fread (&CPU->size, 1, sizeof (CPU->size), Bin);
    fread (&vram_size, 1, sizeof (vram_size), Bin);

    MCA (CPU->size > 0, 1);
    MCA (vram_size >= 0, 2);

    if (signature != StdSign)
    {
        printf ("Wrong sygnature!\n");
        exit (0);
    }
    if (version != StdVersion)
    {
        printf ("Wrong version!\n");
        exit (0);
    }
    // printf ("%0.2X %0.2X   %0.2X %0.2X %0.2X %0.2X (%d)\n", Code->sygnature, Code->version,
    //  ((unsigned char*) &Code->size)[0],  ((unsigned char*) &Code->size)[1],
    //  ((unsigned char*) &Code->size)[2],  ((unsigned char*) &Code->size)[3], Code->size);

    return 0;
}
*/
int read_array_of_code (FILE* Bin, StructCPU* CPU)
{
    MCA (Bin  != NULL, StdError);
    MCA (CPU  != NULL, StdError);

    CPU->Array = (unsigned char*) calloc (CPU->size * TEMPORARY_CONST_1, sizeof (*CPU->Array));
    fread (CPU->Array, CPU->size, sizeof (*CPU->Array), Bin);

    return 0;
}

int execute_code (StructCPU* CPU)
{
    for (; CPU->ip < CPU->size;)
    {
        if (CPU->ip == 0)
        {
            do_dump (CPU);
        }

        int return_value = execute_command (CPU);

        if (return_value == ExError)
        {
            printf ("crush\n");
            return StdError;
        }
        else if (return_value == ExHlt)
        {
            printf ("really halted! ip = %d \n", CPU->ip);
            return 0;
        }

    }

    printf ("overflow\n");
    return StdError;
}

#define DEF_CMD(name, e_num, num, f_proc, f_parse, f_reparse) \
case num: \
    f_proc \
    do_dump (CPU); \
    return 0;

int execute_command (StructCPU* CPU)
{
    int marker = CPU->Array[CPU->ip] & (sixth_bit - 1);

    for (int counter = 0; counter < AmntCommands; ++counter)
    {
       // printf ("%0.4d %d %d\n", CPU->ip, ArrCommands[i].num, marker);
        if (ArrCommands[counter].num == marker)
        {
            switch (marker)
            {
                #include "../ASM/commands.h"
                default:
                    printf ("default error!\n");
                    return StdError;
            }
        }
    }
    printf ("Unknown Command! marker = %d ip %d\n", marker, CPU->ip);

    return StdError;
}
#undef DEF_CMD

void cpu_destructor (StructCPU* CPU)
{
    free (CPU->Array);
    free (CPU->RAM);
    free (CPU->Regs);

    stack_destructor (CPU->stack);
    stack_destructor (CPU->address_stack);

    if (CPU->mode == DBG_mode)
    {
        free (Code);
        free (ArrDbg);
        free (Array->Buffer);
        free (Array);
    }
    return;
}

//=================================================================

#define PUSH_CMD(num,...) \
case num: \
    __VA_ARGS__ \
    break;

void run_push (StructCPU* CPU)
{
    int marker = 0;
    marker = (CPU->Array[CPU->ip] >> marker_mv);
    CPU->ip++;

    switch (marker)
    {
        #include "Push.h"
        default:
            printf ("push error\n");
            return;
    }
}
#undef PUSH_CMD

#define POP_CMD(num,...) \
case num: \
    __VA_ARGS__ \
    break;

void run_pop (StructCPU* CPU)
{
    int marker = 0;
    marker = ( CPU->Array[CPU->ip] >> marker_mv);
    CPU->ip++;

    switch (marker)
    {
        #include "Pop.h"
        default:
            printf ("pop error\n");
            return;
    }
}
#undef POP_CMD

//=================================================================

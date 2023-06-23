#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm.h"


static int ROUND = FIRST_ROUND;

static StructLabels ArrLabels[LabelsAmnt] = {};

int read_array_of_code (FILE* Bin, StructMachineCode* Code)
{
    MY_COOLER_ASSERT (Bin  != NULL, StdError);
    MY_COOLER_ASSERT (Code != NULL, StdError);

    Code->ArrCode = (unsigned char*) calloc (Code->size * TEMPORARY_CONST_1, sizeof (*Code->ArrCode));
    fread (Code->ArrCode, Code->size, sizeof (*Code->ArrCode), Bin);

    //printf ("%0.2X %d", Code->ArrCode[Code->ip], Code->ip);

    return 0;
}

int make_text_from_code (StructDisasm* Array, StructMachineCode* Code)
{
    Array->pointer = 0;
    Code->ip = 0;

    for (; Code->ip < Code->size;)
    {
        if (read_command (Array, Code) != 0)
        {
            printf ("crush!\n");
            return StdError;
        }
    }

    if (ROUND < FIRST_ROUND + 1)
    {
        ROUND++;
        make_text_from_code (Array, Code);
    }

    // for (int i = 0; i < LabelsAmnt; i++)
    // {
    //     printf ("%d  %s\n", ArrLabels[i].num, ArrLabels[i].name);
    // }

    return 0;
}


#define DEF_CMD(name, e_num, num, f_proc, f_parse, f_reparse)\
    case e_num: \
        f_reparse \
        return 0;

int read_command (StructDisasm* Array, StructMachineCode* Code)
{
    for (int counter = 0; counter < LabelsAmnt; ++counter)
    {
        if (Code->ip == ArrLabels[counter].num)
        {
            int length = strlen (ArrLabels[counter].name);

            for (int inline_counter = 0; inline_counter < length; ++inline_counter) \
            {
                Array->Buffer[Array->pointer] = (ArrLabels[counter].name)[inline_counter];
                Array->pointer++;
            }

            Array->Buffer[Array->pointer] = ':';
            Array->pointer++;

            Array->Buffer[Array->pointer] = '\0';
            Array->pointer++;
        }
    }


    int marker = Code->ArrCode[Code->ip] & FirstFiveBitMask;

    for (int counter = 0;  counter < AmntCommands; ++counter) \
    {
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
    printf ("Unknown Command! marker = %d ip %d\n", marker, Code->ip);

    return 0;
}
#undef DEF_CMD

void add_to_array (StructDisasm* Array, const char* line)
{
    Array->Buffer[Array->pointer] = '\t';
    Array->pointer++;

    int length = strlen (line);

    for (int counter = 0; counter < length; ++counter)
    {
        Array->Buffer[Array->pointer] = line[counter];
        Array->pointer++;
    }
    Array->Buffer[Array->pointer] = ' ';
    Array->pointer++;

    return;
}

int reparse_push_or_pop (StructDisasm* Array, StructMachineCode* Code, const char* line)
{
    if ((Code->ArrCode[Code->ip] & eighth_bit) && (Code->ArrCode[Code->ip] & seventh_bit))
    {
        Array->Buffer[Array->pointer] = '[';
        Array->pointer++;

        Code->ip++;
        reparse_reg (Array, Code);

        Array->Buffer[Array->pointer] = ']';
        Array->pointer++;
    }
    else if (Code->ArrCode[Code->ip] & eighth_bit)
    {
        Array->Buffer[Array->pointer] = '[';
        Array->pointer++;

        Code->ip++;
        reparse_int (Array, Code);
        //printf ("oi\n");

        Array->Buffer[Array->pointer] = ']';
        Array->pointer++;
    }
    else if (Code->ArrCode[Code->ip] & seventh_bit)
    {
        Code->ip++;
        //printf ("r\n");
        reparse_reg (Array, Code);
    }
    else if (strcmp (line, "push") == 0)
    {
        Code->ip++;
        //printf ("d\n");
        reparse_double (Array, Code);
    }
    else if (strcmp (line, "pop") == 0)
    {
        Code->ip++;
    }

    //print_array (Array);
    //printf ("\n");

    Array->Buffer[Array->pointer] = '\0';
    Array->pointer++;

    return 0;
}



int reparse_jump (StructDisasm* Array, StructMachineCode* Code, const char* line)
{
    Code->ip++;

    int num = reparse_label (Array, Code);

    char* str = (char*) calloc (StdLabelLineSize + IntegerLineSize, sizeof (*str));
    str = strcpy (str, "label_");
    sprintf (&(str[StdLabelLineSize]), "%d", num);


    for (int counter = 0; counter < LabelsAmnt; ++counter)
    {
        if (ArrLabels[counter].num == num)
        {
            break;
        }

        if (ArrLabels[counter].num == InvalidLabel)
        {
            //printf ("%d %d\n", i, ArrLabels[i].num);
            ArrLabels[counter].name = (char*) calloc (StdLabelLineSize + IntegerLineSize, sizeof (*ArrLabels[counter].name));
            ArrLabels[counter].name = strcpy (ArrLabels[counter].name, str);
            ArrLabels[counter].num = num;
            //printf ("%-3d %-3d %s\n", i, ArrLabels[i].num , ArrLabels[i].name);
            // fprintf (Code->listing_file, "added label: %s %d  [%0.2X %0.2X %0.2X %0.2X]\n", Name, Code->ip,
            break;
        }
    }

    free (str);

    Array->Buffer[Array->pointer] = '\0';
    Array->pointer++;

    return 0;
}

void print_array (StructDisasm* Array)
{
    for (int counter = 0; counter < Array->pointer; ++counter)
    {
        printf ("%c", Array->Buffer[counter]);
    }
    printf ("\n");

    return;
}

int reparse_label (StructDisasm* Array, StructMachineCode* Code)
{
    int argument = 0;

    for (int counter = 0; counter < sizeof (int); ++counter)
    {
        ((unsigned char*) &argument)[counter] = Code->ArrCode[Code->ip];
        Code->ip++;
    }
    char buf[StdLabelLineSize + IntegerLineSize];

    sprintf (buf, "label_%d", argument);

    int length = strlen (buf);

    sprintf ((char*) &Array->Buffer[Array->pointer], "%s", buf);
    Array->pointer += length;

    return argument;
}

void reparse_reg (StructDisasm* Array, StructMachineCode* Code)
{
    Array->Buffer[Array->pointer] = 'r';
    Array->pointer++;

    Array->Buffer[Array->pointer] = 'a' + Code->ArrCode[Code->ip] - 1;
    Code->ip++;
    Array->pointer++;

    Array->Buffer[Array->pointer] = 'x';
    Array->pointer++;
}

void reparse_int (StructDisasm* Array, StructMachineCode* Code)
{
    int argument = 0;

    for (int counter = 0; counter < sizeof (int); ++counter)
    {
        ((unsigned char*) &argument)[counter] = Code->ArrCode[Code->ip];
        Code->ip++;
    }
    char buf[IntegerLineSize];

    sprintf (buf, "%d", argument);

    int length = strlen (buf);

    sprintf ((char*) &Array->Buffer[Array->pointer], "%d", argument);
    Array->pointer += length;
    //printf ("!%s!-------------------------------", Array->Buffer);

    return;
}

void reparse_double (StructDisasm* Array, StructMachineCode* Code)
{
    double argument = 0;

    for (int counter = 0; counter < sizeof (double); ++counter)
    {
        ((unsigned char*) &argument)[counter] = Code->ArrCode[Code->ip];
        Code->ip++;
    }
    char buf[DoubleLineSize];

    sprintf (buf, "%lg", argument);

    int length = strlen (buf);

    sprintf ((char*) &Array->Buffer[Array->pointer], "%lg", argument);

    Array->pointer += length;
}


void print_text_in_file (FILE* Text, StructDisasm* Array)
{
    change_line_end_to_newline (Array);

    fprintf (Text, "==========================================================\n");
    fprintf (Text, "DISASSEMBLED FILE\n");
    fprintf (Text, "==========================================================\n");

    fwrite (Array->Buffer, Array->pointer, sizeof (*Array->Buffer), Text);
    fprintf (Text, "==========================================================\n");

    for (int counter = 0; counter < LabelsAmnt; counter++)
    {
        if (ArrLabels[counter].num != InvalidLabel)
        {
            free (ArrLabels[counter].name);
        }
    }
}

void change_line_end_to_newline (StructDisasm* Array)
{
    for (int counter = 0; counter < Array->pointer; ++counter)
    {
        if (Array->Buffer[counter] == '\0')
        {
            Array->Buffer[counter] = '\n';
        }
    }

    return;
}

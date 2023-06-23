#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm.h"

static int ROUND = 1;

//DO NOT BEGIN LABELS AFTER EMPTY POP WITH LETTER R
//DO NOT NAME LABEL "LABEL"

enum ERegisterNums
{
    rax  = 1,
    rbx  = 2,
    rcx  = 3,
    rdx  = 4
};

typedef struct
{
    ERegisterNums  num;
    const char*    name;
} StructRegisters;

//extern StructCommands ArrCommands[AmntCommands];

static const StructRegisters ArrRegisters[4] =
{
    {rax, "rax"},
    {rbx, "rbx"},
    {rcx, "rcx"},
    {rdx, "rdx"}
};

static StructLabels ArrLabels[LabelsAmnt] = {};

int count_lines_in_file (FILE* stream)
{
    MY_COOLER_ASSERT (stream != NULL, -1);

    char sym = '\0';
    int AmntLines = 0;

    while ((sym = getc (stream)) != EOF)
    {
        if (sym != '\n')
        {
            AmntLines++;
            while ((sym = getc (stream)) != '\n');
        }
    }

    return AmntLines;
}

int count_symbols_in_file (FILE* stream)
{
    MY_COOLER_ASSERT (stream != NULL,  -1);

    fseek (stream, 0, SEEK_END);

    int AmntSymbols = ftell (stream);

    fseek (stream, 0, SEEK_SET);
    return AmntSymbols;
}

void separate_buffer_on_lines (char** ArrLinePtrs, const char* Buffer, int AmntSymbols)
{
    MY_COOLER_ASSERT (ArrLinePtrs != NULL, (void) 0);
    MY_COOLER_ASSERT (Buffer != NULL, (void) 0);
    MY_COOLER_ASSERT (AmntSymbols >= 0, (void) 0);

    int FirstCharNum = 0, CharNum = 0, LineNumber = 0;


    while (FirstCharNum < AmntSymbols)
    {
        CharNum = 0;

        while (Buffer[FirstCharNum + CharNum] != '\n')
        {
            CharNum++;
        }

        if (CharNum != 0)
        {

            ArrLinePtrs[LineNumber] = (char*) &(Buffer[FirstCharNum]);

            ArrLinePtrs[LineNumber][CharNum] = '\0';

            printf ("!%s!\n", ArrLinePtrs[LineNumber]);

            LineNumber++;
        }

        FirstCharNum += CharNum + 1;
    }
    return;
}

int open_listing_file (const char* Filename, StructMachineCode* Code)
{
    Code->listing_file = fopen (Filename, "w");
    MY_COOLER_ASSERT (Code->listing_file != NULL, 1);

    fprintf (Code->listing_file,
             "===============================================================\n"
             "LISTING\n"
             "===============================================================\n"
             "Compilation started: %s %s\n"
             "---------------------------------------------------------------\n"
            , __DATE__, __TIME__);

    return 0;
}

int close_listing_file (StructMachineCode* Code)
{
    MY_COOLER_ASSERT (Code->listing_file != NULL, 1);

    int size = Code->vram_size_x * Code->vram_size_y;

    fprintf (Code->listing_file,
             "---------------------------------------------------------------\n"
             "sygnature 0x%.2X | version 0x%.2X (%d) |"
             " size [%.2X %.2X %.2X %.2X] (%d) | VRAM-size [%.2X %.2X %.2X %.2X] (%d)\n"
             "---------------------------------------------------------------\n"
             "Compilation ended: %s %s\n"
             "===============================================================\n"
            ,Code->signature, Code->version, Code->version,
            ((unsigned char*) &Code->size)[0], ((unsigned char*) &Code->size)[1],
            ((unsigned char*) &Code->size)[2], ((unsigned char*) &Code->size)[3], Code->size,
            ((unsigned char*) &size)[0], ((unsigned char*) &size)[1],
            ((unsigned char*) &size)[2], ((unsigned char*) &size)[3], size,
             __DATE__, __TIME__);

    fclose (Code->listing_file);
    return 0;
}



void make_array_of_code (int Amnt_lines, StructSource* Source, StructMachineCode* Code, const char* Filename)
{
    Code->ip = 0;
    Source->pointer  = 0;
    Source->amnt_lines = Amnt_lines;

    if (ROUND < 2)
    {
        open_listing_file (Filename, Code);
    }

    int LineNum = 0;
    for (; LineNum < Source->amnt_lines * 8; LineNum++)
    {
        if (seek (Source) == 0)
        {
            if (parse (Source, Code) == -1)
            {
                printf ("crush!\n");
                return;
            }
        }
    }
    //printf ("%d %d\n", LineNum, Source->amnt_lines);

    Code->signature = StdSign;
    Code->version   = StdVersion;
    Code->size      = Code->ip;

    fprintf (Code->listing_file,"---------------------------------------------------------------\n"
                                "Round %d ended\n"
                                "---------------------------------------------------------------\n"
                                , ROUND);

    if (ROUND < 2)
    {
        ROUND++;
        make_array_of_code (Amnt_lines, Source, Code, Filename);
    }

    return;
}

int seek (StructSource* Source)
{
    for (; Source->pointer < Source->amnt_symbols; Source->pointer++)
    {
        if ( (Source->Buffer[Source->pointer] != ' ' ) &&
             (Source->Buffer[Source->pointer] != '\n') &&
             (Source->Buffer[Source->pointer] != '\0'))
        {
            if (Source->Buffer[Source->pointer] == ';')
            {
                skip_line (Source);
                continue;
            }

            //printf ("'%c'\n", Source->Buffer[Source->pointer]);
            return 0;
        }
    }
    return 1;

}

void skip_line (StructSource* Source)
{
    Source->amnt_lines--;
    //Source->pointer++;

    for (; Source->pointer < Source->amnt_symbols; Source->pointer++)
    {
        if (Source->Buffer[Source->pointer] == '\n')
        {
            return;
        }
    }
}

int add_label (char* Name, StructMachineCode* Code)
{
    int Length = strlen (Name);

    Name[Length] = '\0';


    for (int counter = 0; counter < LabelsAmnt; ++counter)
    {
        if  ((ArrLabels[counter].num != InvalidLabel) && (strncmp (Name, ArrLabels[counter].name, Length - 1) == 0))
        {
            return 0;
        }

        if (ArrLabels[counter].num == InvalidLabel)
        {
            ArrLabels[counter].name = (char*) calloc (Length + 1, sizeof (char));
            ArrLabels[counter].name = strcpy (ArrLabels[counter].name, Name);

            (ArrLabels[counter].name)[Length - 1] = '\0';
            ArrLabels[counter].num  = Code->ip;

            fprintf (Code->listing_file, "added label: %s %d  [%.2X %.2X %.2X %.2X]\n", Name, Code->ip,
            ((unsigned char*) &Code->ip)[0],
            ((unsigned char*) &Code->ip)[1],
            ((unsigned char*) &Code->ip)[2],
            ((unsigned char*) &Code->ip)[3]
            );

            return 0;
        }
    }

    return 1;
}



#define DEF_CMD(name, e_num, num, f_proc, f_parse, f_reparse)\
    case e_num: \
        f_parse

int parse (StructSource* Source, StructMachineCode* Code)
{
    char* Name = &(Source->Buffer[Source->pointer]);

    //printf ("(%s)\n", Name);

    int inline_counter = 0;
    for (; inline_counter < Source->amnt_symbols; ++inline_counter)
    {
        if ((Name[inline_counter] == ' ') || (Name[inline_counter] == '\n') || (Name[inline_counter] == '\0'))
        {
            Name[inline_counter] = '\0';
            break;
        }
    }

    //printf ("(%s) '%c'\n", Name, Name[i - 1]);

    if (Name[inline_counter - 1] == ':')
    {
        if (add_label (Name, Code) != 0)
        {
            printf ("error labels same '%s'\n", Name);
            return StdError;
        }
        Source->pointer += inline_counter;
        return 0;
    }
    else
    {
        for (int counter = 0; counter < AmntCommands; counter++)
        {
            if (strcmp (Name, ArrCommands[counter].name) == 0)
            {
                int marker = ArrCommands[counter].num;

                Code->ArrCode[Code->ip] = ArrCommands[counter].num;
                //printf ("#%0.2X\n", Code->ArrCode[Code->ip]);

                Source->pointer += inline_counter;
                Code->ip++;

                //printf ("!! %d\n", marker);

                switch (marker)
                {
                    #include "commands.h"
                    default:
                        printf ("default error! %s %d \n", Name, marker);
                        return StdError;
                }

                return 0;
            }
        }
        printf ("Unknown command! --%s-- ip %d\n", Name, Code->ip);
        return StdError;
    }
}

#undef DEF_CMD


int parse_push_or_pop (StructSource* Source, StructMachineCode* Code, const char* Command)
{
    seek (Source);

    char* Name = &(Source->Buffer[Source->pointer]);

    int counter = 0;
    for (; counter < Source->amnt_symbols - Source->pointer - 1; ++counter)
    {
        //printf ("!%d! %d %d\n", counter, Source->pointer, Source->amnt_symbols);
        if ((Name[counter] == ' ') || (Name[counter] == '\n') || (Name[counter] == '\0'))
        {
            Name[counter] = '\0';
            counter = JUNK;
            break;
        }
    }

    if (counter != JUNK)
    {
        printf ("Syntax error! (Maybe because of empty pop in the end)\n");
        return StdError;
    }

    //printf ("counter %d\n", counter);
    //printf ("'%c' %d\n", Name[0], Name[0]);

    if (Name[0] == '[')
    {
        parse_op (Source, Code, Command);
    }
    else if ((Name[0] == 'r') && (Name[2] == 'x'))
    {
        parse_str (Source, Code, Command);
    }
    else if ((Command = "push") && (Name[0] <= '9') && ((Name[0] >= '0')) || (Name[0] == '-') )
    {
        parse_double (Source, Code, Command);
    }
    else
    {
        if (Command = "pop")
        {
            //Source->pointer++;
            //printf ("!'%s'\n", Name);
            print_command (Code, Command);

            return 0;
        }
        else
        {
            printf ("error!\n");
            return StdError;
        }

    }
    return 0;

}

int parse_jump (StructSource* Source, StructMachineCode* Code, const char* Command)
{
    seek (Source);

    char* Name = &(Source->Buffer[Source->pointer]);

    int i = 0;
    for (; i < Source->amnt_symbols - Source->pointer; i++)
    {
        if ((Name[i] == ' ') || (Name[i] == '\n'))
        {
            Name[i] = '\0';
            break;
        }
    }

    if ((Name[0] >= 'A') && (Name[0] <= 'z'))
    {
        print_command (Code, Command);

        jump_label (Source, Code);
        return 0;
    }
    if ((Name[0] >= '0') && (Name[0] <= '9'))
    {
        print_command (Code, Command);

        jump_int (Source, Code);
        return 0;
    }
    else
    {
        printf ("no label\n");
    }

    return -1;
}

void parse_int (StructSource* Source, StructMachineCode* Code,  const char* Command)
{
    //fprintf (Code->listing_file, "%0.4d %0.2X %s\n", Code->ip - 1, Code->ArrCode[Code->ip - 1], "push");
    print_command (Code, Command);

    char* Name = &(Source->Buffer[Source->pointer]);


    int length = strlen (Name);

    int argument = 0;
    sscanf (Name, "%d", &argument);


    int counter = 0;
    for ( ;counter < sizeof (int); ++counter)
    {
        //printf ("%x", ((unsigned char*) &d_argument)[j]);
        Code->ArrCode[Code->ip] = ((unsigned char*) &argument)[counter];
        print_element (Code);
        Code->ip++;
    }

    if (strcmp (Command, "vsetx") == 0)
    {
        if (argument > Code->vram_size_x)
        {
            Code->vram_size_x = argument;
        }
    }
    if(strcmp (Command, "vsety") == 0)
    {
        if (argument > Code->vram_size_y)
        {
            Code->vram_size_y = argument;
        }
    }

    //printf ("'%s' %d %d\n", Name, length, i_argument);

    Source->pointer += length;

    //printf ("(%s) pointer %d\n", Name, *pointer);

    return;
}

void parse_double (StructSource* Source, StructMachineCode* Code,  const char* Command)
{
    //fprintf (Code->listing_file, "%0.4d %0.2X %s\n", Code->ip - 1, Code->ArrCode[Code->ip - 1], "push");
    print_command (Code, Command);

    char* Name = &(Source->Buffer[Source->pointer]);


    int length = strlen (Name);

    double argument = strtod (Name, NULL);


    int counter = 0;
    for ( ;counter < sizeof (double); ++counter)
    {
        //printf ("%x", ((unsigned char*) &d_argument)[j]);
        Code->ArrCode[Code->ip] = ((unsigned char*) &argument)[counter];
        print_element (Code);
        Code->ip++;
    }

    //printf (" %s %lg\n", Name, d_argument);

    Source->pointer += length;

    //printf ("(%s) pointer %d\n", Name, *pointer);

    return;
}

void parse_str (StructSource* Source, StructMachineCode* Code, const char* Command)
{
    Code->ArrCode[Code->ip - 1] = (unsigned char)((int) Code->ArrCode[Code->ip - 1] + seventh_bit);

    print_command (Code, Command);

    char* Name = &(Source->Buffer[Source->pointer]);


    int length = strlen (Name);

    clear_comments (Name);

    for (int counter = 0; counter < length; counter++)
    {
        if (Name[counter] == ' ')
        {
            Name[counter] = '\0';
        }
    }

    //printf ("!%s!\n", Name);

    int in_counter = 0;
    for ( ; in_counter < sizeof (int); in_counter++)
    {
        if (strcmp (Name, ArrRegisters[in_counter].name) == 0)
        {
            Code->ArrCode[Code->ip] = (unsigned char) ArrRegisters[in_counter].num;
            print_element (Code);
            Code->ip++;
        }
    }

    Source->pointer += length;

    return;
}

void parse_op (StructSource* Source, StructMachineCode* Code, const char* Command)
{
    Code->ArrCode[Code->ip - 1] = (unsigned char)((int) Code->ArrCode[Code->ip - 1] + eighth_bit);

    //print_command (Code, Command);
    Source->pointer++;

    char* Name = &(Source->Buffer[Source->pointer]);



    int length = strlen (Name);

    if (Name[0] == 'r')
    {
        parse_str (Source, Code, Command);
    }
    else if ((Name[0] <= '9') && ((Name[0] >= '0')))
    {
        parse_int (Source, Code, Command);
    }
    else
    {
        printf ("2 op error\n");
        return;
    }

}



void jump_int (StructSource* Source,StructMachineCode* Code)
{
    char* Name = &(Source->Buffer[Source->pointer]);

    int length = strlen (Name);

    int ip = 0;
    //printf ("'%s'\n", Name);
    sscanf (Name, "%d", &ip);

    //print_command (Code, "jump");

    for (int in_counter = 0; in_counter < sizeof (int); ++in_counter)
    {
        Code->ArrCode[Code->ip] = ((unsigned char*) &(ip))[in_counter];
        print_element (Code);
        Code->ip++;
    }

    Source->pointer += length;

    return;
}

void jump_label (StructSource* Source,StructMachineCode* Code)
{
    char* Name = &(Source->Buffer[Source->pointer]);

    int old_number = Code->ip;

    int length = strlen (Name);

    for (int counter = 0; counter < LabelsAmnt; ++counter)
    {
        //printf ("%d %d %s\n", i, ArrLabels[i].num, (ArrLabels[i].num != -1) ? ArrLabels[i].name : "a" );
        if ((ArrLabels[counter].num != InvalidLabel) && (strcmp (Name, ArrLabels[counter].name) == 0))
        {
            for (int in_counter = 0; in_counter < sizeof (int); ++in_counter)
            {
                Code->ArrCode[Code->ip] = ((unsigned char*) &(ArrLabels[counter].num))[in_counter];
                print_element (Code);
                Code->ip++;
            }
        }
    }

    if (old_number == Code->ip)
    {
        if (ROUND < FIRST_ROUND + 1)
        {
            fprintf (Code->listing_file, "skipped %lud bytes (label '%s' not found)\n", sizeof (int), Name);
            Code->ip += sizeof (int);
        }
        else
        {
            printf ("No label found! --%s-- ip %d\n", Name, Code->ip);

            printf ("Array of labels:\n");
            for (int counter = 0; counter < LabelsAmnt; counter++)
            {
                printf ("%d %s\n", ArrLabels[counter].num, ArrLabels[counter].name);
            }

            return;
        }
    }

    Source->pointer += length;
}

void print_command (StructMachineCode* Code, const char* Name)
{
    fprintf (Code->listing_file, "    %04d 0x%02X %s\n", Code->ip - 1, Code->ArrCode[Code->ip - 1], Name);
    //printf ("    %0.4d %0.2X %s\n", Code->ip - 1, Code->ArrCode[Code->ip - 1], Name);

    return;
}

void print_element (StructMachineCode* Code)
{
    fprintf (Code->listing_file, "    %04d 0x%02X\n", Code->ip, Code->ArrCode[Code->ip]);
    //printf ("    %0.4d %0.2X\n", Code->ip, Code->ArrCode[Code->ip]);

    return;
}

void make_bin_file (FILE* Bin, StructMachineCode* Code)
{
    free_labels (Code);

    close_listing_file (Code);

    int size = Code->vram_size_x * Code->vram_size_y;

    fwrite (&Code->signature, 1, sizeof (Code->signature), Bin);
    fwrite (&Code->version,   1, sizeof (Code->version),   Bin);
    fwrite (&Code->size,      1, sizeof (Code->size),      Bin);
    fwrite (&size,            1, sizeof (size),            Bin);

    fwrite (Code->ArrCode, Code->ip, sizeof (*Code->ArrCode), Bin);

    return;
}


int clear_comments (char* Name)
{
    int length = strlen (Name);

    int counter = 0;

    for (; counter < length ; counter++)
    {
        if ((Name[counter] == ']') || (Name[counter] == ';'))
        {
            Name[counter] = ' ';
        }
    }

    return 0;
}

void free_labels (StructMachineCode* Code)
{
    fprintf (Code->listing_file, "---------------------------------------------------------------\n");

    for (int counter = 0; counter < LabelsAmnt; counter++)
    {
        if (ArrLabels[counter].num != -1)
        {
            fprintf (Code->listing_file,"freed label: %-3d %-15s %-5d [%.2X %.2X %.2X %.2X]\n",
            counter, ArrLabels[counter].name, ArrLabels[counter].num,
            ((unsigned char*) &ArrLabels[counter].num)[0],
            ((unsigned char*) &ArrLabels[counter].num)[1],
            ((unsigned char*) &ArrLabels[counter].num)[2],
            ((unsigned char*) &ArrLabels[counter].num)[3]
            );

            free (ArrLabels[counter].name);
        }
    }

    return;
}

int check_passport (FILE* Bin, int* Size, int* VramSize)
{
    MY_COOLER_ASSERT (Bin  != NULL, StdError);
    MY_COOLER_ASSERT (Size != NULL, StdError);
    MY_COOLER_ASSERT (VramSize != NULL, StdError);

    unsigned char signature = 0;
    unsigned char version = 0;

    fread (&signature, 1, sizeof (signature),  Bin);
    fread (&version,   1, sizeof (version),    Bin);
    fread (Size,       1, sizeof (*Size),      Bin);
    fread (VramSize,   1, sizeof (*VramSize),  Bin);

    MY_COOLER_ASSERT (*Size > 0, StdError);
    MY_COOLER_ASSERT (*VramSize >= 0, StdError);

    if (signature != StdSign)
    {
        printf ("Wrong sygnature!\n");
        return StdError;
    }
    if (version != StdVersion)
    {
        printf ("Wrong version!\n");
        return StdError;
    }

    return 0;
}

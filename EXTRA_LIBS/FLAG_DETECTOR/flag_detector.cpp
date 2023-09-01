//==================================================================================================================================================================
//20.06.23
//==================================================================================================================================================================

#include "flag_detector.h"
#include "MYassert.h"
#include "elf_back.h"
#include <cstddef>
#include <cstring>

//==================================================================================================================================================================

const int INVALID_FLAG = -1;

//==================================================================================================================================================================

// TODO nice!
void parse_elf_back_flags (ElfBack* Back, int ArgC, char** ArgV, ElfBackFlagFunction* FlagsArray, size_t FlagsAmount)
{
    int NumberOfFlag = 0;

    for (int cnt = 0; cnt != ArgC; ++cnt)
    {
        #ifdef DEBUG
        printf (KBLU "|FlagDet|" KNRM "[%s] %d/%d\n", ArgV[cnt], cnt, ArgC);
        #endif

        NumberOfFlag = is_valid_flag (ArgV[cnt], FlagsArray, FlagsAmount);

        if (NumberOfFlag != INVALID_FLAG)
        {
            if (cnt < ArgC)
            {
                if (cnt + 1 <= ArgC)
                {
                    call_flag_function (Back, FlagsArray, ArgV[cnt + 1], NumberOfFlag);
                }
                else
                {
                    call_flag_function (Back, FlagsArray, NULL, NumberOfFlag);
                }
            }
        }
    }
}

int is_valid_flag (char* Line, ElfBackFlagFunction* FlagsArray, size_t FlagsAmount)
{
    if (Line[0] != '-' || (strlen (Line) > 2))
    {
        return INVALID_FLAG;
    }

    for (int cnt = 0; cnt != FlagsAmount; ++cnt)
    {
        #ifdef DEBUG
        printf (KBLU "|FlagDet|" KNRM "flag check %c|%c\n", Line[1], FlagsArray[cnt].letter);
        #endif

        if (Line[1] == FlagsArray[cnt].letter)
        {
            return cnt;
        }
    }

    return INVALID_FLAG;
}

void call_flag_function (ElfBack* Back, ElfBackFlagFunction* FlagsArray, char* ArgumentLine, int NumberOfFlag)
{
    LinePlusElfBack Argument{.Line = ArgumentLine, .back = Back};

    FlagsArray[NumberOfFlag].function (Argument);
}


//==================================================================================================================================================================

void parse_front_flags (FrontParameters* Parameters, int ArgC, char** ArgV, FrontFlagFunction* FlagsArray, size_t FlagsAmount)
{
    int NumberOfFlag = 0;

    for (int cnt = 0; cnt != ArgC; ++cnt)
    {
        #ifdef DEBUG
        printf (KBLU "|FlagDet|" KNRM "[%s] %d/%d\n", ArgV[cnt], cnt, ArgC);
        #endif

        NumberOfFlag = is_valid_front_flag (ArgV[cnt], FlagsArray, FlagsAmount);

        if (NumberOfFlag != INVALID_FLAG)
        {
            if (cnt < ArgC)
            {
                if (cnt + 1 <= ArgC)
                {
                    call_front_flag_function (Parameters, FlagsArray, ArgV[cnt + 1], NumberOfFlag);
                }
                else
                {
                    call_front_flag_function (Parameters, FlagsArray, NULL, NumberOfFlag);
                }
            }
        }
    }
}

int is_valid_front_flag (char* Line, FrontFlagFunction* FlagsArray, size_t FlagsAmount)
{
    if (Line[0] != '-' || (strlen (Line) > 2))
    {
        return INVALID_FLAG;
    }

    for (int cnt = 0; cnt != FlagsAmount; ++cnt)
    {
        #ifdef DEBUG
        printf (KBLU "|FlagDet|" KNRM "flag check %c|%c (%d/%lu)\n", Line[1], FlagsArray[cnt].letter, cnt, FlagsAmount);
        #endif

        if (Line[1] == FlagsArray[cnt].letter)
        {
            return cnt;
        }
    }

    printf ("|" KRED Kbright "%c" KNRM "| is not valid flag!\n", Line[1]);

    return INVALID_FLAG;
}

void call_front_flag_function (FrontParameters* Parameters, FrontFlagFunction* FlagsArray, char* ArgumentLine, int NumberOfFlag)
{
    LinePlusFrontParams Argument{.Line = ArgumentLine, .parametrs = Parameters};

    FlagsArray[NumberOfFlag].function (Argument);
}

//==================================================================================================================================================================

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

void parse_flags (ElfBack* Back, int ArgC, char** ArgV, FlagFunction* FlagsArray, size_t FlagsAmount)
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

int is_valid_flag (char* Line, FlagFunction* FlagsArray, size_t FlagsAmount)
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

void call_flag_function (ElfBack* Back, FlagFunction* FlagsArray, char* ArgumentLine, int NumberOfFlag)
{
    LinePlusElfBack Argument{.Line = ArgumentLine, .back = Back};

    FlagsArray[NumberOfFlag].function (Argument);
}


//==================================================================================================================================================================

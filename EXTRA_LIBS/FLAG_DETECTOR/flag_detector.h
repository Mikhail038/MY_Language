#pragma once

//==================================================================================================================================================================
//20.06.23
//==================================================================================================================================================================

struct ElfBack;

#include <cstddef>
struct LinePlusElfBack
{
    const char* Line;
    ElfBack*    back;
};

struct FlagFunction
{
    char    letter;
    void    (*function)  (LinePlusElfBack);
};

//==================================================================================================================================================================

void parse_flags (ElfBack* Back, int argc, char** argv, FlagFunction* FlagsArray, size_t FlagsAmount);

int is_valid_flag (char* Line, FlagFunction* FlagsArray, size_t FlagsAmount);

void call_flag_function (ElfBack* Back, FlagFunction* FlagsArray, char* ArgumentLine, int NumberOfFlag);

//==================================================================================================================================================================

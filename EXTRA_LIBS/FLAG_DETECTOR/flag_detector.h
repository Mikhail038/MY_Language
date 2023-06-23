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

struct ElfBackFlagFunction
{
    char    letter;
    void    (*function)  (LinePlusElfBack);
};

//==================================================================================================================================================================

void parse_elf_back_flags (ElfBack* Back, int argc, char** argv, ElfBackFlagFunction* FlagsArray, size_t FlagsAmount);

int is_valid_flag (char* Line, ElfBackFlagFunction* FlagsArray, size_t FlagsAmount);

void call_flag_function (ElfBack* Back, ElfBackFlagFunction* FlagsArray, char* ArgumentLine, int NumberOfFlag);

//==================================================================================================================================================================

struct FrontParameters
{
    char* ast_file_name;
    char* gv_file_name;
    char* src_file_name;

    bool exit_marker;
};

struct LinePlusFrontParams
{
    const char*         Line;
    FrontParameters*    parametrs;
};

struct FrontFlagFunction
{
    char    letter;
    void    (*function)  (LinePlusFrontParams);
};

//==================================================================================================================================================================

void parse_front_flags (FrontParameters* Back, int argc, char** argv, FrontFlagFunction* FlagsArray, size_t FlagsAmount);

int is_valid_front_flag (char* Line, FrontFlagFunction* FlagsArray, size_t FlagsAmount);

void call_front_flag_function (FrontParameters* Back, FrontFlagFunction* FlagsArray, char* ArgumentLine, int NumberOfFlag);

//==================================================================================================================================================================

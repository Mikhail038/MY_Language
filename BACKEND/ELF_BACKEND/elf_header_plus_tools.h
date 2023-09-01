#pragma once

//==================================================================================================================================================================
//12.06.23
//==================================================================================================================================================================

#include "ARRAY/array.h"
#include "elf_back.h"
#include <cstddef>

//==================================================================================================================================================================

struct ElfBack;

struct ElfHead
{
    size_t file_virtual_address = 4096 * 40; //40 is almost random number, 4096 is for alignment

    const char magic_signature = 0x7f; //MAGIC SIGNATURE
    const char elf_name[3] = {'E', 'L', 'F'};
    const char elf_class  = 0x02; //Class (64-bit = 2) // TODO align constants, please)) Also, it's okay to use clang-format if you want to
    const char endian     = 0x01; //Endian (little endian = 1)
    const char elf_version = 0x01; //Elf Version (only correct = 1)
    const char os_abi     = 0x00; //OS ABI (System V (UNIX) = 0)

    const char abi_version = 0x00; //ABI version (none = 0)
    //7 padding

    const char elf_type   = 0x02; //Type (executable = 2)
    //1 padding

    const char machine    = 0x3e; //Machine (x86_64 = 3e)
    //1 padding

    const char version    = 0x01; //Version (only correct = 1)
    //3 padding

    const char flags  = 0x00; //Flags (no flags = 0)
    //3 padding

    const short int header_size = 64; //standard
    const short int program_header_size = 56; //standard
    const short int program_headers_cnt = 1; //in my case
    const short int section_headers_size = 64; //standard
    const short int section_headers_cnt = 3; //in my case
    const short int shstr_table_index  = 2; //in my case

    const char segment_type = 0x01; //Segment type (load = 0)
    //3 padding

    const char segment_flag = 0x05; //Segment flag (r w x = 0 1 2)
    //3 padding

    const char segment_offset = 0x00; //Segment offset (0)
    //7 padding

    //further padding is not stated here

    const char alignment[8] = {0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,};

    const char text_section_type = 0x01; //Section type (bits = 1)
    const char text_section_flags = 0x06; //Section flags (r w x )
    const char text_section_index = 0x00; //Section index
    const char text_section_extra_info = 0x00; //Extra info
    const char text_section_align = 0x10; //Alignment
    const char text_section_extra_sizes = 0x00; //Extra sizes

    const char shstrtable_section_type = 0x03;
    const char shstrtable_section_flags = 0x00;
    const char shstrtable_section_index = 0x00;
    const char shstrtable_section_extra_info = 0x00;
    const char shstrtable_section_align = 0x01;
    const char shstrtable_section_extra_sizes = 0x00;

    const char shstrtable_text_name[6] = {'.', 't', 'e', 'x', 't', '\0'};
    const char shstrtable_table_name[12] = {'.', 's', 'h', 's', 't', 'r', 't', 'a', 'b', 'l', 'e', '\0'};
};

union UniversalNumber
{
    char char_data;
    short int short_data;
    int int_data;
    size_t long_data;
};

const UniversalNumber NULL_UNIVERSAL_NUMBER =
{
    .char_data = 0x00
};

enum SupportedPatches
{
    FileVirtualAddress,
    ProgramHeadersStart,
    SectionHeadersStart,
    SegmentSize,
    SegmentFileSize,
    TextNameOffset,
    EntryVirtualAddress,
    TextOffset,
    TextSize,
    TableNameOffset,
    TableLoadAddress,
    TableAddress,
    TableSize,
};

struct Patch
{
    size_t              size_of_data;
    SupportedPatches    name;
    UniversalNumber     data;
    size_t              address_in_byte_array;
};

//==================================================================================================================================================================

void create_empty_space_for_patch (ElfBack* Back, SupportedPatches Name, size_t SizeOfData);

void paste_patch (ElfBack* Back, SupportedPatches Name, UniversalNumber Data);

//==================================================================================================================================================================

void elf_head_constructor (ElfHead* Head);

//==================================================================================================================================================================

void set_one_byte (ElfBack* Back, char Byte);

void set_bytes (ElfBack* Back, void* Data, size_t Size);

//==================================================================================================================================================================

void align_one_byte (ElfBack* Back, size_t Alignment);

//==================================================================================================================================================================

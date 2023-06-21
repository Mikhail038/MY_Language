//==================================================================================================================================================================
//12.06.23
//==================================================================================================================================================================

#include "elf_header_plus_tools.h"
#include "ARRAY/array.h"
#include "MYassert.h"
#include "elf_back.h"
#include "stackT.h"
#include <cstddef>
#include <cstdlib>
#include <cstring>

//==================================================================================================================================================================

void create_empty_space_for_patch (ElfBack* Back, SupportedPatches Name, size_t SizeOfData)
{
    Patch* NewPatch = (Patch*) calloc(1, sizeof (Patch));

    NewPatch->address_in_byte_array = Back->cur_addr;
    NewPatch->name = Name;
    NewPatch->data = NULL_UNIVERSAL_NUMBER;
    NewPatch->size_of_data = SizeOfData;

    put_in_array(Back->patches, &NewPatch);

    Back->cur_addr += SizeOfData;
}

void paste_patch (ElfBack* Back, SupportedPatches Name, UniversalNumber Data)
{
    Patch* Buffer = NULL;

    for (size_t cnt = 0; cnt != Back->patches->capacity - 1; ++cnt)
    {
        if ((char*) Back->patches->data + cnt * Back->patches->data_size)
        {
            take_from_array(Back->patches, &Buffer, cnt);
            MY_LOUD_ASSERT(Buffer != NULL);

            if (Buffer->name == Name)
            {
                Buffer->data = Data;

                memcpy (&(Back->ByteCodeArray[Buffer->address_in_byte_array]), &Data, Buffer->size_of_data);

                return;
            }
        }
    }

    MY_LOUD_ASSERT(false);
}

//==================================================================================================================================================================

void set_one_byte (ElfBack* Back, char Byte)
{
    // printf ("%p %lu\n", Back->ByteCodeArray, Back->cur_addr);
    // printf ("%p %d\n", Back->ByteCodeArray, Back->cur_addr);

    Back->ByteCodeArray[Back->cur_addr] = Byte;
    Back->cur_addr++;
}

void set_bytes (ElfBack* Back, void* Data, size_t Size)
{
    memcpy(&(Back->ByteCodeArray[Back->cur_addr]), Data, Size);
    Back->cur_addr += Size;
}

//==================================================================================================================================================================

void align_one_byte (ElfBack* Back, size_t Alignment)
{
    for (int cnt = 0; cnt != Alignment - 1; ++cnt)
    {
        set_one_byte(Back, 0x00);
    }
}

//==================================================================================================================================================================

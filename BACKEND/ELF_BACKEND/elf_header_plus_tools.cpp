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

void create_and_skip_patch (ElfBack* Back, size_t Address, SupportedPatches Name, size_t SizeOfData)
{
    Patch* NewPatch = (Patch*) calloc(1, sizeof (Patch));
    // printf (KRED "%p\n" KNRM, NewPatch);

    NewPatch->address_in_byte_array = Address;
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
            // pop_from_array(Back->patches, &Buffer, cnt);
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

// void create_and_skip_patch (SStack<Patch*>* Patches, size_t Address, SupportedPatches Name)
// {
//     Patch* NewPatch = (Patch*) calloc(1, sizeof (Patch));
//
//     NewPatch->address_in_byte_array = Address;
//     NewPatch->name = Name;
//     NewPatch->data = NULL_UNIVERSAL_NUMBER;
//
//     push_in_stack(Patches, NewPatch);
// }
//==================================================================================================================================================================

// void elf_head_constructor(ElfHead* Head)
// {
//     return;
// }

//==================================================================================================================================================================

void set_one_byte (ElfBack* Back, char Byte)
{
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

#define PASTE_8(x,y)  \
    x = Back->cur_addr;    \
    memcpy ((size_t*) &(Back->ByteCodeArray[(y)]), \
    &(x), sizeof (size_t));

#define PASTE_KNOWN_4(x)  \
    memcpy ((unsigned int*) &(Back->ByteCodeArray[(x)]), \
    &(x), sizeof (unsigned int));

#define PASTE_KNOWN_8(x)  \
    memcpy ((size_t*) &(Back->ByteCodeArray[(x)]), \
    &(x), sizeof (size_t));

#define PASTE_4(x,y)  \
    x = Back->cur_addr;    \
    memcpy ((unsigned int*) &(Back->ByteCodeArray[(y)]), \
    &(x), sizeof (unsigned int));

#define SKIP_8(x,y)   \
    size_t x = 0; \
    size_t y = Back->cur_addr;    \
    Back->cur_addr += 8; //we ll skip it now

#define SKIP_4(x,y)   \
    unsigned int x = 0; \
    size_t y = Back->cur_addr;    \
    Back->cur_addr += 4; //we ll skip it now

//==================================================================================================================================================================

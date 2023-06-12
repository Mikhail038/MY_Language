//==================================================================================================================================================================
//12.06.23
//==================================================================================================================================================================

#include "elf_header_plus_tools.h"
#include "elf_back.h"
#include <cstddef>

//==================================================================================================================================================================

void set_one_byte (ElfBack* Back, char Byte)
{
    Back->Array[Back->cur_addr] = Byte;
    Back->cur_addr++;
}

void set_bytes (ElfBack* Back, unsigned short int Number)
{
    *((unsigned short int*) &(Back->Array[Back->cur_addr])) = Number;
    Back->cur_addr += sizeof (Number);
}

void set_bytes (ElfBack* Back, unsigned int Number)
{
    *((unsigned int*) &(Back->Array[Back->cur_addr])) = Number;
    Back->cur_addr += sizeof (Number);
}

void set_bytes (ElfBack* Back, unsigned long int Number)
{
    *((unsigned long int*) &(Back->Array[Back->cur_addr])) = Number;
    Back->cur_addr += sizeof (Number);
}

//==================================================================================================================================================================

void align_one_byte (ElfBack* Back, size_t Alignment)
{
    for (int cnt = 0; cnt != Alignment - 1; ++cnt)
    {
        set_one_byte(Back, 0x00);
    }
}

// #define SET_BYTE(x) Back->Array[Back->cur_addr] = (x); Back->cur_addr++;

// #define SET_BYTE_2_BYTES(x)     \
//     *((unsigned short int*) &(Back->Array[Back->cur_addr])) = x; \
//     Back->cur_addr += 2;
//
// #define SET_BYTE_4_BYTES(x)     \
//     memcpy ((unsigned int*) &(Back->Array[Back->cur_addr]),    \
//                         &(x), sizeof (unsigned int));  \
//     Back->cur_addr += 4;
//
// #define SET_BYTE_8_BYTES(x)     \
//     memcpy ((size_t*) &(Back->Array[Back->cur_addr]),    \
//                         &(x), sizeof (size_t));  \
//     Back->cur_addr += 8;

#define ALIGN_ONE_BYTE(x) for (int cnt = 0; cnt != (x); ++cnt) { set_bytes(Back, 0x00); }

#define PASTE_8(x,y)  \
    x = Back->cur_addr;    \
    memcpy ((size_t*) &(Back->Array[(y)]), \
    &(x), sizeof (size_t));

#define PASTE_KNOWN_4(x)  \
    memcpy ((unsigned int*) &(Back->Array[(x)]), \
    &(x), sizeof (unsigned int));

#define PASTE_KNOWN_8(x)  \
    memcpy ((size_t*) &(Back->Array[(x)]), \
    &(x), sizeof (size_t));

#define PASTE_4(x,y)  \
    x = Back->cur_addr;    \
    memcpy ((unsigned int*) &(Back->Array[(y)]), \
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

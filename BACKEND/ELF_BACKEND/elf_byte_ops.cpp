//==================================================================================================================================================================
//15.05.2023
//==================================================================================================================================================================

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <ostream>
#include <stdlib.h>
#include <cstdio>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <locale.h>
#include <wchar.h>
#include <string.h>

#include "MYassert.h"
#include "colors.h"

//=============================================================================================================================================================================

#include "elf_back.h"

#include "back.h"
#include "front.h"


#include "asm.h"
#include "proc.h"
#include <bits/types/FILE.h>
#include <cstdio>

//=============================================================================================================================================================================

const int CALL_SIZE = 5;

const int NEAR_JUMP_SIZE = 2;

const int NORM_JUMP_SIZE = 6;

//=============================================================================================================================================================================
/*
#define SET_BYTE(x) Back->ByteCodeArray[Back->cur_addr] = (x); Back->cur_addr++;

#define SET_BYTE_2_BYTES(x)     \
    memcpy ((unsigned short int*) &(Back->ByteCodeArray[Back->cur_addr]),    \
                        &(x), sizeof (unsigned short int));  \
    Back->cur_addr += 2;

#define SET_BYTE_4_BYTES(x)     \
    memcpy ((unsigned int*) &(Back->ByteCodeArray[Back->cur_addr]),    \
                        &(x), sizeof (unsigned int));  \
    Back->cur_addr += 4;

#define SET_BYTE_8_BYTES(x)     \
    memcpy ((size_t*) &(Back->ByteCodeArray[Back->cur_addr]),    \
                        &(x), sizeof (size_t));  \
    Back->cur_addr += 8;
// */
//==================================================================================================================================================================

void set_hex_int (ElfBack* Back, int Number)
{
    set_bytes(Back, &Number, sizeof (Number));
}

void set_hex_long (ElfBack* Back, long Address)
{
    set_bytes(Back, &Address, sizeof (Address));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_push_imm (ElfBack* Back, int Number)
{
    if (Number <= 127)
    {
        set_one_byte(Back, 0x6a);

        set_one_byte(Back, (char) Number);

        return;
    }

    set_one_byte(Back, 0x68);

    set_hex_int (Back, Number);
}

void x86_push_reg (ElfBack* Back, int Register)
{
    if (Register >= r8)
    {
        set_one_byte(Back, 0x41);
        Register-= r8;
    }

    x86_extra_one_register_config(Back, Register, 0x50);
}

void x86_push_from_addr_in_reg (ElfBack* Back, int Register)
{
    if (Register >= r8)
    {
        set_one_byte(Back, 0x41);
        Register-= r8;
    }

    set_one_byte(Back, 0xff);

    x86_extra_one_register_config(Back, Register, 0x30);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_pop_to_reg (ElfBack* Back, int Register)
{
    if (Register >= r8)
    {
        set_one_byte(Back, 0x41);
        Register-= r8;
    }

    x86_extra_one_register_config(Back, Register, 0x58);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void x86_mov_reg_reg (ElfBack* Back, int dstReg, int srcReg)
{
    x86_extra_dest_src_config(Back, dstReg, srcReg);

    set_one_byte(Back, 0x89);

    x86_extra_two_registers_config(Back, dstReg, srcReg);
}

void x86_mov_reg_imm (ElfBack* Back, int dstReg, int Number)
{
    x86_extra_destination_config(Back, dstReg);

    set_one_byte(Back, 0xc7);

    x86_extra_one_register_config(Back, dstReg, 0xc0);

    set_hex_int(Back, Number);
}

void x86_mov_to_reg_from_addr_in_reg (ElfBack* Back, int dstReg, int srcReg)
{
    x86_extra_dest_src_config(Back, dstReg, srcReg);

    set_one_byte(Back, 0x8b);

    if ((srcReg == rsp) || (srcReg == r12))
    {
        char opcode = 0x04;
        opcode += (char) dstReg * 8;

        set_one_byte(Back, opcode);

        set_one_byte(Back, 0x24);

        return;
    }
    if ((srcReg == rbp) || (srcReg == r13))
    {
        char opcode = 0x45;
        opcode += (char) dstReg * 8;

        set_one_byte(Back, opcode);

        set_one_byte(Back, 0x00);

        return;
    }

    char opcode = 0x00;
    opcode += (char) dstReg;
    opcode += (char) srcReg * 8;

    set_one_byte(Back, opcode);
}

void x86_mov_to_addr_in_reg_from_reg (ElfBack* Back, int dstReg, int srcReg)
{
    x86_extra_dest_src_config(Back, dstReg, srcReg);

    set_one_byte(Back, 0x89);

    char opcode = 0x00;
    opcode += (char) dstReg;
    opcode += (char) srcReg * 8;

    set_one_byte(Back, opcode);
}

void x86_mov_to_reg_from_imm_addr (ElfBack* Back, int dstReg, int MemAddr)
{
    x86_extra_destination_config(Back, dstReg);

    set_one_byte(Back, 0x8b);

    char opcode = 0x05;
    opcode += (char) dstReg;

    set_one_byte(Back, opcode);

    set_hex_int(Back, MemAddr);
}

void x86_mov_to_reg_from_addr_in_reg_with_imm (ElfBack* Back, int dstReg, int srcReg, int Shift)
{
    const int BORDER_SHORT = 128;

    x86_extra_dest_src_config(Back, dstReg, srcReg);

    set_one_byte(Back, 0x8b);

    if ((srcReg == rsp) || (srcReg == r12))
    {
        char opcode = 0x44;
        opcode += (char) dstReg * 8;

        set_one_byte(Back, opcode);

        set_one_byte(Back, 0x24);

        if (Shift >= -BORDER_SHORT && Shift <= BORDER_SHORT)
        {
            set_one_byte(Back, (char) Shift);
        }
        else
        {
            set_hex_int(Back, Shift);
        }

        return;
    }
    if ((srcReg == rbp) || (srcReg == r13))
    {
        char opcode = 0x45;
        opcode += (char) dstReg * 8;

        set_one_byte(Back, opcode);

        if (Shift >= -BORDER_SHORT && Shift <= BORDER_SHORT)
        {
            set_one_byte(Back, (char) Shift);
        }
        else
        {
            set_hex_int(Back, Shift);
        }
        return;
    }

    if (Shift != 0)
    {
        char opcode = 0x40;
        opcode += (char) dstReg;
        opcode += (char) srcReg * 8;

        set_one_byte(Back, opcode);

        if (Shift >= -BORDER_SHORT && Shift <= BORDER_SHORT)
        {
            set_one_byte(Back, (char) Shift);
        }
        else
        {
            set_hex_int(Back, Shift);
        }
    }
    else
    {
        char opcode = 0x00;
        opcode += (char) dstReg;
        opcode += (char) srcReg * 8;

        set_one_byte(Back, opcode);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_add_reg_reg (ElfBack* Back, int dstReg, int SrcReg)
{
    x86_extra_dest_src_config(Back, dstReg, SrcReg);

    set_one_byte(Back, 0x01);

    x86_extra_two_registers_config(Back, dstReg, SrcReg);
}

void x86_add_stack (ElfBack* Back)
{
    int a_reg = FIRST_REG;
    int b_reg = SECOND_REG;

    x86_pop_to_reg(Back, b_reg);
    x86_pop_to_reg(Back, a_reg);

    x86_add_reg_reg (Back, a_reg, b_reg);

    x86_push_reg(Back, a_reg);
}

void x86_sub_reg_reg (ElfBack* Back, int dstReg, int SrcReg)
{
    x86_extra_dest_src_config(Back, dstReg, SrcReg);

    set_one_byte(Back, 0x29);

    x86_extra_two_registers_config(Back, dstReg, SrcReg);
}

void x86_sub_stack (ElfBack* Back)
{
    int a_reg = FIRST_REG;
    int b_reg = SECOND_REG;

    x86_pop_to_reg(Back, b_reg);
    x86_pop_to_reg(Back, a_reg);

    x86_sub_reg_reg(Back, a_reg, b_reg);

    x86_push_reg(Back, a_reg);
}

void x86_imul_stack (ElfBack* Back)
{
    int a_reg = FIRST_REG;

    x86_pop_to_reg(Back, a_reg);
    x86_pop_to_reg(Back, rax);

    x86_extra_destination_config(Back, a_reg);

    set_one_byte(Back, 0xf7);

    x86_extra_one_register_config(Back, a_reg, 0xe8);

    x86_push_reg(Back, rax);
}

void x86_idiv_stack (ElfBack* Back)
{
    x86_mov_reg_imm(Back, rdx, 0);

    int a_reg = FIRST_REG;

    x86_pop_to_reg(Back, a_reg);
    x86_pop_to_reg(Back, rax);

    x86_extra_destination_config(Back, a_reg);

    set_one_byte(Back, 0xf7);

    x86_extra_one_register_config(Back, a_reg, 0xf8);

    x86_push_reg(Back, rax);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_add_reg_imm (ElfBack* Back, int Register, int Number)
{
    x86_extra_destination_config(Back, Register);

    if (Number <= 127)
    {
        set_one_byte(Back, 0x83);
    }
    else
    {
        set_one_byte(Back, 0x81);
    }

    x86_extra_one_register_config(Back, Register, 0xc0);

    if (Number <= 127)
    {
        set_one_byte(Back, (char) Number);
    }
    else
    {
        set_hex_int(Back, Number);
    }
}

void x86_sub_reg_imm (ElfBack* Back, int Register, int Number)
{
    x86_extra_destination_config(Back, Register);

    if (Number <= 127)
    {
        set_one_byte(Back, 0x83);
    }
    else
    {
        set_one_byte(Back, 0x81);
    }

    x86_extra_one_register_config(Back, Register, 0xe8);

    if (Number <= 127)
    {
        set_one_byte(Back, (char) Number);
    }
    else
    {
        set_hex_int(Back, Number);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_inc (ElfBack* Back, int Register)
{
    x86_extra_destination_config(Back, Register);

    set_one_byte(Back, 0xff);

    x86_extra_one_register_config(Back, Register, 0xc0);
}

void x86_dec (ElfBack* Back, int Register)
{
    x86_extra_destination_config(Back, Register);

    set_one_byte(Back, 0xff);

    x86_extra_one_register_config(Back, Register, 0xc8);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_call (ElfBack* Back, int Shift)
{
    // std::cout << Shift << std::endl;

    set_one_byte(Back, 0xe8);

    set_hex_int(Back, Shift - CALL_SIZE);
}

void x86_call_label (ElfBack* Back, const wchar_t* Name)
{
    #ifdef LABEL_OVERSEER
    fprintf (stdout, "call <%ls>:\n", Name);
    #endif

    // if (Back->Labels.find(Name) != Labels.end())
    if (my_wchar_find(Back, Name) != NULL)
    {
        JumpLabel Label = Back->Labels[my_wchar_find(Back, Name)];

        if (Label.finish != NULL_FINISH)
        {
            // fprintf (stdout, LA  BEL "|%ls|:\n", Name);

            x86_call (Back, Label.finish - Back->cur_addr);
            // x86_call (Back->Labels[Name].finish);
            // x86_call (0);

        }
        else
        {
            Label.start[Label.amount] = Back->cur_addr;
            Label.amount++;
        }

        return;
    }
    else
    {
        #ifdef LABEL_OVERSEER
        fprintf (stdout, "<not found>:\n", Name);
        #endif

        Back->Labels.insert({Name, {Back->cur_addr, 0}});
    }

    for (size_t i = 0; i < CALL_SIZE; ++i)
    {
        x86_nop(Back);
    }
}

void x86_jump_label (ElfBack* Back, const wchar_t* Name, const int JumpMode)
{
    if (Back->Labels.find(Name) != Back->Labels.end())
    {
        if (Back->Labels[Name].finish != NULL_FINISH)
        {

            int JumpAddr = Back->Labels[Name].finish - Back->cur_addr;

            if (JumpMode == x86_jmp && JumpAddr < 0)
            {
                JumpAddr += 1;
            }

            x86_jump_norm (Back, JumpAddr, JumpMode); //norm is hardcoded here cause of not willing to fck with different operation size

            return;
        }
        else
        {
            Back->Labels[Name].start[Back->Labels[Name].amount] = Back->cur_addr;
            Back->Labels[Name].amount++;
        }
    }
    else
    {
        Back->Labels.insert({Name, {Back->cur_addr, 0}});
    }

    if (JumpMode == x86_jmp)
    {
        x86_nop(Back);
    }

    x86_jump_norm(Back, 0, JumpMode); // fake
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_nop (ElfBack* Back)
{
    set_one_byte(Back, 0x90);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_ret (ElfBack* Back)
{
    set_one_byte(Back, 0xc3);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_syscall (ElfBack* Back)
{
    set_one_byte(Back, 0x0f);
    set_one_byte(Back, 0x05);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_cmp_reg_reg (ElfBack* Back, int dstReg, int srcReg)
{
    x86_extra_dest_src_config(Back, dstReg, srcReg);

    set_one_byte(Back, 0x39);

    x86_extra_two_registers_config(Back, dstReg, srcReg);
}

void x86_cmp_stack (ElfBack* Back)
{
    x86_pop_to_reg(Back, SECOND_REG);
    x86_pop_to_reg(Back, FIRST_REG);

    x86_cmp_reg_reg (Back, FIRST_REG, SECOND_REG);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_jump_any (ElfBack* Back, int Shift, int JumpMode)
{
    if ((Shift < 100) && (Shift > -100))
    {
        x86_jump_near(Back, Shift, JumpMode);

        return;
    }
    else
    {
        x86_jump_norm(Back, Shift, JumpMode);
    }
}

void x86_jump_norm (ElfBack* Back, int Shift, int JumpMode)
{
    switch (JumpMode)
    {
        case x86_jmp:
            set_one_byte(Back, 0xe9);
            break;

        case x86_ja:
            set_one_byte(Back, 0x0f);
            set_one_byte(Back, 0x87);
            break;

        case x86_jae:
            set_one_byte(Back, 0x0f);
            set_one_byte(Back, 0x83);
            break;

        case x86_jb:
            set_one_byte(Back, 0x0f);
            set_one_byte(Back, 0x82);
            break;

        case x86_jbe:
            set_one_byte(Back, 0x0f);
            set_one_byte(Back, 0x86);
            break;

        case x86_je:
            set_one_byte(Back, 0x0f);
            set_one_byte(Back, 0x84);
            break;

        case x86_jne:
            set_one_byte(Back, 0x0f);
            set_one_byte(Back, 0x85);
            break;

        case x86_jl:
            set_one_byte(Back, 0x0f);
            set_one_byte(Back, 0x8c);
            break;

        case x86_jle:
            set_one_byte(Back, 0x0f);
            set_one_byte(Back, 0x8e);
            break;

        case x86_jg:
            set_one_byte(Back, 0x0f);
            set_one_byte(Back, 0x8f);
            break;

        case x86_jge:
            set_one_byte(Back, 0x0f);
            set_one_byte(Back, 0x8d);
            break;

        default:
            MY_LOUD_ASSERT(0);
    }

    // int current_jump_size = NORM_JUMP_SIZE;
    // if (JumpMode == x86_jmp)
    // {
    //     current_jump_size -= 1; // Stupid jumps have different size of opcode
    // }

    set_hex_int(Back, Shift - NORM_JUMP_SIZE);
}

void x86_jump_near (ElfBack* Back, int Shift, int JumpMode)
{
    switch (JumpMode)
    {
        case x86_jmp:
            set_one_byte(Back, 0xeb);
            break;

        case x86_ja:
            set_one_byte(Back, 0x77);
            break;

        case x86_jae:
            set_one_byte(Back, 0x73);
            break;

        case x86_jb:
            set_one_byte(Back, 0x72);
            break;

        case x86_jbe:
            set_one_byte(Back, 0x76);
            break;

        case x86_je:
            set_one_byte(Back, 0x74);
            break;

        case x86_jne:
            set_one_byte(Back, 0x75);
            break;

        case x86_jl:
            set_one_byte(Back, 0x7c);
            break;

        case x86_jle:
            set_one_byte(Back, 0x7e);
            break;

        case x86_jg:
            set_one_byte(Back, 0x7f);
            break;

        case x86_jge:
            set_one_byte(Back, 0x7d);
            break;

        default:
            MY_LOUD_ASSERT(0);
    }

    char Offset = 0x00;

    Offset += Shift;

    set_one_byte(Back, Offset - NEAR_JUMP_SIZE);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const int InputFunctionSize = 107;

const char InputFunctionCodeArr[] =
{
0x49, 0x89,
0xc6, 0x48, 0xc7, 0xc7, 0x00, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc2, 0x0a, 0x00, 0x00, 0x00,
0x48, 0x8d, 0x74, 0x24, 0x08, 0x48, 0xc7, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x05, 0x48,
0x31, 0xc0, 0x48, 0x8d, 0x74, 0x24, 0x08, 0x48, 0xc7, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x48,
0xc7, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x8a, 0x16, 0x80, 0xfa, 0x0a, 0x0f, 0x84, 0x27, 0x00,
0x00, 0x00, 0x48, 0x83, 0xea, 0x30, 0x52, 0x48, 0x89, 0xc3, 0x48, 0xc7, 0xc0, 0x0a, 0x00,
0x00, 0x00, 0x50, 0x48, 0x89, 0xd8, 0x50, 0x5b, 0x58, 0x48, 0xf7, 0xeb, 0x50, 0x58, 0x5a,
0x48, 0x01, 0xd0, 0x48, 0xff, 0xc6, 0xe9, 0xce, 0xff, 0xff, 0xff, 0x49, 0x89, 0x06, 0xc3
};

void x86_extra_make_input_func (ElfBack* Back)
{
    x86_extra_paste_call_label(Back, INP_LBL);

    memcpy (&(Back->ByteCodeArray[Back->cur_addr]), InputFunctionCodeArr, InputFunctionSize);

    Back->cur_addr += InputFunctionSize;

    /*
    x86_mov_reg_reg(Back, r14, rax);

    // mov rdi, 0x0      ; file descriptor = stdin = 0
    x86_mov_reg_imm(Back, rdi, 0);

    // mov rdx, 0xa      ; number of bytes to read
    x86_mov_reg_imm(Back, rdx, 0x0a);

    // lea rsi, [rsp+8]  ; buffer = address to store the bytes read
    set_one_byte(Back, 0x48);
    set_one_byte(Back, 0x8d);
    set_one_byte(Back, 0x74);
    set_one_byte(Back, 0x24);
    set_one_byte(Back, 0x08);

    // mov rax, 0x0      ; SYSCALL number for reading from STDIN
    x86_mov_reg_imm(Back, rax, 0);

    // syscall
    x86_syscall(Back);  //read

    // xor rax, rax      ; clear off rax
    set_one_byte(Back, 0x48);
    set_one_byte(Back, 0x31);
    set_one_byte(Back, 0xc0);

    // lea rsi, [rsp+8]  ; Get the address on the stack where the first ASCII byte of the integer is stored.
    set_one_byte(Back, 0x48);
    set_one_byte(Back, 0x8d);
    set_one_byte(Back, 0x74);
    set_one_byte(Back, 0x24);
    set_one_byte(Back, 0x08);

    // mov rax, 0x0      ; initialize the counter which stores the number of bytes in the string representation of the integer
    x86_mov_reg_imm(Back, rax, 0);

    x86_mov_reg_imm(Back, rbx, 0);

    x86_extra_make_input_func_body (Back);

    x86_mov_to_addr_in_reg_from_reg(Back, r14, rax);

    // x86_write_new_line();

    x86_ret(Back);
    */
}

void x86_extra_make_input_func_body (ElfBack* Back)
{
    x86_extra_paste_jump_label(Back, L"_in_convert");

    // mov dl, [rsi]
    set_one_byte(Back, 0x8a);
    set_one_byte(Back, 0x16);

    //cmp dl, 0x0a
    set_one_byte(Back, 0x80);
    set_one_byte(Back, 0xfa);
    set_one_byte(Back, 0x0a);

    x86_jump_label(Back, L"_in_done", x86_je);

    x86_sub_reg_imm(Back, rdx, '0');

    // and rdx, 0xff
    // set_one_byte(Back, 0x48);
    // set_one_byte(Back, 0x81);
    // set_one_byte(Back, 0xed);
    // set_one_byte(Back, 0xff);
    // set_one_byte(Back, 0x00);
    // set_one_byte(Back, 0x00);

    x86_push_reg(Back, rdx);

    x86_mov_reg_reg(Back, rbx, rax);

    // imul eax, 10
    x86_mov_reg_imm(Back, rax, 10);
    x86_push_reg(Back, rax);

    x86_mov_reg_reg(Back, rax, rbx);
    x86_push_reg(Back, rax);

    x86_imul_stack(Back);
    x86_pop_to_reg(Back, rax);

    x86_pop_to_reg(Back, rdx);
    // x86_sub_i(rdx, '0'); !!!

    x86_add_reg_reg(Back, rax, rdx);

    x86_inc(Back, rsi);
    x86_jump_label(Back, L"_in_convert", x86_jmp);
    // add eax, edx
    // inc esi
    // jmp convert

    x86_extra_paste_jump_label(Back, L"_in_done");
}

void x86_write_new_line (ElfBack* Back)
{
    // mov rdi, 0x1      ; file descriptor = stdout
    x86_mov_reg_imm(Back, rdi, 1);

    // mov rdx, 0x1      ; number of bytes to write
    x86_mov_reg_imm(Back, rdx, 0x01);

    // mov rax, 0xa      ; move the new line character to rax
    x86_mov_reg_imm(Back, rax, 0x0a);

    //  mov [rsp+8], rax  ; put this on the stack
    set_one_byte(Back, 0x48);
    set_one_byte(Back, 0x89);
    set_one_byte(Back, 0x44);
    set_one_byte(Back, 0x24);
    set_one_byte(Back, 0x08);

    // lea rsi, [rsp+8]  ; buffer = address to write to consol
    set_one_byte(Back, 0x48);
    set_one_byte(Back, 0x8d);
    set_one_byte(Back, 0x74);
    set_one_byte(Back, 0x24);
    set_one_byte(Back, 0x08);

    //  ; SYSCALL number for writing to STDOUT
    x86_mov_reg_imm(Back, rax, 0x01);

    // syscall
    x86_syscall(Back);  //write
}

const int OutputFunctionSize = 140;

const char OutputFunctionCodeArr[] =
{
0x48, 0xc7, 0xc2, 0x00, 0x00,
0x00, 0x00, 0x49, 0xc7, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc3, 0x0a, 0x00, 0x00,
0x00, 0x48, 0xf7, 0xf3, 0x48, 0x83, 0xc2, 0x30, 0x52, 0x48, 0xc7, 0xc2, 0x00, 0x00, 0x00,
0x00, 0x49, 0xff, 0xc0, 0x48, 0x39, 0xd0, 0x0f, 0x85, 0xe5, 0xff, 0xff, 0xff, 0x48, 0xc7,
0xc1, 0x00, 0x00, 0x00, 0x00, 0x49, 0x39, 0xc8, 0x0f, 0x8e, 0x23, 0x00, 0x00, 0x00, 0x48,
0xc7, 0xc2, 0x01, 0x00, 0x00, 0x00, 0x48, 0x89, 0xe6, 0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00,
0x00, 0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x05, 0x49, 0xff, 0xc8, 0x5b, 0xe9,
0xcd, 0xff, 0xff, 0xff, 0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc2, 0x01,
0x00, 0x00, 0x00, 0x48, 0xc7, 0xc0, 0x0a, 0x00, 0x00, 0x00, 0x48, 0x89, 0x44, 0x24, 0x08,
0x48, 0x8d, 0x74, 0x24, 0x08, 0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc3
};

void x86_extra_make_output_func (ElfBack* Back)
{
    x86_extra_paste_call_label(Back, OUT_LBL);

    memcpy (&(Back->ByteCodeArray[Back->cur_addr]), OutputFunctionCodeArr, OutputFunctionSize);

    Back->cur_addr += OutputFunctionSize;
    /*
    //  xor  rdx, rdx     ; Clear rdx which stores obtains a single digit of the number to convert to ASCII bytes
    x86_mov_reg_imm(Back, rdx, 0);

    //  mov  r8, 0x0      ; Initialize the counter containing the number of digits
    x86_mov_reg_imm(Back, r8, 0);

    // //  pop  rax          ; Pop the read number from the stack
    // x86_pop_r(rax);

    //  mov  rbx, 0xa     ; We store the divisor which is 10 for decimals (base-10) in rbx. rbx will be the divisor.
    x86_mov_reg_imm(Back, rbx, 0x0a);

    x86_extra_make_output_func_body (Back);

    x86_write_new_line(Back);

    x86_ret(Back);
    */
}

void x86_extra_make_output_func_body (ElfBack* Back)
{
    x86_extra_paste_jump_label(Back, L"_out_next");

    //  div  rbx          ; Divide the number in rdx:rax by rbx to get the remainder in rdx
    set_one_byte(Back, 0x48);
    set_one_byte(Back, 0xf7);
    set_one_byte(Back, 0xf3);

    //  add  rdx, 0x30    ; Add 0x30 to get the ASCII byte equivalent of the remainder which is the digit in the number to be written to display.
    x86_add_reg_imm(Back, rdx, '0');

    //  push rdx          ; Push this byte to the stack. We do this because, we get the individial digit bytes in reverse order. So to reverse the order we use the stack
    x86_push_reg(Back, rdx);

    //  xor  rdx, rdx     ; Clear rdx preparing it for next division
    x86_mov_reg_imm(Back, rdx, 0);

    //  inc  r8           ; Increment the digits counter
    x86_inc (Back, r8);

    //  cmp  rax, 0x0     ; Continue until the number becomes 0 when there are no more digits to write to the console.
    x86_cmp_reg_reg(Back, rax, rdx);

    //  jne  wnext        ; Loop until there aren't any more digits.
    x86_jump_label(Back, L"_out_next", x86_jne);

    // popnext:
    x86_extra_paste_jump_label(Back, L"_out_popnext");

    // cmp  r8, 0x0      ; Check if the counter which contains the number of digits to write is 0
    x86_mov_reg_imm (Back, rcx, 0);
    x86_cmp_reg_reg (Back, r8, rcx);

    // jle  endw         ; If so there are no more digits to write
    x86_jump_label(Back, L"_out_done", x86_jle);

    // mov  rdx, 0x1     ; number of bytes to write
    x86_mov_reg_imm (Back, rdx, 1);

    // mov  rsi, rsp     ; buffer = address to write to console
    x86_mov_reg_reg (Back, rsi, rsp);

    // mov  rdi, 0x1     ; file descriptor = stdout
    x86_mov_reg_imm (Back, rdi, 1);

    // mov  rax, 0x1     ; SYSCALL number for writing to STDOUT
    x86_mov_reg_imm (Back, rax, 1);

    // syscall           ; make the syscall
    x86_syscall(Back);

    // dec  r8           ; Decrement the counter
    x86_dec(Back, r8);

    // pop  rbx          ; Pop the current digit that was already written to the display preparing the stack pointer for next digit.
    x86_pop_to_reg(Back, rbx);

    // jmp  popnext      ; Loop until the counter which contains the number of digits goes down to 0.
    x86_jump_label(Back, L"_out_popnext", x86_jmp);

    x86_extra_paste_jump_label(Back, L"_out_done");
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_extra_paste_call_label (ElfBack* Back, const wchar_t* Name)
{
    #ifdef LABEL_OVERSEER
    fprintf (stdout, "paste |%ls|:\n", Name);
    #endif

    // if (my_find (Name) == true)
    // if (Back->Labels.find(Name) != Back->Labels.end())
    if (my_wchar_find (Back, Name) != NULL)
    {
        JumpLabel Label = Back->Labels[my_wchar_find(Back, Name)];

        Label.finish = Back->cur_addr;

        for (size_t i = 0; i < Label.amount; ++i)
        {
            Back->cur_addr = Label.start[i];   // go there

            // fprintf (stdout, LABEL "|%ls|:\n", Name);

            //std::cout << Back->Labels[Name].finish << std::endl << Back->Labels[Name].start[i] << std::endl;
            x86_call (Back, Label.finish - Label.start[i]); //paste code

            // x86_call (Back->Labels[Name].finish); //paste code

            Back->cur_addr = Label.finish;  // go back
        }
    }
    else
    {
        #ifdef LABEL_OVERSEER
        fprintf (stdout, "|not called|:\n", Name);
        #endif

        Back->Labels.insert({Name, {Back->cur_addr}});
    }
}

void x86_extra_paste_jump_label (ElfBack* Back, const wchar_t* Name)
{
    if (Back->Labels.find(Name) != Back->Labels.end())
    {
        Back->Labels[Name].finish = Back->cur_addr;

        for (size_t i = 0; i < Back->Labels[Name].amount; ++i)
        {
            Back->cur_addr = Back->Labels[Name].start[i];   // go there

            //std::cout << Labels[Name].finish << std::endl << Labels[Name].start[i] << std::endl;

            Back->cur_addr += 2; //skip JUMP syntax

            set_hex_int(Back, Back->Labels[Name].finish - Back->Labels[Name].start[i] - NORM_JUMP_SIZE); //paste addr

            Back->cur_addr = Back->Labels[Name].finish;  // go back
        }
    }
    else
    {
        Back->Labels.insert({Name, {Back->cur_addr}});
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_extra_dest_src_config (ElfBack* Back, int& dstReg, int& srcReg)
{
    if ((srcReg >= r8) && (dstReg >= r8))
    {
        set_one_byte(Back, 0x4d);
        srcReg-= r8;
        dstReg-= r8;
    }
    else if (srcReg >= r8)
    {
        set_one_byte(Back, 0x4c);
        srcReg-= r8;
    }
    else if (dstReg >= r8)
    {
        set_one_byte(Back, 0x49);
        dstReg-= r8;
    }
    else
    {
        set_one_byte(Back, 0x48);
    }
}

void x86_extra_destination_config (ElfBack* Back, int& dstReg)
{
    if (dstReg >= r8)
    {
        set_one_byte(Back, 0x49);
        dstReg-= r8;
    }
    else
    {
        set_one_byte(Back, 0x48);
    }
}

void x86_extra_two_registers_config(ElfBack* Back, const int dstReg, const int srcReg)
{
    char opcode = (char) 0xc0;
    opcode += (char) dstReg;
    opcode += (char) (srcReg * 8);

    set_one_byte(Back, opcode);
}

void x86_extra_one_register_config(ElfBack* Back, const int Reg, const int ZeroPoint)
{
    char opcode = (char) ZeroPoint;
    opcode += (char) Reg;

    set_one_byte(Back, opcode);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void x86_extra_exit (ElfBack* Back)
{
    x86_mov_reg_imm(Back, rax, 0x3c);
    x86_mov_reg_imm(Back, rdi, 0);

    x86_syscall (Back);
}

const wchar_t* my_wchar_find (ElfBack* Back, const wchar_t* Name)
{
    for (auto iterator = Back->Labels.begin(); iterator != Back->Labels.end(); ++iterator)
    {
        if (wcscoll(Name, iterator->first) == 0)
        {
            return iterator->first;
        }
    }

    return NULL;
}

//=============================================================================================================================================================================


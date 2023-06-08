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

#define CALL_SIZE 5

#define NEAR_JUMP_SIZE 2

#define NORM_JUMP_SIZE 6

//=============================================================================================================================================================================

void ElfBack::set_hex_int (int Number)
{
    SET_BYTE_4_BYTES(Number);
}

void ElfBack::set_hex_long (long Address)
{
    SET_BYTE_8_BYTES(Address);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ElfBack::x86_push_imm (int Number)
{
    if (Number <= 127)
    {
        SET_BYTE(0x6a);

        SET_BYTE((char) Number);

        return;
    }

    SET_BYTE(0x68);

    set_hex_int (Number);
}

void ElfBack::x86_push_reg (int Register)
{
    if (Register >= DELTA)
    {
        SET_BYTE(0x41);
        Register-= DELTA;
    }

    x86_extra_one_register_config(Register, 0x50);
}

void ElfBack::x86_push_from_addr_in_reg (int Register)
{
    if (Register >= DELTA)
    {
        SET_BYTE(0x41);
        Register-= DELTA;
    }

    SET_BYTE(0xff);

    x86_extra_one_register_config(Register, 0x30);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ElfBack::x86_pop_to_reg (int Register)
{
    if (Register >= DELTA)
    {
        SET_BYTE(0x41);
        Register-= DELTA;
    }

    x86_extra_one_register_config(Register, 0x58);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void ElfBack::x86_mov_reg_reg (int dstReg, int srcReg)
{
    x86_extra_dest_src_config(dstReg, srcReg);

    SET_BYTE(0x89);

    x86_extra_two_registers_config(dstReg, srcReg);
}

void ElfBack::x86_mov_reg_imm (int dstReg, int Number)
{
    x86_extra_destination_config(dstReg);

    SET_BYTE(0xc7);

    x86_extra_one_register_config(dstReg, 0xc0);

    set_hex_int(Number);
}

void ElfBack::x86_mov_to_reg_from_addr_in_reg (int dstReg, int srcReg)
{
    x86_extra_dest_src_config(dstReg, srcReg);

    SET_BYTE(0x8b);

    if ((srcReg == rsp) || (srcReg == r12))
    {
        char opcode = 0x04;
        opcode += (char) dstReg * 8;

        SET_BYTE(opcode);

        SET_BYTE(0x24);

        return;
    }
    if ((srcReg == rbp) || (srcReg == r13))
    {
        char opcode = 0x45;
        opcode += (char) dstReg * 8;

        SET_BYTE(opcode);

        SET_BYTE(0x00);

        return;
    }

    char opcode = 0x00;
    opcode += (char) dstReg;
    opcode += (char) srcReg * 8;

    SET_BYTE(opcode);
}

void ElfBack::x86_mov_to_addr_in_reg_from_reg (int dstReg, int srcReg)
{
    x86_extra_dest_src_config(dstReg, srcReg);

    SET_BYTE(0x89);

    char opcode = 0x00;
    opcode += (char) dstReg;
    opcode += (char) srcReg * 8;

    SET_BYTE(opcode);
}

void ElfBack::x86_mov_to_reg_from_imm_addr (int dstReg, int MemAddr)
{
    x86_extra_destination_config(dstReg);

    SET_BYTE(0x8b);

    char opcode = 0x05;
    opcode += (char) dstReg;

    SET_BYTE(opcode);

    set_hex_int(MemAddr);
}

void ElfBack::x86_mov_to_reg_from_addr_in_reg_with_imm (int dstReg, int srcReg, int Shift)
{
    const int BORDER_SHORT = 128;

    x86_extra_dest_src_config(dstReg, srcReg);

    SET_BYTE(0x8b);

    if ((srcReg == rsp) || (srcReg == r12))
    {
        char opcode = 0x44;
        opcode += (char) dstReg * 8;

        SET_BYTE(opcode);

        SET_BYTE(0x24);

        if (Shift >= -BORDER_SHORT && Shift <= BORDER_SHORT)
        {
            SET_BYTE((char) Shift);
        }
        else
        {
            set_hex_int(Shift);
        }

        return;
    }
    if ((srcReg == rbp) || (srcReg == r13))
    {
        char opcode = 0x45;
        opcode += (char) dstReg * 8;

        SET_BYTE(opcode);

        if (Shift >= -BORDER_SHORT && Shift <= BORDER_SHORT)
        {
            SET_BYTE((char) Shift);
        }
        else
        {
            set_hex_int(Shift);
        }
        return;
    }

    if (Shift != 0)
    {
        char opcode = 0x40;
        opcode += (char) dstReg;
        opcode += (char) srcReg * 8;

        SET_BYTE(opcode);

        if (Shift >= -BORDER_SHORT && Shift <= BORDER_SHORT)
        {
            SET_BYTE((char) Shift);
        }
        else
        {
            set_hex_int(Shift);
        }
    }
    else
    {
        char opcode = 0x00;
        opcode += (char) dstReg;
        opcode += (char) srcReg * 8;

        SET_BYTE(opcode);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ElfBack::x86_add_reg_reg (int dstReg, int SrcReg)
{
    x86_extra_dest_src_config(dstReg, SrcReg);

    SET_BYTE(0x01);

    x86_extra_two_registers_config(dstReg, SrcReg);
}

void ElfBack::x86_add_stack ()
{
    int a_reg = FIRST_REG;
    int b_reg = SECOND_REG;

    x86_pop_to_reg(b_reg);
    x86_pop_to_reg(a_reg);

    x86_add_reg_reg (a_reg, b_reg);

    x86_push_reg(a_reg);
}

void ElfBack::x86_sub_reg_reg (int dstReg, int SrcReg)
{
    x86_extra_dest_src_config(dstReg, SrcReg);

    SET_BYTE(0x29);

    x86_extra_two_registers_config(dstReg, SrcReg);
}

void ElfBack::x86_sub_stack ()
{
    int a_reg = FIRST_REG;
    int b_reg = SECOND_REG;

    x86_pop_to_reg(b_reg);
    x86_pop_to_reg(a_reg);

    x86_sub_reg_reg(a_reg, b_reg);

    x86_push_reg(a_reg);
}

void ElfBack::x86_imul_stack ()
{
    int a_reg = FIRST_REG;

    x86_pop_to_reg(a_reg);
    x86_pop_to_reg(rax);

    x86_extra_destination_config(a_reg);

    SET_BYTE(0xf7);

    x86_extra_one_register_config(a_reg, 0xe8);

    x86_push_reg(rax);
}

void ElfBack::x86_idiv_stack ()
{
    x86_mov_reg_imm(rdx, 0);

    int a_reg = FIRST_REG;

    x86_pop_to_reg(a_reg);
    x86_pop_to_reg(rax);

    x86_extra_destination_config(a_reg);

    SET_BYTE(0xf7);

    x86_extra_one_register_config(a_reg, 0xf8);

    x86_push_reg(rax);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ElfBack::x86_add_reg_imm (int Register, int Number)
{
    x86_extra_destination_config(Register);

    if (Number <= 127)
    {
        SET_BYTE(0x83);
    }
    else
    {
        SET_BYTE(0x81);
    }

    x86_extra_one_register_config(Register, 0xc0);

    if (Number <= 127)
    {
        SET_BYTE((char) Number);
    }
    else
    {
        set_hex_int(Number);
    }
}

void ElfBack::x86_sub_reg_imm (int Register, int Number)
{
    x86_extra_destination_config(Register);

    if (Number <= 127)
    {
        SET_BYTE(0x83);
    }
    else
    {
        SET_BYTE(0x81);
    }

    x86_extra_one_register_config(Register, 0xe8);

    if (Number <= 127)
    {
        SET_BYTE((char) Number);
    }
    else
    {
        set_hex_int(Number);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ElfBack::x86_inc (int Register)
{
    x86_extra_destination_config(Register);

    SET_BYTE(0xff);

    x86_extra_one_register_config(Register, 0xc0);
}

void ElfBack::x86_dec (int Register)
{
    x86_extra_destination_config(Register);

    SET_BYTE(0xff);

    x86_extra_one_register_config(Register, 0xc8);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ElfBack::x86_call (int Shift)
{
    // std::cout << Shift << std::endl;

    SET_BYTE(0xe8);

    set_hex_int(Shift - CALL_SIZE);
}

void ElfBack::x86_call_label (const wchar_t* Name)
{
    #ifdef LABEL_OVERSEER
    fprintf (stdout, "call <%ls>:\n", Name);
    #endif

    // if (Labels.find(Name) != Labels.end())
    if (my_find(Name) != NULL)
    {
        TLabel Label = Labels[my_find(Name)];

        if (Label.finish != NULL_FINISH)
        {
            // fprintf (stdout, LA  BEL "|%ls|:\n", Name);

            x86_call (Label.finish - cur_addr);
            // x86_call (Labels[Name].finish);
            // x86_call (0);

        }
        else
        {
            Label.start[Label.amount] = cur_addr;
            Label.amount++;
        }

        return;
    }
    else
    {
        #ifdef LABEL_OVERSEER
        fprintf (stdout, "<not found>:\n", Name);
        #endif

        Labels.insert({Name, {cur_addr, 0}});
    }

    for (size_t i = 0; i < CALL_SIZE; ++i)
    {
        x86_nop();
    }
}

void ElfBack::x86_jump_label (const wchar_t* Name, const int JumpMode)
{
    // if (my_find (Name) == true)
    if (Labels.find(Name) != Labels.end())
    {
        if (Labels[Name].finish != NULL_FINISH)
        {

            int JumpAddr = Labels[Name].finish - cur_addr;

            if (JumpMode == x86_jmp && JumpAddr < 0)
            {
                JumpAddr += 1;
            }

            x86_jump_norm (JumpAddr, JumpMode); //norm is hardcoded here cause of not willing to fck with different operation size

            return;
        }
        else
        {
            Labels[Name].start[Labels[Name].amount] = cur_addr;
            Labels[Name].amount++;
        }
    }
    else
    {
        Labels.insert({Name, {cur_addr, 0}});
    }

    if (JumpMode == x86_jmp)
    {
        x86_nop();
    }

    x86_jump_norm(0, JumpMode); // fake
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ElfBack::x86_nop ()
{
    SET_BYTE(0x90);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ElfBack::x86_ret ()
{
    SET_BYTE (0xc3);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ElfBack::x86_syscall ()
{
    SET_BYTE (0x0f);
    SET_BYTE (0x05);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ElfBack::x86_cmp_reg_reg (int dstReg, int srcReg)
{
    x86_extra_dest_src_config(dstReg, srcReg);

    SET_BYTE(0x39);

    x86_extra_two_registers_config(dstReg, srcReg);
}

void ElfBack::x86_cmp_stack ()
{
    x86_pop_to_reg(SECOND_REG);
    x86_pop_to_reg(FIRST_REG);

    x86_cmp_reg_reg (FIRST_REG, SECOND_REG);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ElfBack::x86_jump_any (int Shift, int JumpMode)
{
    if ((Shift < 100) && (Shift > -100))
    {
        x86_jump_near(Shift, JumpMode);

        return;
    }
    else
    {
        x86_jump_norm(Shift, JumpMode);
    }
}

void ElfBack::x86_jump_norm (int Shift, int JumpMode)
{
    switch (JumpMode)
    {
        case x86_jmp:
            SET_BYTE(0xe9);
            break;

        case x86_ja:
            SET_BYTE(0x0f);
            SET_BYTE(0x87);
            break;

        case x86_jae:
            SET_BYTE(0x0f);
            SET_BYTE(0x83);
            break;

        case x86_jb:
            SET_BYTE(0x0f);
            SET_BYTE(0x82);
            break;

        case x86_jbe:
            SET_BYTE(0x0f);
            SET_BYTE(0x86);
            break;

        case x86_je:
            SET_BYTE(0x0f);
            SET_BYTE(0x84);
            break;

        case x86_jne:
            SET_BYTE(0x0f);
            SET_BYTE(0x85);
            break;

        case x86_jl:
            SET_BYTE(0x0f);
            SET_BYTE(0x8c);
            break;

        case x86_jle:
            SET_BYTE(0x0f);
            SET_BYTE(0x8e);
            break;

        case x86_jg:
            SET_BYTE(0x0f);
            SET_BYTE(0x8f);
            break;

        case x86_jge:
            SET_BYTE(0x0f);
            SET_BYTE(0x8d);
            break;

        default:
            MY_LOUD_ASSERT(0);
    }

    // int current_jump_size = NORM_JUMP_SIZE;
    // if (JumpMode == x86_jmp)
    // {
    //     current_jump_size -= 1; // Stupid jumps have different size of opcode
    // }

    set_hex_int(Shift - NORM_JUMP_SIZE);
}

void ElfBack::x86_jump_near (int Shift, int JumpMode)
{
    switch (JumpMode)
    {
        case x86_jmp:
            SET_BYTE(0xeb);
            break;

        case x86_ja:
            SET_BYTE(0x77);
            break;

        case x86_jae:
            SET_BYTE(0x73);
            break;

        case x86_jb:
            SET_BYTE(0x72);
            break;

        case x86_jbe:
            SET_BYTE(0x76);
            break;

        case x86_je:
            SET_BYTE(0x74);
            break;

        case x86_jne:
            SET_BYTE(0x75);
            break;

        case x86_jl:
            SET_BYTE(0x7c);
            break;

        case x86_jle:
            SET_BYTE(0x7e);
            break;

        case x86_jg:
            SET_BYTE(0x7f);
            break;

        case x86_jge:
            SET_BYTE(0x7d);
            break;

        default:
            MY_LOUD_ASSERT(0);
    }

    char offset = 0x00;

    offset += Shift;

    SET_BYTE(offset - NEAR_JUMP_SIZE);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ElfBack::x86_extra_make_input_func ()
{
    x86_extra_paste_call_label(INP_LBL);

    x86_mov_reg_reg(r14, rax);

    // mov rdi, 0x0      ; file descriptor = stdin = 0
    x86_mov_reg_imm(rdi, 0);

    // mov rdx, 0xa      ; number of bytes to read
    x86_mov_reg_imm(rdx, 0x0a);

    // lea rsi, [rsp+8]  ; buffer = address to store the bytes read
    SET_BYTE(0x48);
    SET_BYTE(0x8d);
    SET_BYTE(0x74);
    SET_BYTE(0x24);
    SET_BYTE(0x08);

    // mov rax, 0x0      ; SYSCALL number for reading from STDIN
    x86_mov_reg_imm(rax, 0);

    // syscall
    x86_syscall();  //read

    // xor rax, rax      ; clear off rax
    SET_BYTE(0x48);
    SET_BYTE(0x31);
    SET_BYTE(0xc0);

    // lea rsi, [rsp+8]  ; Get the address on the stack where the first ASCII byte of the integer is stored.
    SET_BYTE(0x48);
    SET_BYTE(0x8d);
    SET_BYTE(0x74);
    SET_BYTE(0x24);
    SET_BYTE(0x08);

    // mov rax, 0x0      ; initialize the counter which stores the number of bytes in the string representation of the integer
    x86_mov_reg_imm(rax, 0);

    x86_mov_reg_imm(rbx, 0);

    x86_extra_make_input_func_body ();

    x86_mov_to_addr_in_reg_from_reg(r14, rax);

    // x86_write_new_line();

    x86_ret();
}

void ElfBack::x86_extra_make_input_func_body ()
{
    x86_extra_paste_jump_label(L"_in_convert");

    // mov dl, [rsi]
    SET_BYTE(0x8a);
    SET_BYTE(0x16);

    //cmp dl, 0x0a
    SET_BYTE(0x80);
    SET_BYTE(0xfa);
    SET_BYTE(0x0a);

    x86_jump_label(L"_in_done", x86_je);

    x86_sub_reg_imm(rdx, '0');

    // and rdx, 0xff
    // SET_BYTE(0x48);
    // SET_BYTE(0x81);
    // SET_BYTE(0xed);
    // SET_BYTE(0xff);
    // SET_BYTE(0x00);
    // SET_BYTE(0x00);

    x86_push_reg(rdx);

    x86_mov_reg_reg(rbx, rax);

    // imul eax, 10
    x86_mov_reg_imm(rax, 10);
    x86_push_reg(rax);

    x86_mov_reg_reg(rax, rbx);
    x86_push_reg(rax);

    x86_imul_stack();
    x86_pop_to_reg(rax);

    x86_pop_to_reg(rdx);
    // x86_sub_i(rdx, '0'); !!!

    x86_add_reg_reg(rax, rdx);

    x86_inc(rsi);
    x86_jump_label(L"_in_convert", x86_jmp);
    // add eax, edx
    // inc esi
    // jmp convert

    x86_extra_paste_jump_label(L"_in_done");
}

void ElfBack::x86_write_new_line ()
{
    // mov rdi, 0x1      ; file descriptor = stdout
    x86_mov_reg_imm(rdi, 1);

    // mov rdx, 0x1      ; number of bytes to write
    x86_mov_reg_imm(rdx, 0x01);

    // mov rax, 0xa      ; move the new line character to rax
    x86_mov_reg_imm(rax, 0x0a);

    //  mov [rsp+8], rax  ; put this on the stack
    SET_BYTE(0x48);
    SET_BYTE(0x89);
    SET_BYTE(0x44);
    SET_BYTE(0x24);
    SET_BYTE(0x08);

    // lea rsi, [rsp+8]  ; buffer = address to write to consol
    SET_BYTE(0x48);
    SET_BYTE(0x8d);
    SET_BYTE(0x74);
    SET_BYTE(0x24);
    SET_BYTE(0x08);

    //  ; SYSCALL number for writing to STDOUT
    x86_mov_reg_imm(rax, 0x01);

    // syscall
    x86_syscall();  //write
}

void ElfBack::x86_extra_make_output_func ()
{
    x86_extra_paste_call_label(OUT_LBL);

    //  xor  rdx, rdx     ; Clear rdx which stores obtains a single digit of the number to convert to ASCII bytes
    x86_mov_reg_imm(rdx, 0);

    //  mov  r8, 0x0      ; Initialize the counter containing the number of digits
    x86_mov_reg_imm(r8, 0);

    // //  pop  rax          ; Pop the read number from the stack
    // x86_pop_r(rax);

    //  mov  rbx, 0xa     ; We store the divisor which is 10 for decimals (base-10) in rbx. rbx will be the divisor.
    x86_mov_reg_imm(rbx, 0x0a);

    x86_extra_make_output_func_body ();

    x86_write_new_line();

    x86_ret();
}

void ElfBack::x86_extra_make_output_func_body ()
{
    x86_extra_paste_jump_label(L"_out_next");

    //  div  rbx          ; Divide the number in rdx:rax by rbx to get the remainder in rdx
    SET_BYTE(0x48);
    SET_BYTE(0xf7);
    SET_BYTE(0xf3);

    //  add  rdx, 0x30    ; Add 0x30 to get the ASCII byte equivalent of the remainder which is the digit in the number to be written to display.
    x86_add_reg_imm(rdx, '0');

    //  push rdx          ; Push this byte to the stack. We do this because, we get the individial digit bytes in reverse order. So to reverse the order we use the stack
    x86_push_reg(rdx);

    //  xor  rdx, rdx     ; Clear rdx preparing it for next division
    x86_mov_reg_imm(rdx, 0);

    //  inc  r8           ; Increment the digits counter
    x86_inc (r8);

    //  cmp  rax, 0x0     ; Continue until the number becomes 0 when there are no more digits to write to the console.
    x86_cmp_reg_reg(rax, rdx);

    //  jne  wnext        ; Loop until there aren't any more digits.
    x86_jump_label(L"_out_next", x86_jne);

    // popnext:
    x86_extra_paste_jump_label(L"_out_popnext");

    // cmp  r8, 0x0      ; Check if the counter which contains the number of digits to write is 0
    x86_mov_reg_imm (rcx, 0);
    x86_cmp_reg_reg (r8, rcx);

    // jle  endw         ; If so there are no more digits to write
    x86_jump_label(L"_out_done", x86_jle);

    // mov  rdx, 0x1     ; number of bytes to write
    x86_mov_reg_imm (rdx, 1);

    // mov  rsi, rsp     ; buffer = address to write to console
    x86_mov_reg_reg (rsi, rsp);

    // mov  rdi, 0x1     ; file descriptor = stdout
    x86_mov_reg_imm (rdi, 1);

    // mov  rax, 0x1     ; SYSCALL number for writing to STDOUT
    x86_mov_reg_imm (rax, 1);

    // syscall           ; make the syscall
    x86_syscall();

    // dec  r8           ; Decrement the counter
    x86_dec(r8);

    // pop  rbx          ; Pop the current digit that was already written to the display preparing the stack pointer for next digit.
    x86_pop_to_reg(rbx);

    // jmp  popnext      ; Loop until the counter which contains the number of digits goes down to 0.
    x86_jump_label(L"_out_popnext", x86_jmp);

    x86_extra_paste_jump_label(L"_out_done");
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ElfBack::x86_extra_paste_call_label (const wchar_t* Name)
{
    #ifdef LABEL_OVERSEER
    fprintf (stdout, "paste |%ls|:\n", Name);
    #endif

    // if (my_find (Name) == true)
    // if (Labels.find(Name) != Labels.end())
    if (my_find (Name) != NULL)
    {
        TLabel Label = Labels[my_find(Name)];

        Label.finish = cur_addr;

        for (size_t i = 0; i < Label.amount; ++i)
        {
            cur_addr = Label.start[i];   // go there

            // fprintf (stdout, LABEL "|%ls|:\n", Name);

            //std::cout << Labels[Name].finish << std::endl << Labels[Name].start[i] << std::endl;
            x86_call (Label.finish - Label.start[i]); //paste code

            // x86_call (Labels[Name].finish); //paste code

            cur_addr = Label.finish;  // go back
        }
    }
    else
    {
        #ifdef LABEL_OVERSEER
        fprintf (stdout, "|not called|:\n", Name);
        #endif

        Labels.insert({Name, {cur_addr}});
    }
}

void ElfBack::x86_extra_paste_jump_label (const wchar_t* Name)
{
    if (Labels.find(Name) != Labels.end())
    {
        Labels[Name].finish = cur_addr;

        for (size_t i = 0; i < Labels[Name].amount; ++i)
        {
            cur_addr = Labels[Name].start[i];   // go there

            //std::cout << Labels[Name].finish << std::endl << Labels[Name].start[i] << std::endl;

            cur_addr += 2; //skip JUMP syntax

            set_hex_int(Labels[Name].finish - Labels[Name].start[i] - NORM_JUMP_SIZE); //paste addr

            cur_addr = Labels[Name].finish;  // go back
        }
    }
    else
    {
        Labels.insert({Name, {cur_addr}});
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ElfBack::x86_extra_dest_src_config (int& dstReg, int& srcReg)
{
    if ((srcReg >= DELTA) && (dstReg >= DELTA))
    {
        SET_BYTE(0x4d);
        srcReg-= DELTA;
        dstReg-= DELTA;
    }
    else if (srcReg >= DELTA)
    {
        SET_BYTE(0x4c);
        srcReg-= DELTA;
    }
    else if (dstReg >= DELTA)
    {
        SET_BYTE(0x49);
        dstReg-= DELTA;
    }
    else
    {
        SET_BYTE(0x48);
    }
}

void ElfBack::x86_extra_destination_config (int& dstReg)
{
    if (dstReg >= DELTA)
    {
        SET_BYTE(0x49);
        dstReg-= DELTA;
    }
    else
    {
        SET_BYTE(0x48);
    }
}

void ElfBack::x86_extra_two_registers_config(const int dstReg, const int srcReg)
{
    char opcode = 0xc0;
    opcode += (char) dstReg;
    opcode += (char) srcReg * 8;

    SET_BYTE(opcode);
}

void ElfBack::x86_extra_one_register_config(const int Reg, const int ZeroPoint)
{
    char opcode = ZeroPoint;
    opcode += (char) Reg;

    SET_BYTE(opcode);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ElfBack::x86_extra_exit ()
{
    x86_mov_reg_imm(rax, 0x3c);
    x86_mov_reg_imm(rdi, 0);

    x86_syscall ();
}

const wchar_t* ElfBack::my_find (const wchar_t* Name)
{
    for (auto iterator = Labels.begin(); iterator != Labels.end(); ++iterator)
    {
        if (wcscoll(Name, iterator->first) == 0)
        {
            return iterator->first;
        }
    }

    return NULL;
}

//=============================================================================================================================================================================


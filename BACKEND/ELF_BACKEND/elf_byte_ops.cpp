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

void SElfBack::set_hex_int (int Number)
{
    SET_4(Number);
}

void SElfBack::set_hex_long (long Address)
{
    SET_8(Address);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_push_i (int Number)
{
    if (Number <= 127)
    {
        SET(0x6a);

        SET((char) Number);

        return;
    }

    SET(0x68);

    set_hex_int (Number);
}

void SElfBack::x86_push_r (int Register)
{
    if (Register >= DELTA)
    {
        SET(0x41);
        Register-= DELTA;
    }

    x86___Reg_config(Register, 0x50);
}

void SElfBack::x86_push_IrI (int Register)
{
    if (Register >= DELTA)
    {
        SET(0x41);
        Register-= DELTA;
    }

    SET(0xff);

    x86___Reg_config(Register, 0x30);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_pop_r (int Register)
{
    if (Register >= DELTA)
    {
        SET(0x41);
        Register-= DELTA;
    }

    x86___Reg_config(Register, 0x58);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void SElfBack::x86_mov_r_r (int dstReg, int srcReg)
{
    x86___DstSrc_config(dstReg, srcReg);

    SET(0x89);

    x86___Regs_config(dstReg, srcReg);
}

void SElfBack::x86_mov_r_i (int dstReg, int Number)
{
    x86___Dst_config(dstReg);

    SET(0xc7);

    x86___Reg_config(dstReg, 0xc0);

    set_hex_int(Number);
}

void SElfBack::x86_mov_r_IrI (int dstReg, int srcReg)
{
    x86___DstSrc_config(dstReg, srcReg);

    SET(0x8b);

    if ((srcReg == rsp) || (srcReg == r12))
    {
        char opcode = 0x04;
        opcode += (char) dstReg * 8;

        SET(opcode);

        SET(0x24);

        return;
    }
    if ((srcReg == rbp) || (srcReg == r13))
    {
        char opcode = 0x45;
        opcode += (char) dstReg * 8;

        SET(opcode);

        SET(0x00);

        return;
    }

    char opcode = 0x00;
    opcode += (char) dstReg;
    opcode += (char) srcReg * 8;

    SET(opcode);
}

void SElfBack::x86_mov_IrI_r (int dstReg, int srcReg)
{
    x86___DstSrc_config(dstReg, srcReg);

    SET(0x89);

    char opcode = 0x00;
    opcode += (char) dstReg;
    opcode += (char) srcReg * 8;

    SET(opcode);
}

#define BORDER_SHORT 128

void SElfBack::x86_mov_r_Ir_iI (int dstReg, int srcReg, int Shift)
{
    x86___DstSrc_config(dstReg, srcReg);

    SET(0x8b);

    if ((srcReg == rsp) || (srcReg == r12))
    {
        char opcode = 0x44;
        opcode += (char) dstReg * 8;

        SET(opcode);

        SET(0x24);

        if (Shift >= -BORDER_SHORT && Shift <= BORDER_SHORT)
        {
            SET((char) Shift);
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

        SET(opcode);

        if (Shift >= -BORDER_SHORT && Shift <= BORDER_SHORT)
        {
            SET((char) Shift);
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

        SET(opcode);

        if (Shift >= -BORDER_SHORT && Shift <= BORDER_SHORT)
        {
            SET((char) Shift);
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

        SET(opcode);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_add_stack ()
{
    int a_reg = A_REG;
    int b_reg = B_REG;

    x86_pop_r(b_reg);
    x86_pop_r(a_reg);

    x86___DstSrc_config(a_reg, b_reg);

    SET(0x01);

    x86___Regs_config(a_reg, b_reg);

    x86_push_r(a_reg);
}

void SElfBack::x86_sub_stack ()
{
    int a_reg = A_REG;
    int b_reg = B_REG;

    x86_pop_r(b_reg);
    x86_pop_r(a_reg);

    x86___DstSrc_config(a_reg, b_reg);

    SET(0x29);

    x86___Regs_config(a_reg, b_reg);

    x86_push_r(a_reg);
}

void SElfBack::x86_imul_stack ()
{
    int a_reg = A_REG;

    x86_pop_r(a_reg);
    x86_pop_r(rax);

    x86___Dst_config(a_reg);

    SET(0xf7);

    x86___Reg_config(a_reg, 0xe8);

    x86_push_r(rax);
}

void SElfBack::x86_idiv_stack ()
{
    x86_mov_r_i(rdx, 0);

    int a_reg = A_REG;

    x86_pop_r(a_reg);
    x86_pop_r(rax);

    x86___Dst_config(a_reg);

    SET(0xf7);

    x86___Reg_config(a_reg, 0xf8);

    x86_push_r(rax);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_add_i (int Register, int Number)
{
    x86___Dst_config(Register);

    if (Number <= 127)
    {
        SET(0x83);
    }
    else
    {
        SET(0x81);
    }

    x86___Reg_config(Register, 0xc0);

    if (Number <= 127)
    {
        SET((char) Number);
    }
    else
    {
        set_hex_int(Number);
    }
}

void SElfBack::x86_sub_i (int Register, int Number)
{
    x86___Dst_config(Register);

    if (Number <= 127)
    {
        SET(0x83);
    }
    else
    {
        SET(0x81);
    }

    x86___Reg_config(Register, 0xe8);

    if (Number <= 127)
    {
        SET((char) Number);
    }
    else
    {
        set_hex_int(Number);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_inc (int Register)
{
    x86___Dst_config(Register);

    SET(0xff);

    x86___Reg_config(Register, 0xc0);
}

void SElfBack::x86_dec (int Register)
{
    x86___Dst_config(Register);

    SET(0xff);

    x86___Reg_config(Register, 0xc8);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_call (int Shift)
{
    // std::cout << Shift << std::endl;

    SET(0xe8);

    set_hex_int(Shift - CALL_SIZE);
}

void SElfBack::x86_call_label (const wchar_t* Name)
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

void SElfBack::x86_jump_label (const wchar_t* Name, const int JumpMode)
{
    // if (my_find (Name) == true)
    if (Labels.find(Name) != Labels.end())
    {
        if (Labels[Name].finish != NULL_FINISH)
        {

            int JumpAddr = Labels[Name].finish - cur_addr;

            if (JumpMode == jmp_ && JumpAddr < 0)
            {
                JumpAddr += 1;
            }

            x86_jump_norm (JumpAddr, JumpMode); //TODO maybe not

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

    if (JumpMode == jmp_)
    {
        x86_nop();
    }

    x86_jump_norm(0, JumpMode); // fake
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_nop ()
{
    SET(0x90);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_ret ()
{
    SET (0xc3);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_syscall ()
{
    SET (0x0f);
    SET (0x05);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_cmp_r_r (int dstReg, int srcReg)
{
    x86___DstSrc_config(dstReg, srcReg);

    SET(0x39);

    x86___Regs_config(dstReg, srcReg);
}

void SElfBack::x86_cmp_stack ()
{
    x86_pop_r(B_REG); //TODO maybe other order
    x86_pop_r(A_REG);   // definitely other

    x86_cmp_r_r (A_REG, B_REG);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86_jump_any (int Shift, int JumpMode)
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

void SElfBack::x86_jump_norm (int Shift, int JumpMode)
{
    switch (JumpMode)
    {
        case jmp_:
            SET(0xe9);
            break;

        case ja_:
            SET(0x0f);
            SET(0x87);
            break;

        case jae_:
            SET(0x0f);
            SET(0x83);
            break;

        case jb_:
            SET(0x0f);
            SET(0x82);
            break;

        case jbe_:
            SET(0x0f);
            SET(0x86);
            break;

        case je_:
            SET(0x0f);
            SET(0x84);
            break;

        case jne_:
            SET(0x0f);
            SET(0x85);
            break;

        case jl_:
            SET(0x0f);
            SET(0x8c);
            break;

        case jle_:
            SET(0x0f);
            SET(0x8e);
            break;

        case jg_:
            SET(0x0f);
            SET(0x8f);
            break;

        case jge_:
            SET(0x0f);
            SET(0x8d);
            break;

        default:
            MLA(0);
    }

    // int current_jump_size = NORM_JUMP_SIZE;
    // if (JumpMode == jmp_)
    // {
    //     current_jump_size -= 1; // Stupid jumps have different size of opcode
    // }

    set_hex_int(Shift - NORM_JUMP_SIZE);
}

void SElfBack::x86_jump_near (int Shift, int JumpMode)
{
    switch (JumpMode)
    {
        case jmp_:
            SET(0xeb);
            break;

        case ja_:
            SET(0x77);
            break;

        case jae_:
            SET(0x73);
            break;

        case jb_:
            SET(0x72);
            break;

        case jbe_:
            SET(0x76);
            break;

        case je_:
            SET(0x74);
            break;

        case jne_:
            SET(0x75);
            break;

        case jl_:
            SET(0x7c);
            break;

        case jle_:
            SET(0x7e);
            break;

        case jg_:
            SET(0x7f);
            break;

        case jge_:
            SET(0x7d);
            break;

        default:
            MLA(0);
    }

    char offset = 0x00;

    offset += Shift;

    SET(offset - NEAR_JUMP_SIZE);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86___make_inp_func ()
{
    x86___paste_call_label(INP_LBL);
    //TODO inp
    x86_ret();
}

void SElfBack::x86___make_out_func ()
{
    x86___paste_call_label(OUT_LBL);
    //TODO out
    x86_ret();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86___paste_call_label (const wchar_t* Name)
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

        Labels.insert({Name, {cur_addr}});   //TODO fix this
    }
}

void SElfBack::x86___paste_jump_label (const wchar_t* Name)
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

void SElfBack::x86___DstSrc_config (int& dstReg, int& srcReg)
{
    if ((srcReg >= DELTA) && (dstReg >= DELTA))
    {
        SET(0x4d);
        srcReg-= DELTA;
        dstReg-= DELTA;
    }
    else if (srcReg >= DELTA)
    {
        SET(0x4c);
        srcReg-= DELTA;
    }
    else if (dstReg >= DELTA)
    {
        SET(0x49);
        dstReg-= DELTA;
    }
    else
    {
        SET(0x48);
    }
}

void SElfBack::x86___Dst_config (int& dstReg)
{
    if (dstReg >= DELTA)
    {
        SET(0x49);
        dstReg-= DELTA;
    }
    else
    {
        SET(0x48);
    }
}

void SElfBack::x86___Regs_config(const int dstReg, const int srcReg)
{
    char opcode = 0xc0;
    opcode += (char) dstReg;
    opcode += (char) srcReg * 8;

    SET(opcode);
}

void SElfBack::x86___Reg_config(const int Reg, const int ZeroPoint)
{
    char opcode = ZeroPoint;
    opcode += (char) Reg;

    SET(opcode);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SElfBack::x86___End ()
{
    x86_mov_r_i(rax, 0x3c);
    x86_mov_r_i(rdi, 0);

    x86_syscall ();
}

const wchar_t* SElfBack::my_find (const wchar_t* Name)
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


#pragma once
//==================================================================================================================================================================
//29.04.2023
//==================================================================================================================================================================

#include "back.h"

//==================================================================================================================================================================

void make_elf_file (SNode* Root, FILE* ExFile);

//==================================================================================================================================================================

void create_elf_header (SNode* Root, FILE* ExFile);

void create_elf_body (SNode* Root, FILE* ExFile);

//==================================================================================================================================================================

void generate_elf_array (SNode* Root, SBack* Back);

void elf_generate_end_header (SNode* Root, SBack* Back);

void elf_generate_code (BACK_FUNC_HEAD_PARAMETERS);

void generate_main (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_statement (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_function (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_node (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_op_node (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_input (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_output (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_if (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_while (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_call (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_return (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_announce (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_equation (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_expression (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_postorder (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_pop_var (BACK_FUNC_HEAD_PARAMETERS);

void elf_generate_push_var (BACK_FUNC_HEAD_PARAMETERS);

//===================================================================================================================================================================

void elf_push_parameters (BACK_FUNC_HEAD_PARAMETERS);

void elf_pop_parameters (SBack* Back);

void elf_incr_top_reg (SBack* Back);

void elf_standard_if_jump (SBack* Back);

void elf_write_command (ECommandNums eCommand, FILE* File);

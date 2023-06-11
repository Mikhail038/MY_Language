
#ifndef FRONT_H
#define FRONT_H

//=============================================================================================================================================================================
#include <cstddef>
#define TKN_IS_OP       TOKEN.category = CategoryOperation

#include "stackT.h"

//=============================================================================================================================================================================

#define TKN_IS_OP       TOKEN.category = CategoryOperation

#define TKN_IS_WORD (TOKEN.category == CategoryLine && TOKEN.type == TypeVariable)

#define TKN_OP_AND_IS__ TOKEN.category == CategoryOperation && TOKEN.type ==

#define CHECK_SYNTAX(tkn_type)  \
    MTokAss (TKN_OP_AND_IS__ tkn_type); \
    NEXT_TKN;

#define TOKEN         Tokens->array[Tokens->number]
#define TOKEN_ARGS    TOKEN.category, TOKEN.type, TOKEN.data

#define NEXT_TKN    Tokens->number++; \
    if (Tokens->number >= Tokens->size + 1) \
    { \
        fwprintf (stderr, L"Wrong syntax!!!\n"); \
        return NULL; \
    } \

#define VAR     Vars->Arr[counter]
#define FUNC    Funcs->Arr[counter]

//TODO think about add flag here
#define FUNC_HEAD_ARGUMENTS AllTokens* Tokens, SStack<SVarTable*>* Vars, FunctionArr* Funcs, int* marker, bool* PrintFlag
#define FUNC_ARGUMENTS      Tokens, Vars, Funcs, marker, PrintFlag

#define TOKEN_FUNC_DEBUG_INFO   \
    if (*PrintFlag) \
    {   \
        fwprintf (stderr, L"=[%d/%d] %s %s:%d\n", Tokens->number, Tokens->size, LOCATION);    \
    }

#define LEXEM_IS(str) \
(wcscmp (Lexem, str) == 0)

//===================================================================================================================================================================

#define FORBIDDEN_TERMINATING_SYMBOL "@"

const int VAR_TABLE_CAPACITY = 30;
// #define VAR_TABLE_CAPACITY 30

const int MAX_VARS_ARRAY = 50;
// #define MAX_VARS_ARRAY  50

const int MAX_FUNCS_ARRAY = 50;
// #define MAX_FUNCS_ARRAY 50

//===================================================================================================================================================================

void my_front_main (int argc, char** argv);

//===================================================================================================================================================================
//source//
//===================================================================================================================================================================

typedef wchar_t CharT; //will not work with char

typedef double  ValT;

typedef struct
{
    size_t ip   = 0;
    CharT* Arr = NULL;
    size_t size = 0;
}
CodeSource;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

CodeSource* construct_source (CodeSource* Source, FILE* SourceText);

//===================================================================================================================================================================
//seekers//
//===================================================================================================================================================================

void seek (CodeSource* Source);

void seek_out (CodeSource* Source);

//===================================================================================================================================================================
//lexer//
//===================================================================================================================================================================

typedef union
{
    double  val;
    CharT*  var;
} UData;

#define DEF_OP(d_type, ...) \
    d_type,

enum TokenType
{
    #include "Operations.h"
};
#undef DEF_OP

enum Category
{
    CategoryValue     = 0,
    CategoryLine      = 1,
    CategoryOperation = 2
};

typedef struct
{
    Category  category = CategoryValue;
    TokenType type     = TValue;
    UData      data     = {};
}
Token;

typedef struct
{
    int     number;
    int     size;
    Token*  array;
}
AllTokens;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

AllTokens* lex_src (CodeSource* Source, bool* PrintFlag);

void make_lexem (CodeSource* Source, CharT** Lexem);

int create_token (CodeSource* Source,  AllTokens* Tokens, bool* PrintFlag);

int parse_token (CharT* Lexem, AllTokens* Tokens, bool* PrintFlag);

void print_tokens (AllTokens* Tokens, bool* PrintFlag);

// void print_token (Token* CurToken);

void destruct_tokens (AllTokens* Tokens);

//=============================================================================================================================================================================
//number parsers//
//=============================================================================================================================================================================

double parse_double (CharT* Lexem);

double parse_frac (CharT* Lexem, int* Counter);

double parse_int (CharT* Lexem, int* Counter);

//===================================================================================================================================================================
//grammar//
//===================================================================================================================================================================

typedef struct AstNode
{
    AstNode*    left     = NULL;
    AstNode*    right    = NULL;
    Category    category = CategoryValue;
    TokenType   type     = TValue;
    UData       data     = {};
    AstNode*    parent   = NULL;
}
AstNode;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct
{
    CharT* name;
    int    index;
}
SVarAccord;

typedef struct
{
    bool        param_marker;
    size_t      amount = 0;
    size_t      amount_param = 0;

    int         cur_size = 0;

    SVarAccord* Arr  = NULL;
}
SVarTable;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct
{
    CharT*  name        = NULL;
    int     parameters  = 0;
}
Function;

typedef struct
{
    Function*   Arr    = NULL;
    int      size   = 0;
}
FunctionArr;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

// SVars* construct_vars_table (size_t Capacity);
//
// bool add_to_vars_table (CharT* Name, ValT Data, SVars* Vars);
//
// bool check_vars_table (CharT* Name, SVars* Vars);
//
// void show_vars_table (SVars* Vars);
//
// void destruct_vars_table (SVars* Vars);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

void f_create_new_var_table (SStack<SVarTable*>* Vars, bool* PrintFlag);

void f_delete_var_table (SStack<SVarTable*>* Vars, bool* PrintFlag);

bool f_check_vars_table (CharT* Name, SStack<SVarTable*>* Vars);

bool f_find_in_table (CharT* Name, SVarTable* Table, bool* PrintFlag);

void f_add_to_var_table (CharT* Name, SStack<SVarTable*>* Vars, bool* PrintFlag);


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

FunctionArr* construct_funcs_table (size_t Capacity);

int add_to_funcs_table (CharT* Name, FunctionArr* Funcs, bool* PrintFlag);

void add_parameters (int Number, AstNode* Node, FunctionArr* Funcs, bool* PrintFlag);

int count_parameters (AstNode* Node);

bool check_funcs_table (CharT* Name, FunctionArr* Funcs, bool* PrintFlag);

void show_funcs_table (FunctionArr* Funcs, bool* PrintFlag);

void destruct_funcs_table (FunctionArr* Funcs, bool* PrintFlag);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

AstNode* construct_op_node (TokenType Type);

AstNode* construct_var_node (Token* CurToken);

AstNode* construct_val_node (ValT Value);

void delete_tree (AstNode** Node);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

AstNode* get_All          (FUNC_HEAD_ARGUMENTS);

AstNode* get_Statement    (FUNC_HEAD_ARGUMENTS);

AstNode* get_Function     (FUNC_HEAD_ARGUMENTS);

AstNode* get_Head         (FUNC_HEAD_ARGUMENTS);

AstNode* get_Type         (FUNC_HEAD_ARGUMENTS);

AstNode* get_Parameters   (FUNC_HEAD_ARGUMENTS);

AstNode* get_headParam    (FUNC_HEAD_ARGUMENTS);

AstNode* get_Param (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion);

AstNode* get_IfElse       (FUNC_HEAD_ARGUMENTS);

AstNode* get_IfStatements (FUNC_HEAD_ARGUMENTS);

AstNode* get_While        (FUNC_HEAD_ARGUMENTS);

AstNode* get_Call         (FUNC_HEAD_ARGUMENTS);

AstNode* get_Input        (FUNC_HEAD_ARGUMENTS);

AstNode* get_Output       (FUNC_HEAD_ARGUMENTS);

AstNode* get_Return       (FUNC_HEAD_ARGUMENTS);

AstNode* get_Announce     (FUNC_HEAD_ARGUMENTS);

AstNode* get_func_Announce (FUNC_HEAD_ARGUMENTS);

AstNode* get_Equation     (FUNC_HEAD_ARGUMENTS);

AstNode* get_Variable     (FUNC_HEAD_ARGUMENTS);

AstNode* get_Expression   (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion);

AstNode* get_Logic        (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion);

AstNode* get_Compare      (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion);

AstNode* get_AddSub       (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion);

AstNode* get_MulDiv       (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion);

AstNode* get_Pow          (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion);

AstNode* get_Bracket      (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion);

//===================================================================================================================================================================
//GraphViz//
//===================================================================================================================================================================

#define GV_VAR_COLOUR "#1cfc03"
#define GV_OP_COLOUR  "#39c3ed"
#define GV_VAL_COLOUR "#f21847"

#define VAR_SPEC "%ls"
#define VAL_SPEC "%lg"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

void make_graf_viz_tree (AstNode* Root, const char* FileName, bool Display);

void make_gv_node (FILE* File, AstNode* Node);

void print_gv_node (FILE* File, AstNode* Node);

void show_gv_tree (const char* FileName, bool Display);

//===================================================================================================================================================================
//WriteTree//
//===================================================================================================================================================================

void write_tree (AstNode* Root, const char* FileName);

void file_wprint (AstNode* Node, int n, FILE* OutputFile);

void do_tab (int n, FILE* OutputFile);

void print_node (AstNode* Node, FILE* OutputFile);

//===================================================================================================================================================================

#endif

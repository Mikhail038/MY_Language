
#ifndef FRONT_H
#define FRONT_H

//=============================================================================================================================================================================

#define TKN_IS_OP       TKN.category = COperation

#define TKN_IS_WORD (TKN.category == CLine && TKN.type == TVariable)

#define TKN_OP_AND_IS__ TKN.category == COperation && TKN.type ==

#define CHECK_SYNTAX(tkn_type)  \
    MTokAss (TKN_OP_AND_IS__ tkn_type); \
    NEXT_TKN;

#define TKN         Tokens->TokenArr[Tokens->number]
#define TKN_ARGS    TKN.category, TKN.type, TKN.data

#define NEXT_TKN    Tokens->number++; \
    if (Tokens->number >= Tokens->size + 1) \
    { \
        wprintf (L"Error!!!\n"); /*TODO remove*/\
        return NULL; \
    } \

#define VAR     Vars->Arr[counter]
#define FUNC    Funcs->Arr[counter]

//TODO think about add flag here
#define FUNC_HEAD_ARGUMENTS STokens* Tokens, SVars* Vars, SFuncs* Funcs
#define FUNC_ARGUMENTS      Tokens, Vars, Funcs

#define SHOUT wprintf (L"==%p== [%d] %s %s:%d\n", Vars, Tokens->number, LOCATION)

#define LEXEM_IS(str) \
(wcscmp (Lexem, str) == 0)

//===================================================================================================================================================================

#define FORBIDDEN_TERMINATING_SYMBOL "@"

//===================================================================================================================================================================

void my_f_main (int argc, char** argv);

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
SSrc;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

SSrc* construct_source (SSrc* Source, FILE* SourceText);

//===================================================================================================================================================================
//seekers//
//===================================================================================================================================================================

void seek (SSrc* Source);

void seek_out (SSrc* Source);

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

enum ETokenType
{
    #include "Operations.h"
};
#undef DEF_OP

enum ECategory
{
    CValue     = 0,
    CLine      = 1,
    COperation = 2
};

typedef struct
{
    ECategory  category = CValue;
    ETokenType type     = TValue;
    UData      data     = {};
}
SToken;

typedef struct
{
    int     number;
    int     size;
    SToken* TokenArr;
}
STokens;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

STokens* lex_src (SSrc* Source);

int do_token (SSrc* Source, STokens* Tokens);

int parse_token (CharT* Lexem, STokens* Tokens);

void print_tokens (STokens* Tokens);

// void print_token (SToken* Token);

void destruct_tokens (STokens* Tokens);

//=============================================================================================================================================================================
//number parsers//
//=============================================================================================================================================================================

double parse_double (CharT* Lexem);

double parse_frac (CharT* Lexem, int* Counter);

double parse_int (CharT* Lexem, int* Counter);

//===================================================================================================================================================================
//grammar//
//===================================================================================================================================================================

typedef struct SNode
{
    SNode*      left     = NULL;
    SNode*      right    = NULL;
    ECategory   category = CValue;
    ETokenType  type     = TValue;
    UData       data     = {};
    SNode*      parent   = NULL;
}
SNode;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct
{
    CharT*  name    =  NULL;
    ValT    value   =  0;
}
SVar;

typedef struct
{
    SVar*   Arr     = NULL;
    int     size    = 0;
}
SVars;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct
{
    CharT*  name    =  NULL;
    SNode*  code    =  NULL;
}
SFunc;

typedef struct
{
    SFunc*   Arr    = NULL;
    int      size   = 0;
}
SFuncs;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

SVars* construct_vars_table (size_t Capacity);

bool add_to_vars_table (CharT* Name, ValT Data, SVars* Vars);

bool check_vars_table (CharT* Name, SVars* Vars);

void show_vars_table (SVars* Vars);

void destruct_vars_table (SVars* Vars);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

SFuncs* construct_funcs_table (size_t Capacity);

bool add_to_funcs_table (CharT* Name, SFuncs* Funcs);

bool check_funcs_table (CharT* Name, SFuncs* Funcs);

void show_funcs_table (SFuncs* Funcs);

void destruct_funcs_table (SFuncs* Funcs);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

SNode* construct_op_node (ETokenType Type);

SNode* construct_var_node (SToken* Token);

SNode* construct_val_node (ValT Value);

void delete_tree (SNode** Node);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

SNode* get_All          (FUNC_HEAD_ARGUMENTS);

SNode* get_Statement    (FUNC_HEAD_ARGUMENTS);

SNode* get_Function     (FUNC_HEAD_ARGUMENTS);

SNode* get_Head         (FUNC_HEAD_ARGUMENTS);

SNode* get_Type         (FUNC_HEAD_ARGUMENTS);

SNode* get_Parameters   (FUNC_HEAD_ARGUMENTS);

SNode* get_headParam    (FUNC_HEAD_ARGUMENTS);

SNode* get_Param        (FUNC_HEAD_ARGUMENTS);

SNode* get_IfElse       (FUNC_HEAD_ARGUMENTS);

SNode* get_IfStatements (FUNC_HEAD_ARGUMENTS);

SNode* get_While        (FUNC_HEAD_ARGUMENTS);

SNode* get_Call         (FUNC_HEAD_ARGUMENTS);

SNode* get_Input        (FUNC_HEAD_ARGUMENTS);

SNode* get_Output       (FUNC_HEAD_ARGUMENTS);

SNode* get_Return       (FUNC_HEAD_ARGUMENTS);

SNode* get_Announce     (FUNC_HEAD_ARGUMENTS);

SNode* get_func_Announce (FUNC_HEAD_ARGUMENTS);

SNode* get_Equation     (FUNC_HEAD_ARGUMENTS);

SNode* get_Variable     (FUNC_HEAD_ARGUMENTS);

SNode* get_Expression   (FUNC_HEAD_ARGUMENTS);

SNode* get_Logic        (FUNC_HEAD_ARGUMENTS);

SNode* get_Compare      (FUNC_HEAD_ARGUMENTS);

SNode* get_AddSub       (FUNC_HEAD_ARGUMENTS);

SNode* get_MulDiv       (FUNC_HEAD_ARGUMENTS);

SNode* get_Pow          (FUNC_HEAD_ARGUMENTS);

SNode* get_Bracket      (FUNC_HEAD_ARGUMENTS);

//===================================================================================================================================================================
//GraphViz//
//===================================================================================================================================================================

#define GV_VAR_COLOUR "#1cfc03"
#define GV_OP_COLOUR  "#39c3ed"
#define GV_VAL_COLOUR "#f21847"

#define VAR_SPEC "%ls"
#define VAL_SPEC "%lg"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

void make_gv_tree (SNode* Root, const char* FileName);

void make_gv_node (FILE* File, SNode* Node);

void print_gv_node (FILE* File, SNode* Node);

void draw_gv_tree (const char* FileName);

//===================================================================================================================================================================
//WriteTree//
//===================================================================================================================================================================

void write_tree (SNode* Root, const char* FileName);

void file_wprint (SNode* Node, int n, FILE* OutputFile);

void do_tab (int n, FILE* OutputFile);

void print_node (SNode* Node, FILE* OutputFile);

//===================================================================================================================================================================

#endif

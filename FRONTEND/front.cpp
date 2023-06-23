//=============================================================================================================================================================================

#include <cstddef>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <locale.h>
#include <type_traits>
#include <wctype.h>
#include <wchar.h>

//===================================================================================================================================================================

#include "MYassert.h"
#include "back.h"
#include "colors.h"

//=============================================================================================================================================================================

#include "flag_detector.h"
#include "front.h"

//=============================================================================================================================================================================

// #define TOKEN_FUNC_DEBUG_INFO

const int BASE_SCOPES_NUM = 20;

const int MAX_MEMORY = 100000;

const int MAX_LEXEM_SIZE = 30;

#define SEP_SYMBOLS "\n;,() "

#define UNINITIALIZED   NAN

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

const int Err   = -1;
const int StdErr = 1;
const int NoErr  = 0;

//=============================================================================================================================================================================

void users_front_tree (LinePlusFrontParams Argument)
{
    Argument.parametrs->ast_file_name = (char*) Argument.Line;

    #ifdef DEBUG
    printf(KMGN "|ast choosed| [%s]\n" KNRM, Argument.Line);
    #endif
}

void users_front_source (LinePlusFrontParams Argument)
{
    Argument.parametrs->src_file_name = (char*) Argument.Line;

    #ifdef DEBUG
    printf(KMGN "|src choosed| [%s]\n" KNRM, Argument.Line);
    #endif
}

void users_front_graph_viz (LinePlusFrontParams Argument)
{
    Argument.parametrs->gv_file_name = (char*) Argument.Line;

    #ifdef DEBUG
    printf(KMGN "|graph viz choosed| [%s]\n" KNRM, Argument.Line);
    #endif
}

void users_front_help (LinePlusFrontParams Argument)
{
    printf (KGRN Kbright "-t" KNRM " + " KRED "_FILENAME_    " KNRM Kunderscore "choose custom AST tree\n" KNRM);
    printf (KGRN Kbright "-s" KNRM " + " KRED "_FILENAME_    " KNRM Kunderscore "choose custom source file\n" KNRM);
    printf (KGRN Kbright "-g" KNRM " + " KRED "_FILENAME_    " KNRM Kunderscore "put graph_viz dump in custom file\n" KNRM);
    printf (KGRN Kbright "-h" KNRM "   "      "              " KNRM Kunderscore "show to user this information\n" KNRM);

    Argument.parametrs->exit_marker = true;

    #ifdef DEBUG
    printf(KMGN "|helped|\n" KNRM);
    #endif
}

//=============================================================================================================================================================================

void my_front_main (int argc, char** argv)
{
    setlocale(LC_CTYPE, "");

    FrontParameters Parameters
    {
        .ast_file_name = "AST/ParcedSrc.tr",
        .gv_file_name = "LOGS/FRONTEND/GraphVizASTTree",
        .src_file_name = "EXAMPLES/EXAMPLES_GER/elf_fact.ger",
        .exit_marker = false
    };

    const FrontFlagFunction FlagsArray[] =
    {
        {'t', users_front_tree},
        {'s', users_front_source},
        {'g', users_front_graph_viz},
        {'h', users_front_help}
    };
    size_t FlagsAmount = sizeof (FlagsArray) / sizeof (FrontFlagFunction);
    parse_front_flags(&Parameters, argc, argv, (FrontFlagFunction*) &FlagsArray, FlagsAmount);

    if (Parameters.exit_marker == true)
    {
        return;
    }

    FILE* SourceText = fopen(Parameters.src_file_name, "r");

    bool PrintFlag = false;
    if ((argc > 1) && (strcmp (argv[1],"-v") == 0))
    {
        PrintFlag = true;
    }

    MY_LOUD_ASSERT (SourceText != NULL);

    CodeSource Source = *(construct_source (&Source, SourceText));

    fclose (SourceText);

    AllTokens* Tokens = lex_src (&Source, &PrintFlag);

    #ifdef DEBUG_2
    print_tokens (Tokens);
    #endif

    if (Tokens == NULL)
    {
        if (Source.Arr != NULL)
        {
            free (Source.Arr);
        }

        return;
    }

    Tokens->size   = Tokens->number;
    Tokens->number = 0;

    SStack<VarTable*>* Vars = (SStack<VarTable*>*) calloc (1, sizeof (SStack<VarTable*>));
    stack_constructor (Vars, BASE_SCOPES_NUM);

    FunctionArr* Funcs   = construct_funcs_table (MAX_FUNCS_ARRAY);

    int* marker = (int*) calloc (1, sizeof (int));
    *marker = 0;

    AstNode* Root = get_All (Tokens, Vars, Funcs, marker, &PrintFlag);

    free (marker);

    if (Source.Arr != NULL)
    {
        free (Source.Arr);
    }

    if (Tokens != NULL)
    {
        destruct_tokens (Tokens);
    }

    if (Vars != NULL)
    {
        stack_destructor (Vars);
    }

    if (Funcs != NULL)
    {
        destruct_funcs_table (Funcs, &PrintFlag);
    }

    if (Root != NULL)
    {
        make_graf_viz_tree (Root, Parameters.gv_file_name, false);

        write_tree (Root, Parameters.ast_file_name);

        delete_tree (&Root);
    }

    return;
}

//=============================================================================================================================================================================
//source//
//=============================================================================================================================================================================

CodeSource* construct_source (CodeSource* Source, FILE* SourceText)
{

    fwscanf (SourceText, L"%ml[^~]", &Source->Arr); // Asked Danya how to remove this shit!! nikak

    Source->ip   = 0;
    Source->size = wcslen (Source->Arr);

    return Source;
}

//=============================================================================================================================================================================
//seekers//
//=============================================================================================================================================================================

void seek (CodeSource* Source)
{
    for (; Source->ip < Source->size; Source->ip++)
    {
        if ((Source->Arr[Source->ip] != L'\n') && ((Source->Arr[Source->ip] != L' ')))
        {
            return;
        }
    }

    return;
}

void seek_out (CodeSource* Source)
{
    for (; Source->ip < Source->size; Source->ip++)
    {
        if ((Source->Arr[Source->ip] == L'\n'))// || ((Buffer->Array[Buffer->ip] == L' ')))
        {
            Source->ip++;
            return;
        }
    }

    return;
}

//=============================================================================================================================================================================
//lexer//
//=============================================================================================================================================================================

AllTokens* lex_src (CodeSource* Source, bool* PrintFlag)
{
    AllTokens* Tokens  = (AllTokens*) calloc (1, sizeof (*Tokens));
    Tokens->array = (Token*)  calloc (Source->size, sizeof (*Tokens->array));

    //wprintf (L"%lu==%lu==\n", Source->ip, Source->size);

    while (Source->ip < Source->size)
    {
        if (create_token (Source, Tokens, PrintFlag) != NoErr)
        {
            free (Tokens->array);

            return NULL;
        }
    }

    if (*PrintFlag)
    {
        wprintf (L"Created Tokens Array. size |%d|\n", Tokens->number);
    }

    return Tokens;
}

int create_token (CodeSource* Source,  AllTokens* Tokens, bool* PrintFlag)
{
    seek (Source);
    CharT* Lexem = NULL;

    make_lexem (Source, &Lexem);

//     swscanf (&(Source->Arr[Source->ip]), L"%ml[^" SEP_SYMBOLS "]", &Lexem);
//
//     //wprintf (L"\n++%ls++\n", Lexem);
//
//     if (Lexem == NULL)
//     {
//         swscanf (&(Source->Arr[Source->ip]), L"%ml[^\n ]", &Lexem);
//         //wprintf (L"!%lc!\n", Source->Code[Source->ip]);
//     }

    if (Lexem == NULL && Source->Arr[Source->ip] == L'\0')
    {
        return NoErr;
    }

    MTokAss (Lexem != NULL);

    //wprintf (L"\n++%ls++\n", Lexem);
    //wprintf (L"\n++%lc++\n", *Lexem);

    if (parse_token (Lexem, Tokens, PrintFlag) != NoErr)
    {
        free (Lexem);

        return StdErr;
    }


    free (Lexem);

    return NoErr;
}

void make_lexem (CodeSource* Source, CharT** Lexem)
{
    CharT* RawLexem = (CharT*) calloc (MAX_LEXEM_SIZE, sizeof (CharT));

    int i = 0;
    for (; 1 ; )
    {
        MY_LOUD_ASSERT (i < MAX_LEXEM_SIZE);

        if (iswalpha (Source->Arr[Source->ip]) != 0)
        {
            while ( (iswalpha (Source->Arr[Source->ip]) != 0)
                ||  (iswdigit (Source->Arr[Source->ip]) != 0)
                ||  (Source->Arr[Source->ip] == L'_'))
            {
                RawLexem[i] = Source->Arr[Source->ip];
                i++;
                Source->ip++;
            }

            break;
        }

        if (iswdigit (Source->Arr[Source->ip]) != 0)
        {
            while ( (iswdigit (Source->Arr[Source->ip]) != 0)
                ||  (Source->Arr[Source->ip] == L'.') )
            {
                RawLexem[i] = Source->Arr[Source->ip];
                i++;
                Source->ip++;
            }

            break;
        }

        if ((   Source->Arr[Source->ip] == '!'
            ||  Source->Arr[Source->ip] == '<'
            ||  Source->Arr[Source->ip] == '>'
            ||  Source->Arr[Source->ip] == '=')
            && (Source->Arr[Source->ip + 1] == '='))
        {
                RawLexem[i] = Source->Arr[Source->ip];
                i++;
                Source->ip++;

                RawLexem[i] = Source->Arr[Source->ip];
                i++;
                Source->ip++;

                break;
        }

        if ((   Source->Arr[Source->ip] == '&'
            &&  Source->Arr[Source->ip + 1] == '&')
            ||  (Source->Arr[Source->ip] == '|'
            &&  Source->Arr[Source->ip + 1] == '|'))
        {
                RawLexem[i] = Source->Arr[Source->ip];
                i++;
                Source->ip++;

                RawLexem[i] = Source->Arr[Source->ip];
                i++;
                Source->ip++;

                break;
        }

        if (    (Source->Arr[Source->ip] == L'\n')
             || (Source->Arr[Source->ip] == L' '))
        {
            Source->ip++;
            continue;
        }

        if ((Source->Arr[Source->ip]) == L'\0')
        {
            break;
        }

        RawLexem[i] = Source->Arr[Source->ip];
        i++;
        Source->ip++;

        break;
    }

    RawLexem[i] = L'\0';

    swscanf (RawLexem, L"%mls", Lexem);
    free (RawLexem);

    //wprintf (L"\n++%ls++\n", *Lexem);

    return;
}


#define DEF_OP(d_type, d_condition, d_tokenize,...) \
else if (d_condition) \
{ \
    if (*PrintFlag) \
    {   \
        wprintf (L"It is:  " #d_type "\n"); \
    }   \
    d_tokenize \
    TOKEN.type = d_type; \
    Tokens->number++; \
}

int parse_token (CharT* Lexem, AllTokens* Tokens, bool* PrintFlag)
{
    if (*PrintFlag)
    {
        wprintf (L"[%d]\n""I see: '%ls'\n", Tokens->number, Lexem);
    }

    if (0) {}
    #include "Operations.h"
    else
    {
        wprintf (L"Lexer default error. Word '%ls'\n", Lexem);
        return StdErr;
    }

    #undef DEF_OP

    return 0;
}

void print_tokens (AllTokens* Tokens, bool* PrintFlag)
{
    if (*PrintFlag)
    {
        wprintf (L"==Tokens=array==\n");
    }

    for (int counter = 0; counter < Tokens->number; ++counter)
    {
        if (*PrintFlag)
        {
            wprintf (L"[%.3d] %d %d", counter, Tokens->array[counter].category, Tokens->array[counter].type);

            wprintf (L"\n");
        }

        //print_token (Tokens->array[counter]);
    }

    return;
}

void destruct_tokens (AllTokens* Tokens)
{
    //wprintf (L"|%d|\n", Tokens->number);
    for (int counter = 0; counter < Tokens->size; ++counter)
    {
        if (Tokens->array[counter].category == NameNode)
        {
            free (Tokens->array[counter].data.var);
        }
    }

    free (Tokens->array);
    free (Tokens);
}

//=============================================================================================================================================================================
//number parsers//
//=============================================================================================================================================================================

double parse_double (CharT* Lexem)
{
    double Value = 0;

    int counter = 0;

    if (Lexem[counter] == 'e' || Lexem[counter] == 'E')
    {
        counter++;

        Value =  M_E;
    }
    else if (wcsncasecmp (&(Lexem[counter]), L"PI", 2) == 0)
    {
        counter += 2;

        Value = M_PI;
    }
    else
    {
        Value = parse_int (Lexem, &counter);

        double SecondValue = 0;

        double ThirdValue = 0;

        if (Lexem[counter] == '.')
        {
            counter++;

            SecondValue = parse_frac (Lexem, &counter);
        }


        if (Lexem[counter] == 'e' || Lexem[counter] == 'E')
        {
            counter++;

            ThirdValue = parse_int (Lexem, &counter);
        }

        Value = Value + SecondValue;

        Value = Value * pow (10, ThirdValue);
    }

    return Value;
}

double parse_frac (CharT* Lexem, int* Counter)
{
    int counter = 1;

    int in_counter = 0;

    double Value = 0;

    double Add = 0;

    int OldPtr = *Counter;

    while (Lexem[*Counter] >= '0' && Lexem[*Counter] <= '9')
    {
        Add = (Lexem[*Counter] - '0');

        in_counter = counter;

        while (in_counter > 0)
        {
            Add /= 10;

            in_counter--;
        }

        Value = Value  + Add;

        counter++;

        (*Counter)++;
    }

    MY_LOUD_ASSERT (*Counter > OldPtr);

    return Value;
}

double parse_int (CharT* Lexem, int* Counter)
{
    double Value = 0;

    int OldPtr = *Counter;

    while (Lexem[*Counter] >= L'0' && Lexem[*Counter] <= L'9')
    {
        Value = Value * 10 + (Lexem[*Counter] - L'0');

        (*Counter)++;
    }

    MY_LOUD_ASSERT (*Counter > OldPtr);

    return Value;
}

//=============================================================================================================================================================================
//front stuff//
//=============================================================================================================================================================================

void f_create_new_var_table (SStack<VarTable*>* Vars, bool* PrintFlag)
{
    if (*PrintFlag)
    {
        wprintf (L"==%p== %s %s:%d\n", Vars, LOCATION);
    }

    VarTable* NewTable = (VarTable*)  calloc (1,                  sizeof (VarTable));
    NewTable->Arr       = (SVarAccord*) calloc (VAR_TABLE_CAPACITY, sizeof (SVarAccord));
    NewTable->cur_size = 0;

    push_in_stack (Vars, NewTable);

    return;
}

void f_delete_var_table (SStack<VarTable*>* Vars, bool* PrintFlag)
{
    if (*PrintFlag)
    {
        wprintf (L"==%p== %s %s:%d\n", Vars, LOCATION);
    }

    VarTable* Table = NULL;

    pop_from_stack (Vars, &Table);

    free (Table->Arr);

    free (Table);

    return;
}

bool f_check_vars_table (CharT* Name, SStack<VarTable*>* Vars, bool* PrintFlag)
{
    VarTable* Table = NULL;

    int depth = 1;
    do
    {
        Table = Vars->data[Vars->size - depth];
        depth++;

        if (*PrintFlag)
        {
            wprintf (L"!!%ls!!\n", Name);
        }

        if (f_find_in_table (Name, Table, PrintFlag) == true)
        {
            return true;
        }

        if (*PrintFlag)
        {
            wprintf (L"^^%ls^^\n", Name);
        }

    } while (depth <= Vars->size);

    return false;
}

bool f_find_in_table (CharT* Name, VarTable* Table, bool* PrintFlag)
{
    MY_LOUD_ASSERT (Table != NULL);

    for (int counter = 0; counter < Table->cur_size; ++counter)
    {
        if (*PrintFlag)
        {
            wprintf (L"%d--%d--%ls--\n", Table->cur_size, counter, Table->Arr[counter].name);
        }

        if (wcscmp (Name, Table->Arr[counter].name) == 0)
        {
            return true;
        }
    }

    return false;
}

void f_add_to_var_table (CharT* Name, SStack<VarTable*>* Vars, bool* PrintFlag)
{
    if (*PrintFlag)
    {
        wprintf (L"==%p== %s %s:%d\n", Vars, LOCATION);
    }

    MY_LOUD_ASSERT (Name != NULL);

    // if (Back->table_condition == none)
    // {
    //     create_new_var_table (Back);
    // }

    VarTable* Table = NULL;
    peek_from_stack (Vars, &Table);

    Table->Arr[Table->cur_size].name = Name; //copy address from node

    if (*PrintFlag)
    {
        wprintf (L"added '%ls'\n", Table->Arr[Table->cur_size].name);
    }

    Table->cur_size++;

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

FunctionArr* construct_funcs_table (size_t Capacity)
{
    FunctionArr* Funcs = (FunctionArr*) calloc (1, sizeof (*Funcs));

    Funcs->Arr = (Function*) calloc (Capacity, sizeof (*Funcs->Arr));

    return Funcs;
}

int add_to_funcs_table (CharT* Name, FunctionArr* Funcs)
{
        for (int counter = 0; counter < MAX_FUNCS_ARRAY; counter++)
        {
            if ( FUNC.name != NULL && wcscmp (FUNC.name, Name) == 0)
            {
                return Err;
            }

            if (FUNC.name == NULL)
            {
                FUNC.name = wcsdup (Name);

                Funcs->size++;

                return counter;
            }
        }

        wprintf (KRED L"==ERROR==Function table overflow\n" KNRM);
        return Err;
}

void add_parameters (int Number, AstNode* Node, FunctionArr* Funcs)
{
    MY_LOUD_ASSERT (Funcs->Arr[Number].name != NULL);
    Funcs->Arr[Number].parameters = count_parameters (Node);

    return;
}

int count_parameters (AstNode* Node)
{
    AstNode* CurNode = Node;

    MY_LOUD_ASSERT (Node != NULL);
    int counter = 0;

    while (CurNode != NULL)
    {
        if (CurNode->left != NULL)
        {
            counter++;
        }

        CurNode = CurNode->right;
    }

    return counter;
}

bool check_funcs_table (CharT* Name, FunctionArr* Funcs)
{
    for (int counter = 0; counter < Funcs->size; counter++)
    {
        if (wcscmp (FUNC.name, Name) == 0)
        {
            return true;
        }
    }

    return false;
}

void show_funcs_table (FunctionArr* Funcs, bool* PrintFlag)
{
    if (*PrintFlag)
    {
        wprintf (L"==Funcs Array== size %d\n", Funcs->size);
    }

    for (int counter = 0; counter <= Funcs->size; ++counter)
    {
        if (FUNC.name != NULL)
        {
            if (*PrintFlag)
            {
                wprintf (L"[%d] %ls\n", counter, FUNC.name);
            }
        }
    }
}

void destruct_funcs_table (FunctionArr* Funcs, bool* PrintFlag)
{
    show_funcs_table (Funcs, PrintFlag);

    for (int counter = 0; counter <= Funcs->size; ++counter)
    {
        if (Funcs->Arr[counter].name != NULL)
        {
            free (Funcs->Arr[counter].name);
            Funcs->Arr[counter].name = NULL;
        }
    }

    free (Funcs->Arr);
    Funcs->Arr = NULL;

    free (Funcs);
    Funcs = NULL;

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

AstNode* construct_op_node (TokenType Type)
{
    AstNode* Node = (AstNode*) calloc (1, sizeof (*Node));

    Node->category = OperationNode;

    Node->type = Type;

    return Node;
}

AstNode* construct_var_node (Token* CurToken)
{
    AstNode* Node = (AstNode*) calloc (1, sizeof (*Node));

    Node->category = NameNode;

    Node->type = TypeVariable;

    MY_LOUD_ASSERT (CurToken->category == NameNode && CurToken->type == TypeVariable);

    Node->data.var = wcsdup (CurToken->data.var);

    return Node;
}

AstNode* construct_val_node (ValT Value)
{
    AstNode* Node = (AstNode*) calloc (1, sizeof (*Node));

    Node->category = ValueNode;

    Node->type = TValue;

    Node->data.val = Value;

    return Node;
}

void delete_tree (AstNode** Node)
{
    if (*Node == NULL)
    {
        return;
    }

    if ((*Node)->left != NULL)
    {
        delete_tree (&((*Node)->left));
    }

    if ((*Node)->right != NULL)
    {
        delete_tree (&((*Node)->right));
    }

    if ((*Node)->category == NameNode)
    {
        free ((*Node)->data.var);
        (*Node)->data.var = NULL;
    }

    free (*Node);

    *Node = NULL;
    //printf ("\n%p freed^\n", *Node);

    return;
}

//=============================================================================================================================================================================
//grammar//
//=============================================================================================================================================================================

AstNode* get_All (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    AstNode* Root = get_Statement (FUNC_ARGUMENTS);

    return Root;
}

AstNode* get_Statement (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    // if (TKN_OP_AND_IS__ TypeOpenBracket
    // && (Tokens->array[Tokens->number + 1].category == OperationNode
    // &&  Tokens->array[Tokens->number + 1].type == TypeCloseBracket))
    // {
    //     NEXT_TKN;
    //     NEXT_TKN;
    // }

    if (TKN_OP_AND_IS__ TypeFinish)
    {
        NEXT_TKN;

        return get_Statement (FUNC_ARGUMENTS);
    }

    if (TKN_OP_AND_IS__ TypeCloseBracket)
    {
        NEXT_TKN;

        f_delete_var_table (Vars, PrintFlag);

        return NULL;
    }

    AstNode* Node = construct_op_node (TypeStatement);


    Node->left  = get_Function (FUNC_ARGUMENTS);

    if (TKN_OP_AND_IS__ TypeOpenBracket)
    {
        if (*marker > 0)
        {
            (*marker)--;
        }
        else
        {
            f_create_new_var_table (Vars, PrintFlag);
        }

        NEXT_TKN;
    }

    if (Tokens->number < Tokens->size)
    {
        //int OldPtr = Tokens->number;

        Node->right = get_Statement (FUNC_ARGUMENTS);
    }

    return Node;
}

AstNode* get_Function (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (((TKN_OP_AND_IS__ TypeVoidVarType) || (TKN_OP_AND_IS__ TypeStdVarType)) &&
    (Tokens->array[Tokens->number + 1].category == NameNode) &&
    (Tokens->array[Tokens->number + 2].type == TypeOpenRoundBracket))
    {
        AstNode* Node = construct_op_node (TypeFunction);

        // if (Vars != NULL && (Vars->size > 0))
        // {
        //     f_delete_var_table (Vars);
        // }
        f_create_new_var_table (Vars, PrintFlag);

        (*marker)++;

        Node->left  = get_Head (FUNC_ARGUMENTS);

        Node->right = get_Statement (FUNC_ARGUMENTS);

        return Node;
    }

    return get_IfElse (FUNC_ARGUMENTS);
}

AstNode* get_Head (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    AstNode* Left_son = get_Type (FUNC_ARGUMENTS);

    AstNode* Node = construct_var_node (&TOKEN);

    int table_number = add_to_funcs_table (TOKEN.data.var, Funcs);

    if (table_number == Err)
    {
        wprintf (KRED L"ERROR\n" KNRM);
        MY_LOUD_ASSERT (0);
    }

    NEXT_TKN; //Tokens->number++;

    Node->left  = Left_son;

    Node->right = get_Parameters (FUNC_ARGUMENTS);

    add_parameters (table_number, Node->right, Funcs);

    return Node;
}

AstNode* get_Type (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    MTokAss ((TKN_OP_AND_IS__ TypeStdVarType) || (TKN_OP_AND_IS__ TypeVoidVarType));

    AstNode* Node = construct_op_node (TOKEN.type);

    NEXT_TKN; //Tokens->number++;

    return Node;
}

AstNode* get_Parameters (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    CHECK_SYNTAX (TypeOpenRoundBracket);

    AstNode* Node = get_headParam (FUNC_ARGUMENTS);

    CHECK_SYNTAX (TypeCloseRoundBracket);

    return Node;
}

AstNode* get_headParam (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    AstNode* Node = construct_op_node (TypeParameter);

    Node->left = get_func_Announce (FUNC_ARGUMENTS);

    if (TKN_OP_AND_IS__ TypeComma)
    {
        NEXT_TKN; //Tokens->number++;

        Node->right = get_headParam (FUNC_ARGUMENTS);

        return Node;
    }

    Node->right = NULL;

    return Node;
}

AstNode* get_Param (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    AstNode* Node = construct_op_node (TypeParameter);

    //MY_LOUD_ASSERT (f_check_vars_table (TOKEN.data.var, Vars) == true);

    Node->left = get_Expression (FUNC_ARGUMENTS, ZeroRetPermisiion);

    if (TKN_OP_AND_IS__ TypeComma)
    {
        NEXT_TKN; //Tokens->number++;

        Node->right = get_Param (FUNC_ARGUMENTS, false);

        return Node;
    }

    Node->right = NULL;

    return Node;
}

AstNode* get_IfElse (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_OP_AND_IS__ TypeIf)
    {
        AstNode* Node = construct_op_node (TOKEN.type);
        NEXT_TKN;


        CHECK_SYNTAX (TypeOpenRoundBracket);

        Node->left  = get_Expression (FUNC_ARGUMENTS, false);

        CHECK_SYNTAX (TypeCloseRoundBracket);

        Node->right = get_IfStatements (FUNC_ARGUMENTS);

        return Node;
    }

    return get_While (FUNC_ARGUMENTS);
}

AstNode* get_IfStatements (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    AstNode* Node = construct_op_node (TypeBranch);

    Node->left = get_Statement (FUNC_ARGUMENTS);

    if (TKN_OP_AND_IS__ TypeElse)
    {
        NEXT_TKN;

        if (TKN_OP_AND_IS__ TypeIf)
        {
            Node->right = get_IfElse (FUNC_ARGUMENTS);

            return Node;
        }

        Node->right = get_Statement (FUNC_ARGUMENTS);
    }

    return Node;
}

AstNode* get_While (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_OP_AND_IS__ TypeWhile)
    {
        AstNode* Node = construct_op_node (TOKEN.type);
        NEXT_TKN;


        CHECK_SYNTAX (TypeOpenRoundBracket);

        Node->left  = get_Expression (FUNC_ARGUMENTS, false);

        CHECK_SYNTAX (TypeCloseRoundBracket);

        Node->right = get_Statement (FUNC_ARGUMENTS);

        return Node;
    }

    return get_Input (FUNC_ARGUMENTS);
}

AstNode* get_Input (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_OP_AND_IS__ TypeInput)
    {
        AstNode* Node = construct_op_node (TOKEN.type);
        NEXT_TKN;

        CHECK_SYNTAX (TypeOpenRoundBracket);

        Node->left = get_Param (FUNC_ARGUMENTS, false);

        CHECK_SYNTAX (TypeCloseRoundBracket);

        MY_LOUD_ASSERT (Node->left->left != NULL);

        return Node;
    }

    return get_Output (FUNC_ARGUMENTS);
}

AstNode* get_Output (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_OP_AND_IS__ TypeOutput)
    {
        AstNode* Node = construct_op_node (TOKEN.type);
        NEXT_TKN;

        CHECK_SYNTAX (TypeOpenRoundBracket);

        Node->left = get_Param (FUNC_ARGUMENTS, false);

        CHECK_SYNTAX (TypeCloseRoundBracket);

        MY_LOUD_ASSERT (Node->left->left != NULL);

        return Node;
    }

    return get_Return (FUNC_ARGUMENTS);
}

AstNode* get_Return (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_OP_AND_IS__ TypeReturn)
    {
        AstNode* Node = construct_op_node (TOKEN.type);
        NEXT_TKN;

        Node->left  = get_Expression (FUNC_ARGUMENTS, false);

        Node->right = NULL;

        return Node;
    }

    return get_Announce (FUNC_ARGUMENTS);
}

AstNode* get_Announce (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_OP_AND_IS__ TypeStdVarType)
    {
        NEXT_TKN;

        AstNode* Node = construct_op_node (TypeAnnounce);

        Node->left  = get_Variable (FUNC_ARGUMENTS);

        if (TKN_OP_AND_IS__ TypeAssign)
        {
            NEXT_TKN;

            Node->right = get_Expression (FUNC_ARGUMENTS, false);
        }

        MY_LOUD_ASSERT (Vars->size != 0);

        if (f_find_in_table (Node->left->data.var, Vars->data[Vars->size - 1], PrintFlag) == true)
        {
            wprintf (L"==ERROR==\n""Variable '%ls' has been already announced!\n", Node->left->data.var);

            MY_LOUD_ASSERT (0);
        }
        else
        {
            f_add_to_var_table (Node->left->data.var, Vars, PrintFlag);
        }

        //TODO do this in middle end later
        // if (Node->right != NULL && Node->right->category == ValueNode && Node->right->type == TValue)
        // {
        //     if (add_to_vars_table (Node->left->data.var, Node->right->data.val, Vars) == false)
        //     {
        //         wprintf (L"==ERROR==\n""Variable '%ls' has been already announced!\n", Node->left->data.var);
        //         exit (0);
        //     }
        // }
        // else
        // {
        //     if (add_to_vars_table (Node->left->data.var, UNINITIALIZED, Vars) == false)
        //     {
        //         wprintf (L"==ERROR==\n""Variable '%ls' has been already announced!\n", Node->left->data.var);
        //         exit (0);
        //     }
        // }

        CHECK_SYNTAX (TypeFinish);

        return Node;
    }

    return get_Equation (FUNC_ARGUMENTS);
}

AstNode* get_func_Announce (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_OP_AND_IS__ TypeStdVarType)
    {
        NEXT_TKN;

        AstNode* Node = construct_op_node (TypeFuncAnnounce);

        Node->left  = get_Variable (FUNC_ARGUMENTS);

        if (f_find_in_table (Node->left->data.var, Vars->data[Vars->size - 1], PrintFlag) == true)
        {
            wprintf (L"==ERROR==\n""Variable '%ls' has been already announced!\n", Node->left->data.var);

            MY_LOUD_ASSERT (0);
        }
        else
        {
            f_add_to_var_table (Node->left->data.var, Vars, PrintFlag);
        }

        return Node;
    }

    return NULL;
}

AstNode* get_Variable (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    AstNode* Node = construct_var_node (&TOKEN);

    NEXT_TKN; //Tokens->number++;

    return Node;
}

AstNode* get_Equation (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_IS_WORD &&
    (Tokens->array[Tokens->number + 1].category == OperationNode &&
    Tokens->array[Tokens->number + 1].type == TypeAssign))
    {
        if (f_check_vars_table (TOKEN.data.var, Vars, PrintFlag) == true)
        {
            AstNode* Node = construct_op_node (TypeAssignment);

            Node->left  = construct_var_node (&TOKEN);
            NEXT_TKN;

            //here is TypeAssign
            NEXT_TKN;

            Node->right = get_Expression (FUNC_ARGUMENTS, false);

            CHECK_SYNTAX (TypeFinish);

            return Node;
        }

        wprintf (L"==ERROR==\n""No such word '%ls' found!\n", TOKEN.data.var);

        MY_LOUD_ASSERT (0);
    }

    return get_Call (FUNC_ARGUMENTS);
}

AstNode* get_Call (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TOKEN.category == NameNode && TOKEN.type == TypeVariable && check_funcs_table (TOKEN.data.var, Funcs) == true)
    {
        AstNode* Node = construct_op_node (TypeCall);

        Node->left = construct_var_node (&TOKEN);

        CharT* FuncName = TOKEN.data.var;

        NEXT_TKN;

        CHECK_SYNTAX (TypeOpenRoundBracket);

        Node->right = get_Param (FUNC_ARGUMENTS, true);

        CHECK_SYNTAX (TypeCloseRoundBracket);

        int Parameters = count_parameters (Node->right);

        for (int counter = 0; counter < Funcs->size; counter++)
        {
            if (wcscmp (FUNC.name, FuncName) == 0)
            {
                MY_LOUD_ASSERT (Parameters == Funcs->Arr[counter].parameters);
            }
        }

        return Node;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

AstNode* get_Expression (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    AstNode* Node = get_Logic (FUNC_ARGUMENTS, ZeroRetPermisiion);

    return Node;
}

AstNode* get_Logic (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    AstNode* LeftSon = get_Compare (FUNC_ARGUMENTS, ZeroRetPermisiion);

    if
    (
        (TKN_OP_AND_IS__ TypeLogicAnd)   ||
        (TKN_OP_AND_IS__ TypeLogicOr)   /* ||
        (TKN_OP_AND_IS__ TypeLogicNot) */
    )
    {
        TokenType CurrentType = TOKEN.type;
        NEXT_TKN;

        AstNode* Node = construct_op_node (CurrentType);

        Node->left = LeftSon;

        Node->right = get_Logic (FUNC_ARGUMENTS, ZeroRetPermisiion);

        return Node;
    }

    return LeftSon;
}

AstNode* get_Compare (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    AstNode* LeftSon = get_AddSub (FUNC_ARGUMENTS, ZeroRetPermisiion);

    if
    (
        (TKN_OP_AND_IS__ TypeLogicEqual)   ||
        (TKN_OP_AND_IS__ TypeLogicLess)    ||
        (TKN_OP_AND_IS__ TypeLogicMore)    ||
        (TKN_OP_AND_IS__ TypeLogicLessEqual)  ||
        (TKN_OP_AND_IS__ TypeLogicMoreEqual)  ||
        (TKN_OP_AND_IS__ TypeLogicNotEqual)
    )
    {
        TokenType CurrentType = TOKEN.type;
        NEXT_TKN;

        AstNode* Node = construct_op_node (CurrentType);

        Node->left = LeftSon;

        Node->right = get_Compare (FUNC_ARGUMENTS, ZeroRetPermisiion);

        return Node;
    }

    return LeftSon;
}

AstNode* get_AddSub (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    AstNode* LeftSon = get_MulDiv (FUNC_ARGUMENTS, ZeroRetPermisiion);

    if ((TKN_OP_AND_IS__ TypeArithAdd)|| (TKN_OP_AND_IS__ TypeArithSub))
    {
        TokenType CurrentType = TOKEN.type;
        NEXT_TKN;

        AstNode* Node = construct_op_node (CurrentType);

        Node->left = LeftSon;

        Node->right = get_AddSub (FUNC_ARGUMENTS, ZeroRetPermisiion);

        return Node;
    }

    return LeftSon;
}

AstNode* get_MulDiv (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    AstNode* LeftSon =  get_Pow (FUNC_ARGUMENTS, ZeroRetPermisiion);

    if ((TKN_OP_AND_IS__ TypeArithMul)|| (TKN_OP_AND_IS__ TypeArithDiv))
    {
        TokenType CurrentType = TOKEN.type;
        NEXT_TKN;

        AstNode* Node = construct_op_node (CurrentType);

        Node->left = LeftSon;

        Node->right =  get_MulDiv (FUNC_ARGUMENTS, ZeroRetPermisiion);

        return Node;
    }

    return LeftSon;
}

AstNode* get_Pow (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    AstNode* LeftSon = get_Bracket (FUNC_ARGUMENTS, ZeroRetPermisiion);

    if (TKN_OP_AND_IS__ TypeArithPow)
    {
        TokenType CurrentType = TOKEN.type;
        NEXT_TKN;

        AstNode* Node = construct_op_node (CurrentType);

        Node->left = LeftSon;

        Node->right = get_Pow (FUNC_ARGUMENTS, ZeroRetPermisiion);

        return Node;
    }

    return LeftSon;
}

AstNode* get_Bracket (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    AstNode* Node = NULL;

    if (TKN_OP_AND_IS__ TypeOpenRoundBracket)
    {
        NEXT_TKN;

        Node = get_Expression (FUNC_ARGUMENTS, ZeroRetPermisiion); //CHANGE IT ON GET_LOGIC IF CHANGES EXPRESSION

        CHECK_SYNTAX (TypeCloseRoundBracket);

        return Node;
    }

    if (TKN_IS_WORD)
    {
        if (Tokens->array[Tokens->number + 1].category == OperationNode &&
            Tokens->array[Tokens->number + 1].type     == TypeOpenRoundBracket)
        {
            #define DEF_UN(def_name, def_type) \
            else if (wcscmp (def_name, TOKEN.data.var) == 0) \
            { \
                Node = construct_op_node (def_type); \
                NEXT_TKN; /* skipped func name */ \
                NEXT_TKN; /* skipped ( */ \
                Node->right = get_Expression (FUNC_ARGUMENTS, ZeroRetPermisiion); /* //I forgot -> HERE!!! */ \
                CHECK_SYNTAX (TypeCloseRoundBracket); \
                return Node; \
            }

            if (0) {}

            #include "BaseUnaryFunc.h"

            #undef DEF_UN

            return get_Call (FUNC_ARGUMENTS);
        }


        if (f_check_vars_table (TOKEN.data.var, Vars, PrintFlag) == true)
        {
            Node = construct_var_node (&TOKEN);

            NEXT_TKN;

            return Node;
        }

        wprintf (L"==ERROR==\n""No such word '%ls' found!\n", TOKEN.data.var);
    }

    if (TOKEN.category == ValueNode && TOKEN.type == TValue)
    {
        Node = construct_val_node (TOKEN.data.val);
        NEXT_TKN;

        return Node;
    }

    if (ZeroRetPermisiion == true)
    {
        return NULL;
    }

    wprintf (KRED L"==ERROR!!=expected variable, function or value=\n" KNRM);
    //wprintf (L"==ERROR==\n""No such'%ls' found!\n", TOKEN.data.var);
    MTokAss (0);

    return NULL;
}

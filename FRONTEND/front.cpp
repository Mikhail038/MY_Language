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

#include "front.h"

//=============================================================================================================================================================================

// #define TOKEN_FUNC_DEBUG_INFO

#define BASE_SCOPES_NUM 20

#define MAX_MEMORY  100000
#define MAX_LEXEM_SIZE 30

#define SEP_SYMBOLS "\n;,() "

#define UNINITIALIZED   NAN

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

const int Err   = -1;
const int StdErr = 1;
const int NoErr  = 0;

//=============================================================================================================================================================================

void my_f_main (int argc, char** argv)
{
    setlocale(LC_CTYPE, "");

    FILE* SourceText = ((argc > 1) && (strlen (argv[1]) > 3)) ? fopen (argv[1], "r") : fopen ("EXAMPLES/kvad.ger", "r") ;
    if (argc > 2)
    {
        fopen (argv[2], "r");
    }

    bool PrintFlag = false;
    if ((argc > 1) && (strcmp (argv[1],"-v") == 0))
    {
        PrintFlag = true;
    }

    MY_LOUD_ASSERT (SourceText != NULL);

    CodeSource Source = *(construct_source (&Source, SourceText));

    fclose (SourceText);

    AllTokens* Tokens = lex_src (&Source, &PrintFlag);

    //print_tokens (Tokens);

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

    SStack<SVarTable*>* Vars = (SStack<SVarTable*>*) calloc (1, sizeof (SStack<SVarTable*>));
    stack_constructor (Vars, BASE_SCOPES_NUM);

    SFuncs* Funcs   = construct_funcs_table (MAX_FUNCS_ARRAY);

    int* marker = (int*) calloc (1, sizeof (int));
    *marker = 0;

    SNode* Root = get_All (Tokens, Vars, Funcs, marker, &PrintFlag);

    free (marker);

    if (Source.Arr != NULL)
    {
        free (Source.Arr);
    }

    if (Tokens != NULL)
    {
        //print_tokens (Tokens);

        destruct_tokens (Tokens);
    }

    if (Vars != NULL)
    {
        stack_destructor (Vars);
        //free (Vars);
    }

    if (Funcs != NULL)
    {
        destruct_funcs_table (Funcs, &PrintFlag);
    }

    if (Root != NULL)
    {
        make_graf_viz_tree (Root, "FRONTEND/GRAPH_VIZ/GraphViz_treeDump", false);

        write_tree (Root, "FILES/ParsedSrc.tr");

        delete_tree (&Root);
    }

    return;
}

//=============================================================================================================================================================================
//source//
//=============================================================================================================================================================================

CodeSource* construct_source (CodeSource* Source, FILE* SourceText)
{
    //Source->Code = (CharT*) calloc (MAX_MEMORY, sizeof (*(Source->Code)));
    //Source->size = MAX_MEMORY;

    fwscanf (SourceText, L"%ml[^~]", &Source->Arr); // Asked Danya how to remove this shit!! nikak
    //wprintf (L"|\n%ls|\n", Source->Arr);

    Source->ip   = 0;
    Source->size = wcslen (Source->Arr);

    //fread (Source->Code, MAX_MEMORY, sizeof (CharT), SourceText);
    //fwscanf (SourceText, L"%m[^.]", &(Source->Code));
    //wprintf (L"\n'%ls'\n", Source->Code);

    return Source;
}

//=============================================================================================================================================================================
//seekers//
//=============================================================================================================================================================================

void seek (CodeSource* Source)
{
    for (; Source->ip < Source->size; Source->ip++)
    {
        // if ((wcscmp (Buffer->Array[Buffer->ip], L"\n")) && ((wcscmp (Buffer->Array[Buffer->ip], L" "))))
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
        // if ((wcscmp (Buffer->Array[Buffer->ip], L"\n")) && ((wcscmp (Buffer->Array[Buffer->ip], L" "))))
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
        if (do_token (Source, Tokens, PrintFlag) != NoErr)
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

int do_token (CodeSource* Source,  AllTokens* Tokens, bool* PrintFlag)
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

    //Source->ip += wcslen (Lexem);
    //Source->ip++;

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
        if (Tokens->array[counter].category == CategoryLine)
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
    //wprintf (L"|%d|\n", *Counter);
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

void f_create_new_var_table (SStack<SVarTable*>* Vars, bool* PrintFlag)
{
    if (*PrintFlag)
    {
        wprintf (L"==%p== %s %s:%d\n", Vars, LOCATION);
    }

    SVarTable* NewTable = (SVarTable*)  calloc (1,                  sizeof (SVarTable));
    NewTable->Arr       = (SVarAccord*) calloc (VAR_TABLE_CAPACITY, sizeof (SVarAccord));
    NewTable->cur_size = 0;

    push_in_stack (Vars, NewTable);

    return;
}

void f_delete_var_table (SStack<SVarTable*>* Vars, bool* PrintFlag)
{
    if (*PrintFlag)
    {
        wprintf (L"==%p== %s %s:%d\n", Vars, LOCATION);
    }

    SVarTable* Table = NULL;

    pop_from_stack (Vars, &Table);

    free (Table->Arr);

    free (Table);

    return;
}

bool f_check_vars_table (CharT* Name, SStack<SVarTable*>* Vars, bool* PrintFlag)
{
    SVarTable* Table = NULL;

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

bool f_find_in_table (CharT* Name, SVarTable* Table, bool* PrintFlag)
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

void f_add_to_var_table (CharT* Name, SStack<SVarTable*>* Vars, bool* PrintFlag)
{
    if (*PrintFlag)
    {
        wprintf (L"==%p== %s %s:%d\n", Vars, LOCATION);
    }

    MY_LOUD_ASSERT (Name != NULL);

    // if (Back->table_cond == none)
    // {
    //     create_new_var_table (Back);
    // }

    SVarTable* Table = NULL;
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

SFuncs* construct_funcs_table (size_t Capacity)
{
    SFuncs* Funcs = (SFuncs*) calloc (1, sizeof (*Funcs));

    Funcs->Arr = (SFunc*) calloc (Capacity, sizeof (*Funcs->Arr));

    return Funcs;
}

int add_to_funcs_table (CharT* Name, SFuncs* Funcs)
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

void add_parameters (int Number, SNode* Node, SFuncs* Funcs)
{
    MY_LOUD_ASSERT (Funcs->Arr[Number].name != NULL);
    Funcs->Arr[Number].parameters = count_parameters (Node);

    return;
}

int count_parameters (SNode* Node)
{
    SNode* CurNode = Node;

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

bool check_funcs_table (CharT* Name, SFuncs* Funcs)
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

void show_funcs_table (SFuncs* Funcs, bool* PrintFlag)
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

void destruct_funcs_table (SFuncs* Funcs, bool* PrintFlag)
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

SNode* construct_op_node (TokenType Type)
{
    SNode* Node = (SNode*) calloc (1, sizeof (*Node));

    Node->category = CategoryOperation;

    Node->type = Type;

    return Node;
}

SNode* construct_var_node (Token* CurToken)
{
    SNode* Node = (SNode*) calloc (1, sizeof (*Node));

    Node->category = CategoryLine;

    Node->type = TypeVariable;

    MY_LOUD_ASSERT (CurToken->category == CategoryLine && CurToken->type == TypeVariable);

    Node->data.var = wcsdup (CurToken->data.var);

    return Node;
}

SNode* construct_val_node (ValT Value)
{
    SNode* Node = (SNode*) calloc (1, sizeof (*Node));

    Node->category = CategoryValue;

    Node->type = TValue;

    Node->data.val = Value;

    return Node;
}

void delete_tree (SNode** Node)
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

    //free (Node->data);

    //printf ("\n%p freed\n", *Node);

    if ((*Node)->category == CategoryLine)
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

SNode* get_All (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    SNode* Root = get_Statement (FUNC_ARGUMENTS);

    //make_gv_tree (Root, "GRAPH_VIZ/gvDiff_func.dot");

    return Root;
}

SNode* get_Statement (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    // if (TKN_OP_AND_IS__ TypeOpenBracket
    // && (Tokens->array[Tokens->number + 1].category == CategoryOperation
    // &&  Tokens->array[Tokens->number + 1].type == TypeCloseBracket))
    // {
    //     NEXT_TKN;
    //     NEXT_TKN;
    // }

    if (TKN_OP_AND_IS__ TypeFinish)
    {
        NEXT_TKN; //Tokens->number++;

        return get_Statement (FUNC_ARGUMENTS);
    }

    if (TKN_OP_AND_IS__ TypeCloseBracket)
    {
        NEXT_TKN; //Tokens->number++;

        f_delete_var_table (Vars, PrintFlag);

        return NULL;
    }

    SNode* Node = construct_op_node (TypeLinkerStatement);


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

SNode* get_Function (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (((TKN_OP_AND_IS__ TypeVoidVarType) || (TKN_OP_AND_IS__ TypeStdVarType)) &&
    (Tokens->array[Tokens->number + 1].category == CategoryLine) &&
    (Tokens->array[Tokens->number + 2].type == TypeOpenRoundBracket))
    {
        SNode* Node = construct_op_node (TypeLinkerFunction);

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

SNode* get_Head (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    SNode* Left_son = get_Type (FUNC_ARGUMENTS);

    SNode* Node = construct_var_node (&TOKEN);

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

SNode* get_Type (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    MTokAss ((TKN_OP_AND_IS__ TypeStdVarType) || (TKN_OP_AND_IS__ TypeVoidVarType));

    SNode* Node = construct_op_node (TOKEN.type);

    NEXT_TKN; //Tokens->number++;

    return Node;
}

SNode* get_Parameters (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    CHECK_SYNTAX (TypeOpenRoundBracket);

    SNode* Node = get_headParam (FUNC_ARGUMENTS);

    CHECK_SYNTAX (TypeCloseRoundBracket);

    return Node;
}

SNode* get_headParam (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    SNode* Node = construct_op_node (TypeLinkerParameter);

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

SNode* get_Param (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    SNode* Node = construct_op_node (TypeLinkerParameter);

    //MY_LOUD_ASSERT (f_check_vars_table (TOKEN.data.var, Vars) == true);

    Node->left = get_Expression (FUNC_ARGUMENTS, ZeroRetPermisiion);

    if (TKN_OP_AND_IS__ TypeComma)
    {
        NEXT_TKN; //Tokens->number++;

        Node->right = get_Param (FUNC_ARGUMENTS, false);

        //NEXT_TKN;

        return Node;
    }

    Node->right = NULL;

    return Node;
}

SNode* get_IfElse (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_OP_AND_IS__ TypeIf)
    {
        SNode* Node = construct_op_node (TOKEN.type);
        NEXT_TKN;


        CHECK_SYNTAX (TypeOpenRoundBracket);

        Node->left  = get_Expression (FUNC_ARGUMENTS, false);

        CHECK_SYNTAX (TypeCloseRoundBracket);

        Node->right = get_IfStatements (FUNC_ARGUMENTS);

        return Node;
    }

    return get_While (FUNC_ARGUMENTS);
}

SNode* get_IfStatements (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    SNode* Node = construct_op_node (TypeLinkerCrossroads);

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

SNode* get_While (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_OP_AND_IS__ TypeWhile)
    {
        SNode* Node = construct_op_node (TOKEN.type);
        NEXT_TKN;


        CHECK_SYNTAX (TypeOpenRoundBracket);

        Node->left  = get_Expression (FUNC_ARGUMENTS, false);

        CHECK_SYNTAX (TypeCloseRoundBracket);

        Node->right = get_Statement (FUNC_ARGUMENTS);

        return Node;
    }

    return get_Input (FUNC_ARGUMENTS);
}

SNode* get_Input (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_OP_AND_IS__ TypeInput)
    {
        SNode* Node = construct_op_node (TOKEN.type);
        NEXT_TKN;

        CHECK_SYNTAX (TypeOpenRoundBracket);

        Node->left = get_Param (FUNC_ARGUMENTS, false);

        CHECK_SYNTAX (TypeCloseRoundBracket);

        MY_LOUD_ASSERT (Node->left->left != NULL);

        return Node;
    }

    return get_Output (FUNC_ARGUMENTS);
}

SNode* get_Output (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_OP_AND_IS__ TypeOutput)
    {
        SNode* Node = construct_op_node (TOKEN.type);
        NEXT_TKN;

        CHECK_SYNTAX (TypeOpenRoundBracket);

        Node->left = get_Param (FUNC_ARGUMENTS, false);

        CHECK_SYNTAX (TypeCloseRoundBracket);

        MY_LOUD_ASSERT (Node->left->left != NULL);

        return Node;
    }

    return get_Return (FUNC_ARGUMENTS);
}

SNode* get_Return (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_OP_AND_IS__ TypeReturn)
    {
        SNode* Node = construct_op_node (TOKEN.type);
        NEXT_TKN;

        Node->left  = get_Expression (FUNC_ARGUMENTS, false);

        Node->right = NULL;

        return Node;
    }

    return get_Announce (FUNC_ARGUMENTS);
}

SNode* get_Announce (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_OP_AND_IS__ TypeStdVarType)
    {
        NEXT_TKN;

        SNode* Node = construct_op_node (TypeLinkerAnnounce);

        Node->left  = get_Variable (FUNC_ARGUMENTS);

        if (TKN_OP_AND_IS__ TypeAssign)
        {
            NEXT_TKN;

            Node->right = get_Expression (FUNC_ARGUMENTS, false);
        }

        // if (f_add_to_var_table (Node->left->data.var, Vars) == false)
        // if (f_check_vars_table (Node->left->data.var, Vars) == true)

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
        // if (Node->right != NULL && Node->right->category == CategoryValue && Node->right->type == TValue)
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

SNode* get_func_Announce (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_OP_AND_IS__ TypeStdVarType)
    {
        NEXT_TKN;

        SNode* Node = construct_op_node (TypeLinkerFuncAnnounce);

        Node->left  = get_Variable (FUNC_ARGUMENTS);

        // if (f_check_vars_table (Node->left->data.var, Vars) == true)
        if (f_find_in_table (Node->left->data.var, Vars->data[Vars->size - 1], PrintFlag) == true)
        {
            wprintf (L"==ERROR==\n""Variable '%ls' has been already announced!\n", Node->left->data.var);

            MY_LOUD_ASSERT (0);
        }
        else
        {
            f_add_to_var_table (Node->left->data.var, Vars, PrintFlag);
        }

//         if (TKN_OP_AND_IS__ TypeAssign)
//         {
//             NEXT_TKN; //Tokens->number++;
//
//             Node->right = get_Expression (FUNC_ARGUMENTS);
//         }

        // if (Node->right != NULL)
        // {
        //     add_to_vars_table (Node->left->data.var, Node->right->data.val, Vars);
        // }
        // else
        // {
        //     add_to_vars_table (Node->left->data.var, UNINITIALIZED, Vars);
        // }

        return Node;
    }

    return NULL;
}

SNode* get_Variable (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    SNode* Node = construct_var_node (&TOKEN);

    NEXT_TKN; //Tokens->number++;

    return Node;
}

SNode* get_Equation (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TKN_IS_WORD &&
    (Tokens->array[Tokens->number + 1].category == CategoryOperation &&
    Tokens->array[Tokens->number + 1].type == TypeAssign))
    {
        if (f_check_vars_table (TOKEN.data.var, Vars, PrintFlag) == true)
        {
            SNode* Node = construct_op_node (TypeLinkerEquation);

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

SNode* get_Call (FUNC_HEAD_ARGUMENTS)
{
    TOKEN_FUNC_DEBUG_INFO;

    if (TOKEN.category == CategoryLine && TOKEN.type == TypeVariable && check_funcs_table (TOKEN.data.var, Funcs) == true)
    {
        SNode* Node = construct_op_node (TypeLinkerCall);

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

SNode* get_Expression (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    SNode* Node = get_Logic (FUNC_ARGUMENTS, ZeroRetPermisiion);

    return Node;
}

SNode* get_Logic (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    SNode* LeftSon = get_Compare (FUNC_ARGUMENTS, ZeroRetPermisiion);

    if
    (
        (TKN_OP_AND_IS__ TypeLogicAnd)   ||
        (TKN_OP_AND_IS__ TypeLogicOr)   /* ||
        (TKN_OP_AND_IS__ TypeLogicNot) */
    )
    {
        TokenType CurrentType = TOKEN.type;
        NEXT_TKN;

        SNode* Node = construct_op_node (CurrentType);

        Node->left = LeftSon;

        Node->right = get_Logic (FUNC_ARGUMENTS, ZeroRetPermisiion);

        return Node;
    }

    return LeftSon;
}

SNode* get_Compare (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    SNode* LeftSon = get_AddSub (FUNC_ARGUMENTS, ZeroRetPermisiion);

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

        SNode* Node = construct_op_node (CurrentType);

        Node->left = LeftSon;

        Node->right = get_Compare (FUNC_ARGUMENTS, ZeroRetPermisiion);

        return Node;
    }

    return LeftSon;
}

SNode* get_AddSub (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    SNode* LeftSon = get_MulDiv (FUNC_ARGUMENTS, ZeroRetPermisiion);

    if ((TKN_OP_AND_IS__ TypeArithAdd)|| (TKN_OP_AND_IS__ TypeArithSub))
    {
        TokenType CurrentType = TOKEN.type;
        NEXT_TKN;

        SNode* Node = construct_op_node (CurrentType);

        Node->left = LeftSon;

        Node->right = get_AddSub (FUNC_ARGUMENTS, ZeroRetPermisiion);

        return Node;
    }

    return LeftSon;
}

SNode* get_MulDiv (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    SNode* LeftSon =  get_Pow (FUNC_ARGUMENTS, ZeroRetPermisiion);

    if ((TKN_OP_AND_IS__ TypeArithMul)|| (TKN_OP_AND_IS__ TypeArithDiv))
    {
        TokenType CurrentType = TOKEN.type;
        NEXT_TKN;

        SNode* Node = construct_op_node (CurrentType);

        Node->left = LeftSon;

        Node->right =  get_MulDiv (FUNC_ARGUMENTS, ZeroRetPermisiion);

        return Node;
    }

    return LeftSon;
}

SNode* get_Pow (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    SNode* LeftSon = get_Bracket (FUNC_ARGUMENTS, ZeroRetPermisiion);

    if (TKN_OP_AND_IS__ TypeArithPow)
    {
        TokenType CurrentType = TOKEN.type;
        NEXT_TKN;

        SNode* Node = construct_op_node (CurrentType);

        Node->left = LeftSon;

        Node->right = get_Pow (FUNC_ARGUMENTS, ZeroRetPermisiion);

        return Node;
    }

    return LeftSon;
}

SNode* get_Bracket (FUNC_HEAD_ARGUMENTS, bool ZeroRetPermisiion)
{
    TOKEN_FUNC_DEBUG_INFO;

    SNode* Node = NULL;

    if (TKN_OP_AND_IS__ TypeOpenRoundBracket)
    {
        NEXT_TKN;

        Node = get_Expression (FUNC_ARGUMENTS, ZeroRetPermisiion); //CHANGE IT ON GET_LOGIC IF CHANGES EXPRESSION

        CHECK_SYNTAX (TypeCloseRoundBracket);

        return Node;
    }

    if (TKN_IS_WORD)
    {
        if (Tokens->array[Tokens->number + 1].category == CategoryOperation &&
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

    if (TOKEN.category == CategoryValue && TOKEN.type == TValue)
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

//=============================================================================================================================================================================
//GraphViz//
//===================================================================================================================================================================

void make_graf_viz_tree (SNode* Root, const char* FileName, bool Display)
{
    FILE* gvInputFile = fopen (FileName, "w");
    MY_LOUD_ASSERT (gvInputFile != NULL);

    fprintf (gvInputFile,
        R"(digraph {
    rankdir = VR
    graph [splines = curved];
    bgcolor = "white";
    node [shape = "plaintext", style = "solid"];)");

    make_gv_node (gvInputFile, Root);

    fprintf (gvInputFile, "\n}\n");

    fclose (gvInputFile);

    show_gv_tree (FileName, Display);

    return;
}

void make_gv_node (FILE* File, SNode* Node)
{
    MY_LOUD_ASSERT (File != NULL);

    if (Node == NULL)
    {
        return;
    }

    fprintf (File,
        R"(

                    node_%p
                    [
                        label=
                        <
                        <table border="0" cellborder="1" cellspacing="0">
                            <tr>)", Node);

    print_gv_node (File, Node);

    fprintf (File,
            R"(</td>
                            </tr>)");

    // if (Node->left != NULL)
    // {
    //     fprintf(File,
    //     R"(
    //                         <tr>
    //                             <td bgcolor = "#61de4b" port="left" > %p   </td>
    //                             <td bgcolor = "#f27798" port="right"> %p   </td>
    //                         </tr>)", Node->left, Node->right);
    // }

    fprintf (File, R"(
                        </table>
                        >
                    ]
                    )");

    if (Node->left != NULL)
    {
        fprintf (File,  "\n                    node_%p -> node_%p;", Node, Node->left);
    }

    if (Node->right != NULL)
    {
        fprintf (File,  "\n                    node_%p -> node_%p;", Node, Node->right);
    }


    // if (Node->parent != NULL) //TOO !!!was working
    // {
    //     //wprintf (L"!%d!\n", Node->branch);
    //     if ((Node->parent != NULL) && (Node->parent->left == Node))
    //     {
    //         fprintf (File,  "\n                    node_%p -> node_%p;",
    //                     Node->parent, Node);
    //     }
    //     else if ((Node->parent != NULL) && (Node->parent->right == Node))
    //     {
    //         fprintf (File,  "\n                    node_%p -> node_%p;",
    //                     Node->parent, Node);
    //     }
    // }

    make_gv_node (File, Node->left);
    make_gv_node (File, Node->right);
}

void print_gv_node (FILE* File, SNode* Node)
{
    MY_LOUD_ASSERT (File != NULL);

    switch (Node->category)
    {
        case CategoryOperation:
            switch (Node->type)
            {
                #define DEF_OP(d_type, d_condition, d_tokenize, d_print, ...) \
                case d_type: \
                    fprintf (File, "<td colspan=\"2\" bgcolor = \"" GV_OP_COLOUR "\">"  " %ls ", d_print); \
                    break;

                #include "Operations.h"

                #undef DEF_OP

                default:
                MCA (0, (void) 0);
            }
            break;

        case CategoryLine:
            fprintf (File, "<td colspan=\"2\" bgcolor = \"" GV_VAR_COLOUR "\"> " VAR_SPEC " ", Node->data.var);
            break;

        case CategoryValue:
            fprintf (File, "<td colspan=\"2\" bgcolor = \"" GV_VAL_COLOUR "\"> " VAL_SPEC " ", Node->data.val);
            break;

        default:
            MCA (0, (void) 0);
    }

    return;
}

void show_gv_tree (const char* FileName, bool Display)
{
    size_t length = strlen (FileName) + 60;

    char* Command = (char*) calloc (sizeof (*FileName), length);

    sprintf (Command, "dot -Tpng %s -o %s.png", FileName, FileName);

    system (Command);

    if (Display)
    {
        sprintf (Command, "xdg-open %s.png", FileName);

        system (Command);
    }

    free (Command);

    return;
}

//===================================================================================================================================================================
//WriteTree//
//===================================================================================================================================================================

void write_tree (SNode* Root, const char* FileName)
{
    FILE* OutputFile = fopen (FileName, "w");
    MY_LOUD_ASSERT (OutputFile != NULL);

    file_wprint (Root, 0, OutputFile);

    fwprintf (OutputFile, L"" FORBIDDEN_TERMINATING_SYMBOL);

    fclose (OutputFile);

    return;
}

void file_wprint (SNode* Node, int n, FILE* OutputFile)
{
    if (Node == NULL)
    {
        return;
    }

    do_tab (n, OutputFile);

    print_node (Node, OutputFile);

    //if (Node->left != NULL)
    {
        do_tab (n, OutputFile);
        fwprintf (OutputFile, L"{\n");
        file_wprint (Node->left, n + 1, OutputFile);
        do_tab (n, OutputFile);
        fwprintf (OutputFile, L"}\n");
    }

    //if (Node->right != NULL)
    {
        //wprintf (L"i m here %ls\n", Node->data);
        do_tab (n, OutputFile);
        fwprintf (OutputFile, L"{\n");
        file_wprint (Node->right, n + 1, OutputFile);
        do_tab (n, OutputFile);
        fwprintf (OutputFile, L"}\n");
    }

    return;
}

void do_tab (int n, FILE* OutputFile)
{
    for (int i = 0; i < n; i++)
    {
        fwprintf (OutputFile, L"" TAB);
    }

    return;
}

void print_node (SNode* Node, FILE* OutputFile)
{
    switch (Node->category)
    {
        case CategoryValue:
            fwprintf (OutputFile, L"CategoryValue TValue %lg", Node->data.val);
            break;

        case CategoryLine:
            fwprintf (OutputFile, L"CategoryLine TypeVariable %ls", Node->data.var);
            break;

        case CategoryOperation:
            switch (Node->type)
            {
                #define DEF_OP(d_type, d_condition, d_tokenize, d_print, ...) \
                case d_type: \
                    fwprintf (OutputFile, L"CategoryOperation %ls", d_print); \
                    break;

                #include "Operations.h"

                #undef DEF_OP
            }
            break;
    }

    fwprintf (OutputFile, L"\n");
    return;
}

//===================================================================================================================================================================

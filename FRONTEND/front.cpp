//=============================================================================================================================================================================

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <locale.h>
#include <wchar.h>

//===================================================================================================================================================================

#include "MYassert.h"
#include "colors.h"

//=============================================================================================================================================================================

#include "front.h"

//=============================================================================================================================================================================

// #define SHOUT

#define BASE_SCOPES_NUM 20

#define MAX_MEMORY  10000

#define SEP_SYMBOLS "\n;,() "

#define UNINITIALIZED   NAN

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

const int StdErr = 1;
const int NoErr  = 0;

//=============================================================================================================================================================================

void my_f_main (int argc, char** argv)
{
    setlocale(LC_CTYPE, "");

    FILE* SourceText = (argc > 1) ? fopen (argv[1], "r") : fopen ("EXAMPLES/first_prog.ger", "r") ;
    MLA (SourceText != NULL);

    SSrc Source = *(construct_source (&Source, SourceText));

    fclose (SourceText);

    STokens* Tokens = lex_src (&Source);

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

    SNode* Root = get_All (FUNC_ARGUMENTS);

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
        free (Vars);
    }

    if (Funcs != NULL)
    {
        destruct_funcs_table (Funcs);
    }

    if (Root != NULL)
    {
        make_gv_tree (Root, "FRONTEND/GRAPH_VIZ/GraphViz_treeDump");

        write_tree (Root, "FILES/ParsedSrc.tr");

        delete_tree (&Root);
    }

    return;
}

//=============================================================================================================================================================================
//source//
//=============================================================================================================================================================================

SSrc* construct_source (SSrc* Source, FILE* SourceText)
{
    //Source->Code = (CharT*) calloc (MAX_MEMORY, sizeof (*(Source->Code)));
    //Source->size = MAX_MEMORY;

    fwscanf (SourceText, L"%ml[^~]", &Source->Arr); // TODO Ask Danya how to remove this shit!!
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

void seek (SSrc* Source)
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

void seek_out (SSrc* Source)
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

STokens* lex_src (SSrc* Source)
{
    STokens* Tokens  = (STokens*) calloc (1, sizeof (*Tokens));
    Tokens->TokenArr = (SToken*)  calloc (Source->size, sizeof (*Tokens->TokenArr));

    //wprintf (L"%lu==%lu==\n", Source->ip, Source->size);

    while (Source->ip < Source->size)
    {
        if (do_token (Source, Tokens) != NoErr)
        {
            free (Tokens->TokenArr);

            return NULL;
        }
    }

    wprintf (L"Created Tokens Array. size |%d|\n", Tokens->number);

    return Tokens;
}

int do_token (SSrc* Source,  STokens* Tokens)
{
    seek (Source);
    CharT* Lexem = NULL;

    swscanf (&(Source->Arr[Source->ip]), L"%ml[^" SEP_SYMBOLS "]", &Lexem);

    if (Lexem == NULL)
    {
        swscanf (&(Source->Arr[Source->ip]), L"%ml[^\n ]", &Lexem);

        if (Lexem == NULL && Source->Arr[Source->ip] == L'\0')
        {
            return NoErr;
        }

        //wprintf (L"!%lc!\n", Source->Code[Source->ip]);
    }

    MTokAss (Lexem != NULL);

    //wprintf (L"\n++%ls++\n", Lexem);
    //wprintf (L"\n++%lc++\n", *Lexem);

    if (parse_token (Lexem, Tokens) != NoErr)
    {
        free (Lexem);

        return StdErr;
    }

    Source->ip += wcslen (Lexem);

    free (Lexem);

    return NoErr;
}

#define DEF_OP(d_type, d_condition, d_tokenize,...) \
else if (d_condition) \
{ \
    wprintf (L"It is:  " #d_type "\n"); \
    d_tokenize \
    TKN.type = d_type; \
    Tokens->number++; \
}

int parse_token (CharT* Lexem, STokens* Tokens)
{
    wprintf (L"[%d]\n""I see: '%ls'\n", Tokens->number, Lexem);

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

void print_tokens (STokens* Tokens)
{
    wprintf (L"==Tokens=array==\n");

    for (int counter = 0; counter < Tokens->number; ++counter)
    {
        wprintf (L"[%.3d] %d %d", counter, Tokens->TokenArr[counter].category, Tokens->TokenArr[counter].type);

        //print_token (Tokens->TokenArr[counter]);

        wprintf (L"\n");
    }

    return;
}

void destruct_tokens (STokens* Tokens)
{
    //wprintf (L"|%d|\n", Tokens->number);
    for (int counter = 0; counter < Tokens->size; ++counter)
    {
        if (Tokens->TokenArr[counter].category == CLine)
        {
            free (Tokens->TokenArr[counter].data.var);
        }
    }

    free (Tokens->TokenArr);
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

    MLA (*Counter > OldPtr);

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

    MLA (*Counter > OldPtr);

    return Value;
}

//=============================================================================================================================================================================
//front stuff//
//=============================================================================================================================================================================

void f_create_new_var_table (SStack<SVarTable*>* Vars)
{
    wprintf (L"==%p== %s %s:%d\n", Vars, LOCATION);

    SVarTable* NewTable = (SVarTable*)  calloc (1,                  sizeof (SVarTable));
    NewTable->Arr       = (SVarAccord*) calloc (VAR_TABLE_CAPACITY, sizeof (SVarAccord));
    NewTable->size = 0;

    push_in_stack (Vars, NewTable);

    return;
}

void f_delete_var_table (SStack<SVarTable*>* Vars)
{
    wprintf (L"==%p== %s %s:%d\n", Vars, LOCATION);

    SVarTable* Table = NULL;

    pop_from_stack (Vars, &Table);

    free (Table->Arr);

    free (Table);

    return;
}

bool f_check_vars_table (CharT* Name, SStack<SVarTable*>* Vars)
{
    SVarTable* Table = NULL;

    int depth = 1;
    do
    {
        Table = Vars->data[Vars->size - depth];
        depth++;

        wprintf (L"!!%ls!!\n", Name);

        if (f_find_in_table (Name, Table) == true)
        {
            return true;
        }

        wprintf (L"^^%ls^^\n", Name);


    } while (depth <= Vars->size);

    return false;
}

bool f_find_in_table (CharT* Name, SVarTable* Table)
{
    MLA (Table != NULL);

    for (int counter = 0; counter < Table->size; ++counter)
    {
        wprintf (L"%d--%d--%ls--\n", Table->size, counter, Table->Arr[counter].name);
        if (wcscmp (Name, Table->Arr[counter].name) == 0)
        {
            return true;
        }
    }

    return false;
}

void f_add_to_var_table (CharT* Name, SStack<SVarTable*>* Vars)
{
    wprintf (L"==%p== %s %s:%d\n", Vars, LOCATION);

    MLA (Name != NULL);

    // if (Back->table_cond == none)
    // {
    //     create_new_var_table (Back);
    // }

    SVarTable* Table = NULL;
    peek_from_stack (Vars, &Table);

    Table->Arr[Table->size].name = Name; //copy address from node
    wprintf (L"added '%ls'\n", Table->Arr[Table->size].name);
    Table->size++;

    return;
}

// SVars* construct_vars_table (size_t Capacity)
// {
//     SVars* Vars  = (SVars*) calloc (1, sizeof (*Vars));
//     Vars->Arr    = (SVar*)  calloc (Capacity, sizeof (*(Vars->Arr)));
//     Vars->size   = 0;
//
//     MLA (Vars != NULL);
//     MLA (Vars->Arr != NULL);
//
//     return Vars;
// }
//
// void show_vars_table (SVars* Vars)
// {
//     wprintf (L"==Vars Array== size %d\n", Vars->size);
//     for (int counter = 0; counter < MAX_VARS_ARRAY; ++counter)
//     {
//         if (VAR.name != NULL)
//         {
//             wprintf (L"[%d] %ls = %lg\n", counter, VAR.name, VAR.value);
//         }
//     }
// }
//
// bool check_vars_table (CharT* Name, SVars* Vars)
// {
//     for (int counter = 0; counter < Vars->size; counter++)
//     {
//         if (wcscmp (VAR.name, Name) == 0)
//         {
//             return true;
//         }
//     }
//
//     return false;
// }
//
// bool add_to_vars_table (CharT* Name, ValT Data, SVars* Vars)
// {
//         for (int counter = 0; counter < MAX_VARS_ARRAY; counter++)
//         {
//             if ( VAR.name != NULL && wcscmp (VAR.name, Name) == 0)
//             {
//                 return false;
//             }
//
//             if (VAR.name == NULL)
//             {
//                 VAR.name = wcsdup (Name);
//
//                 VAR.value = Data;
//
//                 Vars->size++;
//
//                 return true;
//             }
//         }
//
//         wprintf (L"2much vars\n");
//         return false;
// }

// void destruct_vars_table (SVars* Vars)
// {
//     //show_vars_table (Vars);
//
//     for (int counter = 0; counter <= Vars->size; ++counter)
//     {
//         if (Vars->Arr[counter].name != NULL)
//         {
//             free (Vars->Arr[counter].name);
//             Vars->Arr[counter].name = NULL;
//         }
//     }
//
//     free (Vars->Arr);
//     Vars->Arr = NULL;
//
//     free (Vars);
//     Vars = NULL;
//
//     return;
// }

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

SFuncs* construct_funcs_table (size_t Capacity)
{
    SFuncs* Funcs = (SFuncs*) calloc (1, sizeof (*Funcs));

    Funcs->Arr = (SFunc*) calloc (Capacity, sizeof (*Funcs->Arr));

    return Funcs;
}

bool add_to_funcs_table (CharT* Name, SFuncs* Funcs)
{
        for (int counter = 0; counter < MAX_FUNCS_ARRAY; counter++)
        {
            if ( FUNC.name != NULL && wcscmp (FUNC.name, Name) == 0)
            {
                return false;
            }

            if (FUNC.name == NULL)
            {
                FUNC.name = wcsdup (Name);

                Funcs->size++;

                return true;
            }
        }

        wprintf (L"2much funcs\n");
        return false;
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

void show_funcs_table (SFuncs* Funcs)
{
    wprintf (L"==Funcs Array== size %d\n", Funcs->size);

    for (int counter = 0; counter <= Funcs->size; ++counter)
    {
        if (FUNC.name != NULL)
        {
            wprintf (L"[%d] %ls\n", counter, FUNC.name);
        }
    }
}

void destruct_funcs_table (SFuncs* Funcs)
{
    show_funcs_table (Funcs);

    for (int counter = 0; counter <= Funcs->size; ++counter)
    {
        if (Funcs->Arr[counter].name != NULL)
        {
            free (Funcs->Arr[counter].name);
            Funcs->Arr[counter].name = NULL;

            free (Funcs->Arr[counter].code);
            Funcs->Arr[counter].code = NULL;
        }
    }

    free (Funcs->Arr);
    Funcs->Arr = NULL;

    free (Funcs);
    Funcs = NULL;

    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

SNode* construct_op_node (ETokenType Type)
{
    SNode* Node = (SNode*) calloc (1, sizeof (*Node));

    Node->category = COperation;

    Node->type = Type;

    return Node;
}

SNode* construct_var_node (SToken* Token)
{
    SNode* Node = (SNode*) calloc (1, sizeof (*Node));

    Node->category = CLine;

    Node->type = TVariable;

    MLA (Token->category == CLine && Token->type == TVariable);

    Node->data.var = wcsdup (Token->data.var);

    return Node;
}

SNode* construct_val_node (ValT Value)
{
    SNode* Node = (SNode*) calloc (1, sizeof (*Node));

    Node->category = CValue;

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

    if ((*Node)->category == CLine)
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
    SHOUT;

    SNode* Root = get_Statement (FUNC_ARGUMENTS);

    //make_gv_tree (Root, "GRAPH_VIZ/gvDiff_func.dot");

    return Root;
}

SNode* get_Statement (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    // if (TKN_OP_AND_IS__ TOpenBracket
    // && (Tokens->TokenArr[Tokens->number + 1].category == COperation
    // &&  Tokens->TokenArr[Tokens->number + 1].type == TCloseBracket))
    // {
    //     NEXT_TKN;
    //     NEXT_TKN;
    // }

    if (TKN_OP_AND_IS__ TFinish)
    {
        NEXT_TKN; //Tokens->number++;

        return get_Statement (FUNC_ARGUMENTS);
    }

    if (TKN_OP_AND_IS__ TCloseBracket)
    {
        NEXT_TKN; //Tokens->number++;

        f_delete_var_table (Vars);

        return NULL;
    }

    SNode* Node = construct_op_node (T_Statement);


    Node->left  = get_Function (FUNC_ARGUMENTS);

    if (TKN_OP_AND_IS__ TOpenBracket)
    {
        if (*marker > 0)
        {
            (*marker)--;
        }
        else
        {
            f_create_new_var_table (Vars);
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
    SHOUT;

    if (((TKN_OP_AND_IS__ TnoType) || (TKN_OP_AND_IS__ TstdType)) &&
    (Tokens->TokenArr[Tokens->number + 1].category == CLine) &&
    (Tokens->TokenArr[Tokens->number + 2].type == TOpenRoundBracket))
    {
        SNode* Node = construct_op_node (T_Function);

        // if (Vars != NULL && (Vars->size > 0))
        // {
        //     f_delete_var_table (Vars);
        // }
        f_create_new_var_table (Vars);

        (*marker)++;

        Node->left  = get_Head (FUNC_ARGUMENTS);

        Node->right = get_Statement (FUNC_ARGUMENTS);

        return Node;
    }

    return get_IfElse (FUNC_ARGUMENTS);
}

SNode* get_Head (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    SNode* Left_son = get_Type (FUNC_ARGUMENTS);

    SNode* Node = construct_var_node (&TKN);

    if (add_to_funcs_table (TKN.data.var, Funcs) == false)
    {
        wprintf (L"Loshara\n");
    }

    NEXT_TKN; //Tokens->number++;

    Node->left  = Left_son;

    Node->right = get_Parameters (FUNC_ARGUMENTS);

    return Node;
}

SNode* get_Type (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    MTokAss ((TKN_OP_AND_IS__ TstdType) || (TKN_OP_AND_IS__ TnoType));

    SNode* Node = construct_op_node (TKN.type);

    NEXT_TKN; //Tokens->number++;

    return Node;
}

SNode* get_Parameters (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    CHECK_SYNTAX (TOpenRoundBracket);

    SNode* Node = get_headParam (FUNC_ARGUMENTS);

    CHECK_SYNTAX (TCloseRoundBracket);

    return Node;
}

SNode* get_headParam (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    SNode* Node = construct_op_node (T_Param);

    Node->left = get_func_Announce (FUNC_ARGUMENTS);

    if (TKN_OP_AND_IS__ TComma)
    {
        NEXT_TKN; //Tokens->number++;

        Node->right = get_headParam (FUNC_ARGUMENTS);

        return Node;
    }

    Node->right = NULL;

    return Node;
}

SNode* get_Param (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    SNode* Node = construct_op_node (T_Param);

    //MLA (f_check_vars_table (TKN.data.var, Vars) == true);

    Node->left = get_Bracket (FUNC_ARGUMENTS);

    if (TKN_OP_AND_IS__ TComma)
    {
        NEXT_TKN; //Tokens->number++;

        Node->right = get_Param (FUNC_ARGUMENTS);

        return Node;
    }

    Node->right = NULL;

    return Node;
}

SNode* get_IfElse (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    if (TKN_OP_AND_IS__ TIf)
    {
        SNode* Node = construct_op_node (TKN.type);
        NEXT_TKN;


        CHECK_SYNTAX (TOpenRoundBracket);

        Node->left  = get_Expression (FUNC_ARGUMENTS);

        CHECK_SYNTAX (TCloseRoundBracket);

        Node->right = get_IfStatements (FUNC_ARGUMENTS);

        return Node;
    }

    return get_While (FUNC_ARGUMENTS);
}

SNode* get_IfStatements (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    SNode* Node = construct_op_node (T_Crossroads);

    Node->left = get_Statement (FUNC_ARGUMENTS);

    if (TKN_OP_AND_IS__ TElse)
    {
        NEXT_TKN;

        if (TKN_OP_AND_IS__ TIf)
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
    SHOUT;

    if (TKN_OP_AND_IS__ TWhile)
    {
        SNode* Node = construct_op_node (TKN.type);
        NEXT_TKN;


        CHECK_SYNTAX (TOpenRoundBracket);

        Node->left  = get_Expression (FUNC_ARGUMENTS);

        CHECK_SYNTAX (TCloseRoundBracket);

        Node->right = get_Statement (FUNC_ARGUMENTS);

        return Node;
    }

    return get_Input (FUNC_ARGUMENTS);
}

SNode* get_Input (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    if (TKN_OP_AND_IS__ TInput)
    {
        SNode* Node = construct_op_node (TKN.type);
        NEXT_TKN;

        CHECK_SYNTAX (TOpenRoundBracket);

        Node->left = get_Param (FUNC_ARGUMENTS);

        CHECK_SYNTAX (TCloseRoundBracket);

        return Node;
    }

    return get_Output (FUNC_ARGUMENTS);
}

SNode* get_Output (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    if (TKN_OP_AND_IS__ TOutput)
    {
        SNode* Node = construct_op_node (TKN.type);
        NEXT_TKN;

        CHECK_SYNTAX (TOpenRoundBracket);

        Node->left = get_Param (FUNC_ARGUMENTS);

        CHECK_SYNTAX (TCloseRoundBracket);

        return Node;
    }

    return get_Return (FUNC_ARGUMENTS);
}

SNode* get_Return (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    if (TKN_OP_AND_IS__ TReturn)
    {
        SNode* Node = construct_op_node (TKN.type);
        NEXT_TKN;

        Node->left  = get_Expression (FUNC_ARGUMENTS);

        Node->right = NULL;

        return Node;
    }

    return get_Announce (FUNC_ARGUMENTS);
}

SNode* get_Announce (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    if (TKN_OP_AND_IS__ TstdType)
    {
        NEXT_TKN;

        SNode* Node = construct_op_node (T_Announce);

        Node->left  = get_Variable (FUNC_ARGUMENTS);

        if (TKN_OP_AND_IS__ TAssign)
        {
            NEXT_TKN;

            Node->right = get_Expression (FUNC_ARGUMENTS);
        }

        // if (f_add_to_var_table (Node->left->data.var, Vars) == false)
        // if (f_check_vars_table (Node->left->data.var, Vars) == true)
        if (f_find_in_table (Node->left->data.var, Vars->data[Vars->size - 1]) == true)
        {
            wprintf (L"==ERROR==\n""Variable '%ls' has been already announced!\n", Node->left->data.var);

            MLA (0);
        }
        else
        {
            f_add_to_var_table (Node->left->data.var, Vars);
        }

        //TODO do this in middle end later
        // if (Node->right != NULL && Node->right->category == CValue && Node->right->type == TValue)
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

        CHECK_SYNTAX (TFinish);

        return Node;
    }

    return get_Equation (FUNC_ARGUMENTS);
}

SNode* get_func_Announce (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    if (TKN_OP_AND_IS__ TstdType)
    {
        NEXT_TKN;

        SNode* Node = construct_op_node (T_func_Announce);

        Node->left  = get_Variable (FUNC_ARGUMENTS);

        // if (f_check_vars_table (Node->left->data.var, Vars) == true)
        if (f_find_in_table (Node->left->data.var, Vars->data[Vars->size - 1]) == true)
        {
            wprintf (L"==ERROR==\n""Variable '%ls' has been already announced!\n", Node->left->data.var);

            MLA (0);
        }
        else
        {
            f_add_to_var_table (Node->left->data.var, Vars);
        }

//         if (TKN_OP_AND_IS__ TAssign)
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
    SHOUT;

    SNode* Node = construct_var_node (&TKN);

    NEXT_TKN; //Tokens->number++;

    return Node;
}

SNode* get_Equation (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    if (TKN_IS_WORD &&
    (Tokens->TokenArr[Tokens->number + 1].category == COperation &&
    Tokens->TokenArr[Tokens->number + 1].type == TAssign))
    {
        if (f_check_vars_table (TKN.data.var, Vars) == true)
        {
            SNode* Node = construct_op_node (T_Equation);

            Node->left  = construct_var_node (&TKN);
            NEXT_TKN;

            //here is TAssign
            NEXT_TKN;

            Node->right = get_Expression (FUNC_ARGUMENTS);

            CHECK_SYNTAX (TFinish);

            return Node;
        }

        wprintf (L"==ERROR==\n""No such word '%ls' found!\n", TKN.data.var);

        MLA (0);
    }

    return get_Call (FUNC_ARGUMENTS);
}

SNode* get_Call (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;



    return NULL;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

SNode* get_Expression (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    SNode* Node = get_Logic (FUNC_ARGUMENTS);

    return Node;
}

SNode* get_Logic (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    SNode* LeftSon = get_Compare (FUNC_ARGUMENTS);

    if
    (
        (TKN_OP_AND_IS__ TcAnd)   ||
        (TKN_OP_AND_IS__ TcOr)   /* ||
        (TKN_OP_AND_IS__ TcNot) */
    )
    {
        ETokenType CurrentType = TKN.type;
        NEXT_TKN;

        SNode* Node = construct_op_node (CurrentType);

        Node->left = LeftSon;

        Node->right = get_Logic (FUNC_ARGUMENTS);

        return Node;
    }

    return LeftSon;
}

SNode* get_Compare (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    SNode* LeftSon = get_AddSub (FUNC_ARGUMENTS);

    if
    (
        (TKN_OP_AND_IS__ TcEqual)   ||
        (TKN_OP_AND_IS__ TcLess)    ||
        (TKN_OP_AND_IS__ TcMore)    ||
        (TKN_OP_AND_IS__ TcLessEq)  ||
        (TKN_OP_AND_IS__ TcMoreEq)  ||
        (TKN_OP_AND_IS__ TcNotEq)
    )
    {
        ETokenType CurrentType = TKN.type;
        NEXT_TKN;

        SNode* Node = construct_op_node (CurrentType);

        Node->left = LeftSon;

        Node->right = get_Compare (FUNC_ARGUMENTS);

        return Node;
    }

    return LeftSon;
}

SNode* get_AddSub (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    SNode* LeftSon = get_MulDiv (FUNC_ARGUMENTS);

    if ((TKN_OP_AND_IS__ TaAdd)|| (TKN_OP_AND_IS__ TaSub))
    {
        ETokenType CurrentType = TKN.type;
        NEXT_TKN;

        SNode* Node = construct_op_node (CurrentType);

        Node->left = LeftSon;

        Node->right = get_AddSub (FUNC_ARGUMENTS);

        return Node;
    }

    return LeftSon;
}

SNode* get_MulDiv (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    SNode* LeftSon =  get_Pow (FUNC_ARGUMENTS);

    if ((TKN_OP_AND_IS__ TaMul)|| (TKN_OP_AND_IS__ TaDiv))
    {
        ETokenType CurrentType = TKN.type;
        NEXT_TKN;

        SNode* Node = construct_op_node (CurrentType);

        Node->left = LeftSon;

        Node->right =  get_MulDiv (FUNC_ARGUMENTS);

        return Node;
    }

    return LeftSon;
}

SNode* get_Pow (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    SNode* LeftSon = get_Bracket (FUNC_ARGUMENTS);

    if (TKN_OP_AND_IS__ TaPow)
    {
        ETokenType CurrentType = TKN.type;
        NEXT_TKN;

        SNode* Node = construct_op_node (CurrentType);

        Node->left = LeftSon;

        Node->right = get_Pow (FUNC_ARGUMENTS);

        return Node;
    }

    return LeftSon;
}

SNode* get_Bracket (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    SNode* Node = NULL;

    if (TKN_OP_AND_IS__ TOpenRoundBracket)
    {
        NEXT_TKN;

        Node = get_Expression (FUNC_ARGUMENTS); //CHANGE IT ON GET_LOGIC IF CHANGES EXPRESSION

        CHECK_SYNTAX (TCloseRoundBracket);

        return Node;
    }

    if (TKN_IS_WORD)
    {
        if (Tokens->TokenArr[Tokens->number + 1].category == COperation &&
            Tokens->TokenArr[Tokens->number + 1].type     == TOpenRoundBracket)
        {
            #define DEF_UN(def_name, def_type) \
            else if (wcscmp (def_name, TKN.data.var) == 0) \
            { \
                Node = construct_op_node (def_type); \
                NEXT_TKN; /* skipped func name */ \
                NEXT_TKN; /* skipped ( */ \
                Node->right = get_Expression (FUNC_ARGUMENTS); /* //TODO HERE!!! */ \
                CHECK_SYNTAX (TCloseRoundBracket); \
                return Node; \
            }

            if (0) {}

            #include "BaseUnaryFunc.h"

            #undef DEF_UN



            if (check_funcs_table (TKN.data.var, Funcs) == true) //custom func check
            {
                // SNode* Node = construct_var_node (&TKN);
                SNode* Node = construct_op_node (T_Call);

                Node->left = construct_var_node (&TKN);

                NEXT_TKN;

                CHECK_SYNTAX (TOpenRoundBracket);

                Node->right = get_Param (FUNC_ARGUMENTS);

                CHECK_SYNTAX (TCloseRoundBracket);

                return Node;

//                 NEXT_TKN; //skipped func name
//
//                 NEXT_TKN; //skipped (
//
//                 Node = get_Bracket (FUNC_ARGUMENTS); //TODO HERE!!!
//
//                 CHECK_SYNTAX (TCloseRoundBracket);
//
//                 return Node;
            }
        }

        if (f_check_vars_table (TKN.data.var, Vars) == true)
        {
            Node = construct_var_node (&TKN);

            NEXT_TKN;

            return Node;
        }

        wprintf (L"==ERROR==\n""No such word '%ls' found!\n", TKN.data.var);
    }

    //TODO if func

    if (TKN.category == CValue && TKN.type == TValue)
    {
        Node = construct_val_node (TKN.data.val);
        NEXT_TKN;

        return Node;
    }

    wprintf (L"ERROR!\n");
    MTokAss (0);

    return NULL;
}

//=============================================================================================================================================================================
//GraphViz//
//===================================================================================================================================================================

void make_gv_tree (SNode* Root, const char* FileName)
{
    FILE* gvInputFile = fopen (FileName, "w");
    MLA (gvInputFile != NULL);

    fprintf (gvInputFile,
        R"(digraph {
    rankdir = VR
    graph [splines = curved];
    bgcolor = "white";
    node [shape = "plaintext", style = "solid"];)");

    make_gv_node (gvInputFile, Root);

    fprintf (gvInputFile, "\n}\n");

    fclose (gvInputFile);

    draw_gv_tree (FileName);

    return;
}

void make_gv_node (FILE* File, SNode* Node)
{
    MLA (File != NULL);

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


    // if (Node->parent != NULL) //TODO !!!was working
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
    MLA (File != NULL);

    switch (Node->category)
    {
        case COperation:
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

        case CLine:
            fprintf (File, "<td colspan=\"2\" bgcolor = \"" GV_VAR_COLOUR "\"> " VAR_SPEC " ", Node->data.var);
            break;

        case CValue:
            fprintf (File, "<td colspan=\"2\" bgcolor = \"" GV_VAL_COLOUR "\"> " VAL_SPEC " ", Node->data.val);
            break;

        default:
            MCA (0, (void) 0);
    }

    return;
}

void draw_gv_tree (const char* FileName)
{
    size_t length = strlen (FileName) + 60;

    char* Command = (char*) calloc (sizeof (*FileName), length);

    sprintf (Command, "dot -Tpng %s -o %s.png", FileName, FileName);
    //system ("dot -Tpng gvDiff.dot -o gvDiff.png");
    system (Command);

    //sprintf (Command, "xdg-open %s.png", FileName);

    //system ("xdg-open 1.png");
    system (Command);

    free (Command);

    return;
}

//===================================================================================================================================================================
//WriteTree//
//===================================================================================================================================================================

void write_tree (SNode* Root, const char* FileName)
{
    FILE* OutputFile = fopen (FileName, "w");
    MLA (OutputFile != NULL);

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
        case CValue:
            fwprintf (OutputFile, L"CValue TValue %lg", Node->data.val);
            break;

        case CLine:
            fwprintf (OutputFile, L"CLine TVariable %ls", Node->data.var);
            break;

        case COperation:
            switch (Node->type)
            {
                #define DEF_OP(d_type, d_condition, d_tokenize, d_print, ...) \
                case d_type: \
                    fwprintf (OutputFile, L"COperation %ls", d_print); \
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

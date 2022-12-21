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

#define MAX_MEMORY 10000

#define MAX_VARS_ARRAY  50
#define MAX_FUNCS_ARRAY 50

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

const int StdErr = 1;
const int NoErr  = 0;

//=============================================================================================================================================================================

void my_main (void)
{
    setlocale(LC_CTYPE, "");

    FILE* SourceText = fopen ("EXAMPLES/first_prog.ger", "r");
    MLA (SourceText != NULL);

    SSrc Source = *(construct_source (&Source, SourceText));

    fclose (SourceText);

    STokens* Tokens = lex_src (&Source);

    //print_tokens (Tokens);


    Tokens->size   = Tokens->number;
    Tokens->number = 0;


    SVars* Vars = construct_vars_array (MAX_VARS_ARRAY);

    SFuncs* Funcs = construct_funcs_array (MAX_FUNCS_ARRAY);


    SNode* Root = get_All (FUNC_ARGUMENTS);

    if (Source.Code != NULL)
    {
        free (Source.Code);
    }

    if (Tokens != NULL)
    {
        print_tokens (Tokens);

        destruct_tokens (Tokens);
    }

    if (Vars != NULL)
    {
        destruct_vars_array (Vars);
    }

    if (Funcs != NULL)
    {
        destruct_funcs_array (Funcs);
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

    fwscanf (SourceText, L"%ml[^~]", &Source->Code); // TODO Ask Danya how to remove this shit!!
    wprintf (L"|\n%ls|\n", Source->Code);

    Source->ip   = 0;
    Source->size = wcslen (Source->Code);

    //fread (Source->Code, MAX_MEMORY, sizeof (CharT), SourceText);
    //fwscanf (SourceText, L"%m[^.]", &(Source->Code));
    //wprintf (L"\n'%ls'\n", Source->Code);

    return Source;
}

//=============================================================================================================================================================================
<<<<<<< HEAD
//lexer//
//=============================================================================================================================================================================

SToken* lex_src (SSrc* Source)
{
    SToken* TokenArr = (SToken*) calloc (Source->size, sizeof (*TokenArr));

    wprintf (L"%lu==%lu==\n", Source->ip, Source->size);

    while (Source->ip < Source->size)
    {
        if (do_token (Source, TokenArr) != NoErr)
        {
            free (TokenArr);

            return NULL;
        }
    }

    return TokenArr;
}

int do_token (SSrc* Source, SToken* TokenArr)
{
    seek (Source);
    CharT* Lexem = NULL;

    swscanf (&(Source->Code[Source->ip]), L"%ml[^\n; ]", &Lexem);
    MLA (Lexem != NULL);

    wprintf (L"\n++%ls++\n", Lexem);
    wprintf (L"\n++%lc++\n", *Lexem);

    if (parse_token (Lexem, TokenArr) != NoErr)
    {
        free (Lexem);

        return StdErr;
    }

    Source->ip += wcslen (Lexem); //TODO CHANGE IF CHAR_T IS NOT WCHAR_T !!! T

    free (Lexem);

    return NoErr;
}

int parse_token (CharT* Lexem, SToken* TokenArr)
{
    wprintf (L"tkn '%ls'\n", Lexem);

    #define DEF_LEX(p_condition, p_tokenize) \
    else if (p_condition) \
    { \
        p_tokenize \
    }

    if (0) {}
    #include "Lexer.h"
    else
    {
        wprintf (L"Lexer default error. Word '%ls'\n", Lexem);
        return StdErr;
    }

    #undef DEF_LEX

    return 0;
}

=======
//seekers//
>>>>>>> e5a8470 (Half of Recoursive descent 0.2)
//=============================================================================================================================================================================

void seek (SSrc* Source)
{
    for (; Source->ip < Source->size; Source->ip++)
    {
        // if ((wcscmp (Buffer->Array[Buffer->ip], L"\n")) && ((wcscmp (Buffer->Array[Buffer->ip], L" "))))
        if ((Source->Code[Source->ip] != L'\n') && ((Source->Code[Source->ip] != L' ')))
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
        if ((Source->Code[Source->ip] == L'\n'))// || ((Buffer->Array[Buffer->ip] == L' ')))
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

    swscanf (&(Source->Code[Source->ip]), L"%ml[^\n; ]", &Lexem);

    if (Lexem == NULL)
    {
        swscanf (&(Source->Code[Source->ip]), L"%ml[^\n ]", &Lexem);

        if (Lexem == NULL && Source->Code[Source->ip] == L'\0')
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

#define DEF_LEX(d_type, d_condition, d_tokenize) \
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
    #include "Lexer.h"
    else
    {
        wprintf (L"Lexer default error. Word '%ls'\n", Lexem);
        return StdErr;
    }

    #undef DEF_LEX

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
//grammar//
//=============================================================================================================================================================================

SVars* construct_vars_array (size_t Capacity)
{
    SVars* Vars = (SVars*) calloc (1, sizeof (*Vars));

    Vars->Arr = (SVar*) calloc (Capacity, sizeof (*Vars->Arr));

    return Vars;
}

void destruct_vars_array (SVars* Vars)
{
    for (int counter = 0; counter < Vars->number; ++counter)
    {
        if (Vars->Arr[counter].name != NULL)
        {
            free (Vars->Arr[counter].name);
            Vars->Arr[counter].name = NULL;
        }
    }

    free (Vars->Arr);
    Vars->Arr = NULL;

    free (Vars);
    Vars = NULL;

    return;
}

SFuncs* construct_funcs_array (size_t Capacity)
{
    SFuncs* Funcs = (SFuncs*) calloc (1, sizeof (*Funcs));

    Funcs->Arr = (SFunc*) calloc (Capacity, sizeof (*Funcs->Arr));

    return Funcs;
}

void destruct_funcs_array (SFuncs* Funcs)
{
    for (int counter = 0; counter < Funcs->number; ++counter)
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

    Node->data.var = (CharT*) calloc (1, sizeof (*Node->data.var));

    wcscpy (Node->data.var, Token->data.var);

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

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

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

    if (TKN_OP_AND_IS__ TFinish)
    {
        Tokens->number++;

        return get_Statement (FUNC_ARGUMENTS);
    }

    if (TKN_OP_AND_IS__ TCloseBracket)
    {
        Tokens->number++;

        return NULL;
    }

    SNode* Node = construct_op_node (T_Statement);

    Node->left  = get_Function (FUNC_ARGUMENTS);

    Node->right = get_Statement (FUNC_ARGUMENTS);

    return Node;
}

SNode* get_Function (FUNC_HEAD_ARGUMENTS)
{
    SHOUT; //TODO if not do if/else //if not do while //if not do call// if not do return// if not vars//think about


    if ((TKN_OP_AND_IS__ TVariable) && (Tokens->TokenArr[Tokens->number + 1].category == COperation && Tokens->TokenArr[Tokens->number + 1].type == TOpenRoundBracket))
    {
        SNode* Node = construct_op_node (T_Function);

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

    Tokens->number++;

    Node->left  = Left_son;

    Node->right = get_Parameters (FUNC_ARGUMENTS);

    return Node;
}

SNode* get_Type (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    MTokAss ((TKN_OP_AND_IS__ TstdType) || (TKN_OP_AND_IS__ TnoType));

    SNode* Node = construct_op_node (TKN.type);

    Tokens->number++;

    return Node;
}

SNode* get_Parameters (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    // MTokAss (TKN_OP_AND_IS__ TOpenRoundBracket);
    // Tokens->number++;
    CHECK_SYNTAX (TOpenBracket);

    if (TKN_OP_AND_IS__ TCloseRoundBracket)
    {
        return NULL;
    }

    while (true)
    {
        SNode* Node = construct_op_node (T_Param);

        Node->left  = get_Param (FUNC_ARGUMENTS);

        if (TKN_OP_AND_IS__ TCloseRoundBracket)
        {
            Tokens->number++;

            return Node;
        }

        CHECK_SYNTAX (TComma);

        Node->right = get_Param
    }

    return Node;
}

SNode* get_Param (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    SNode* Node = construct_op_node (T_Param);

    Node->left = get_Type (FUNC_ARGUMENTS);

    Node->right = get_Variable (FUNC_ARGUMENTS);
}

SNode* get_IfElse (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    if (TKN_OP_AND_IS__ TIf)
    {
        SNode* Node = construct_op_node (TKN.type);
        Tokens->number++;

        MTokAss (TKN_OP_AND_IS__ TOpenRoundBracket);
        Tokens->number++;

        Node->left  = get_Expression (FUNC_ARGUMENTS);

        MTokAss (TKN_OP_AND_IS__ TOpenRoundBracket);
        Tokens->number++;

        Node->right = get_Statement (FUNC_ARGUMENTS);

        return Node;
    }
}

SNode* get_While (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

}

SNode* get_Announce (FUNC_HEAD_ARGUMENTS)
{
    if (TKN_OP_AND_IS__ TAnnounce)
    {
        SNode* Node = construct_op_node (TAnnounce);

        Node->left  = get_Variable (FUNC_ARGUMENTS);

        Node->right = get_Expression (FUNC_ARGUMENTS);

        for (int counter = 0; counter < Vars->number; counter++)
        {
            if (VAR.name != NULL)
            {
                VAR.name = Node->left->data.var;

                VAR.value = Node->right->data.val;
            }
        }

        return Node;
    }

    return NULL;
}

SNode* get_Variable (FUNC_HEAD_ARGUMENTS)
{
    SNode* Node = construct_var_node (&TKN);

    return Node;
}

SNode* get_Expression (FUNC_HEAD_ARGUMENTS)
{
    SNode* Node = construct_val_node (get_AddSub (FUNC_ARGUMENTS));

    // MTokAss (TKN_OP_AND_IS__ TFinish);
    // Tokens->number++;

    return Node;
}

ValT get_AddSub (FUNC_HEAD_ARGUMENTS)
{
    double Value = get_MulDiv (FUNC_ARGUMENTS);

    while ((TKN_OP_AND_IS__ TAdd)|| (TKN_OP_AND_IS__ TSub))
    {
        ETokenType CurrentType = TKN.type;
        Tokens->number++;

        double SecondValue = get_MulDiv (FUNC_ARGUMENTS);

        if (CurrentType == TAdd)
        {
            Value = Value + SecondValue;
        }
        else
        {
            Value = Value - SecondValue;
        }
    }

    return Value;
}

ValT get_MulDiv (FUNC_HEAD_ARGUMENTS)
{
    double Value = get_Pow (FUNC_ARGUMENTS);

    while ((TKN_OP_AND_IS__ TMul)|| (TKN_OP_AND_IS__ TDiv))
    {
        ETokenType CurrentType = TKN.type;
        Tokens->number++;

        double SecondValue = get_Pow (FUNC_ARGUMENTS);

        if (CurrentType == TMul)
        {
            Value = Value * SecondValue;
        }
        else
        {
            Value = Value / SecondValue;
        }
    }

    return Value;
}

ValT get_Pow (FUNC_HEAD_ARGUMENTS)
{
    double Value = get_Bracket (FUNC_ARGUMENTS);

    while (TKN_OP_AND_IS__ TPow)
    {
        Tokens->number++;

        double SecondValue = get_Bracket (FUNC_ARGUMENTS);

        Value = pow (Value, SecondValue);
    }

    return Value;
}

ValT get_Bracket (FUNC_HEAD_ARGUMENTS)
{
    double Value = 0;

    if (TKN_OP_AND_IS__ TOpenRoundBracket)
    {
        Tokens->number++;

        Value = get_AddSub (FUNC_ARGUMENTS);

        MTokAss (TKN_OP_AND_IS__ TCloseRoundBracket);

        Tokens->number++;

        return Value;
    }

    if (TKN.category == CLine)
    {
        if (TKN.type == TVariable)
        {
            for (int counter = 0; counter < Vars->number; counter++)
            {
                if (VAR.name == TKN.data.var)
                {
                    Tokens->number++;

                    return VAR.value;
                }
            }
        } //TODO if func
    }

    if (TKN.category == CValue && TKN.type == TValue)
    {
        Value = TKN.data.val;
        Tokens->number++;

        return Value;
    }

    wprintf (L"ERROR!\n");
    MTokAss (0);

    return 0;
}

//=============================================================================================================================================================================

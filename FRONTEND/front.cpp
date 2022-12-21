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


#define SEP_SYMBOLS "\n;, "
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
//seekers//
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

    swscanf (&(Source->Code[Source->ip]), L"%ml[^" SEP_SYMBOLS "]", &Lexem);

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

#define DEF_LEX(d_type, d_condition, d_tokenize,...) \
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

    if (((TKN_OP_AND_IS__ TnoType) || (TKN_OP_AND_IS__ TstdType)) && (Tokens->TokenArr[Tokens->number + 1].category == CLine) && (Tokens->TokenArr[Tokens->number + 2].type == TOpenRoundBracket))
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
    CHECK_SYNTAX (TOpenRoundBracket);

    if (TKN_OP_AND_IS__ TCloseRoundBracket)
    {
        return NULL;
    }

    SNode* Node = get_Param (FUNC_ARGUMENTS);

    CHECK_SYNTAX (TCloseRoundBracket);

    return Node;
//     SNode* Node = construct_op_node (T_Param);
//
//     Node->left  = get_Param (FUNC_ARGUMENTS);
//
//     if (TKN_OP_AND_IS__ TCloseRoundBracket)
//     {
//         Tokens->number++;
//
//         return Node;
//     }
//
//     CHECK_SYNTAX (TComma);
//
//     Node->right = get_Param
//
//     return Node;
}

SNode* get_Param (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    SNode* Node = construct_op_node (T_Param);

    Node->left = get_Announce (FUNC_ARGUMENTS);

    if (TKN_OP_AND_IS__ TComma)
    {
        Tokens->number++;

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
        Tokens->number++;

        MTokAss (TKN_OP_AND_IS__ TOpenRoundBracket);
        Tokens->number++;

        Node->left  = get_Expression (FUNC_ARGUMENTS);

        MTokAss (TKN_OP_AND_IS__ TOpenRoundBracket);
        Tokens->number++;

        Node->right = get_Statement (FUNC_ARGUMENTS);

        return Node;
    }

    return get_While (FUNC_ARGUMENTS);
}

SNode* get_While (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    Tokens->number++;
    if (Tokens->number = Tokens->size + 1)
    {
        return NULL;
    }

    //exit (0);

    return NULL;
}

SNode* get_Announce (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    if (TKN_OP_AND_IS__ TstdType)
    {
        Tokens->number++;

        SNode* Node = construct_op_node (T_Announce);

        Node->left  = get_Variable (FUNC_ARGUMENTS);

        if (TKN_OP_AND_IS__ TAssign)
        {
            Tokens->number++;

            Node->right = get_Expression (FUNC_ARGUMENTS);
        }

        for (int counter = 0; counter < Vars->number; counter++)
        {
            if (VAR.name != NULL)
            {
                VAR.name = Node->left->data.var;

                VAR.value = (Node->right == NULL) ? 38 : Node->right->data.val;
            }
        }

        return Node;
    }

    return NULL;
}

SNode* get_Variable (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    SNode* Node = construct_var_node (&TKN);
    Tokens->number++;

    return Node;
}

SNode* get_Expression (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

    SNode* Node = construct_val_node (get_AddSub (FUNC_ARGUMENTS));

    // MTokAss (TKN_OP_AND_IS__ TFinish);
    // Tokens->number++;

    return Node;
}

ValT get_AddSub (FUNC_HEAD_ARGUMENTS)
{
    SHOUT;

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
    SHOUT;

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
    SHOUT;

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

    if (Node->parent != NULL)
    {
        //wprintf (L"!%d!\n", Node->branch);
        if ((Node->parent != NULL) && (Node->parent->left == Node))
        {
            fprintf (File,  "\n                    node_%p -> node_%p;",
                        Node->parent, Node);
        }
        else if ((Node->parent != NULL) && (Node->parent->right == Node))
        {
            fprintf (File,  "\n                    node_%p -> node_%p;",
                        Node->parent, Node);
        }
    }

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
                #define DEF_LEX(d_type, d_condition, d_tokenize, d_print) \
                case d_type: \
                    fprintf (File, "<td colspan=\"2\" bgcolor = \"" GV_OP_COLOUR "\">\n"  " %s", d_print); \
                    break;

                    #include "Lexer.h"

                #undef DEF_LEX

                default:
                MCA (0, (void) 0);
            }
            break;



        case CLine:
            fprintf (File, "<td colspan=\"2\" bgcolor = \"" GV_VAR_COLOUR "\">\n" VAR_SPEC, Node->data.var);
            break;

        case CValue:
            fprintf (File, "<td colspan=\"2\" bgcolor = \"" GV_VAL_COLOUR "\">\n" VAL_SPEC, Node->data.val);
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

    sprintf (Command, "xdg-open %s.png", FileName);

    //system ("xdg-open 1.png");
    system (Command);

    free (Command);

    return;
}

//===================================================================================================================================================================

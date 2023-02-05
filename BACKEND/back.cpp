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

#include "back.h"
#include "front.h"


#include "asm.h"
#include "proc.h"


//=============================================================================================================================================================================

#define MAIN_WORD L"main"

#define MAIN_JUMP "main"

#define SHIFT_REG   "rax"
#define COUNT_REG   "rbx"
#define TOP_REG     "rcx"
#define FUNC_REG    "rdx"

#define LABEL "lbl_"

#define JUNK -1

#define TRUE   1
#define FALSE  0


#define PUTLN(d_command) writeln_command ( d_command , Back->file)
#define PUT(d_command)   write_command   ( d_command , Back->file)

#define CLEAN_TABLE if (Back->table_cond != none) delete_var_table (Back); Back->table_cond = exist;

#define SEP_LINE ";----------------------------------------------------------------------------------------------------------------------------------------------------------------------------"

//=============================================================================================================================================================================

void my_b_main (int argc, char** argv)
{
    setlocale(LC_CTYPE, "");


    SNode* Root = read_tree ("FILES/ParsedSrc.tr");

    if (Root != NULL)
    {
        FILE* ExFile = fopen ("EXAMPLES_ASM/code.asm", "w");

        make_gv_tree (Root, "BACKEND/GRAPH_VIZ/GraphViz_treeDump");

        make_asm_file (Root, ExFile);

        delete_tree (&Root);

        execute_asm_file (ExFile);

    }

    return;
}

//=============================================================================================================================================================================
//Readers
//=============================================================================================================================================================================

SNode* read_tree (const char* FileName)
{
    FILE* InputFile = fopen (FileName, "r");
    MLA (InputFile != NULL);

    SSrc Tree = {};

    fscanf (InputFile, "%ml[^" FORBIDDEN_TERMINATING_SYMBOL "]", &(Tree.Arr));
    fclose (InputFile);

    MLA (Tree.Arr != NULL);

    Tree.ip = 0;
    Tree.size = wcslen (Tree.Arr);

    //wprintf (L"%ls\n", Tree.Arr);

    SNode* Root = read_node (&Tree);

    free (Tree.Arr);

    return Root;
}

SNode* read_node (SSrc* Tree)
{
    SNode* Node = (SNode*) calloc (1, sizeof (SNode));

    CharT* CategoryLine = NULL;
    seek (Tree);
    swscanf (&(Tree->Arr[Tree->ip]), L"%mls", &CategoryLine);

    if (wcscmp (CategoryLine, L"COperation") == 0)
    {
        Node->category = COperation;
        seek_out (Tree);
    }
    else if (wcscmp (CategoryLine, L"CValue") == 0)
    {
        Node->category = CValue;
        seek_out (Tree);
    }
    else if (wcscmp (CategoryLine, L"CLine") == 0)
    {
        Node->category = CLine;
        seek_out (Tree);
    }
    else
    {
        //wprintf (L"%ls\n", CategoryLine);
        free (CategoryLine);
        free (Node);

        return NULL;
    }

    //wprintf (L"++%ls++\n", CategoryLine);
    free (CategoryLine);

    CharT* Line = NULL;

    switch (Node->category)
    {
        case COperation:
            swscanf (&(Tree->Arr[Tree->ip]), L"%ml[^\n]", &Line);
            MLA (Line != NULL);
            //wprintf (L"--%ls--\n", Line);

            #define DEF_OP(d_type, d_condition, d_tokenize, d_print, ...) \
            else if (wcscmp (Line, d_print) == 0) \
            { \
                Node->type = d_type; \
            }

            if (0) {}

            #include "Operations.h"

            else
            {
                wprintf (L"ERROR!==%ls== in ParcedFile\n", Line);
                MLA (0);
            }

            #undef DEF_OP

            Tree->ip += wcslen (Line);

            break;

        case CValue:
            swscanf (&(Tree->Arr[Tree->ip]), L"%mls", &Line);
            MLA (Line != NULL);

            Tree->ip += wcslen (Line);

            swscanf (&(Tree->Arr[Tree->ip]), L"%lg", &(Node->data.val));

            Tree->ip++;
            seek_out (Tree);

            Node->type = TValue;

            break;

        case CLine:
            swscanf (&(Tree->Arr[Tree->ip]), L"%mls", &Line);
            MLA (Line != NULL);

            Tree->ip += wcslen (Line) + 1;

            free (Line);

            swscanf (&(Tree->Arr[Tree->ip]), L"%ml[^\n]", &Line);

            Node->data.var = wcsdup (Line);

            Tree->ip += wcslen (Node->data.var) + 1;

            Node->type = TVariable;

            break;

        default:
            wprintf (L"DEFAULT ERROR!\n");
            MLA (0);
    }

    free (Line);

    // swscanf (&(Tree->Arr[Tree->ip]), L"%mls", &Line);
    // wprintf (L"==%ls==\n", Line);

    seek (Tree);
    seek_out (Tree);

    // swscanf (&(Tree->Arr[Tree->ip]), L"%mls", &Line);
    // wprintf (L"==%ls==\n", Line);

    //wprintf (L"L\n");
    Node->left = read_node (Tree);

    seek (Tree);
    seek_out (Tree);

    seek (Tree);
    seek_out (Tree);

    //wprintf (L"R\n");
    Node->right = read_node (Tree);

    seek (Tree);
    seek_out (Tree);

    return Node;
}

//=============================================================================================================================================================================
//Seekers//
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
        if (((Source->Arr[Source->ip] == L'\n')) || ((Source->Arr[Source->ip] == L' ')))
        {
            Source->ip++;
            return;
        }
    }

    return;
}

//=============================================================================================================================================================================
//Tree//
//=============================================================================================================================================================================

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

//=============================================================================================================================================================================
//Make ASM//
//=============================================================================================================================================================================

void make_asm_file (SNode* Root, FILE* File)
{
    MLA (File != NULL);

    SBack* Back = (SBack*) calloc (1, sizeof (SBack));

    Back->Funcs         = (SBackFuncTable*)     calloc (1, sizeof (SBackFuncTable));
    Back->Funcs->Table  = (SBackFunc*)          calloc (MAX_FUNCS_ARRAY, sizeof (SBackFunc));
    Back->Funcs->top_index = 0;

    Back->VarStack      = (SStack<SVarTable*>*) calloc (1, sizeof (SStack<SVarTable*>));

    Back->file = File;

    Back->table_cond = none;

    stack_constructor (Back->VarStack, 4);

    //generate_user_functions (Root, Back);

    generate_code (Root, Back);

    // generate_main (Root, Back);

    stack_destructor (Back->VarStack);

    fclose (Back->file);

    // for (int i = 0; i < Back->Funcs->top_index; i++)
    // {
    //     printf ("'%ls'[%d]\n", Back->Funcs->Table[i]);
    // }

    free (Back->Funcs->Table);
    free (Back->Funcs);
    free (Back->VarStack);
    free (Back);

    return;
}

void generate_code (SNode* Root, SBack* Back)
{
    fprintf (Back->file, SEP_LINE "\n\n");

    PUT (push);
    fprintf (Back->file, " 0\n"); //TODO ask Danya maybe %d 0
    PUT (pop);
    fprintf (Back->file, " " SHIFT_REG "\n\n");


    PUT (push);
    fprintf (Back->file, " 0\n");
    PUT (pop);
    fprintf (Back->file, " " TOP_REG "\n\n");

    PUT (jump);
    fprintf (Back->file, " " LABEL MAIN_JUMP "\n\n");

    fprintf (Back->file, SEP_LINE "\n\n");

    generate_statement (Root, Back);

//     SNode* Main = find_main (Root);
//     MLA (Main != NULL);
//
//     Back->func_cond = main_f;
//
//     generate_statement (Main, Back);

//     generate_op_node (Main->left, Back);
//
//     SNode* CurStatement = Main->right;
//
//     while (CurStatement != NULL)
//     {
//         generate_op_node (CurStatement->left, Back);
//
//         CurStatement = CurStatement->right;
//     }

    //delete_var_table (Back);

    return;
}

void generate_main (SNode* Root, SBack* Back)
{
    SNode* Main = find_main (Root);
    MLA (Main != NULL);

    Back->func_cond = main_f;

    generate_statement (Main, Back);

//     generate_op_node (Main->left, Back);
//
//     SNode* CurStatement = Main->right;
//
//     while (CurStatement != NULL)
//     {
//         generate_op_node (CurStatement->left, Back);
//
//         CurStatement = CurStatement->right;
//     }

    delete_var_table (Back);

    return;
}

void generate_statement (BACK_FUNC_HEAD_PARAMETERS)
{
    if (CurNode->left != NULL)
    {
        generate_op_node (BACK_LEFT_SON_FUNC_PARAMETERS);
    }

    if (CurNode->right != NULL)
    {
        generate_statement (BACK_RIGHT_SON_FUNC_PARAMETERS);
    }

    return;
}

void generate_function (BACK_FUNC_HEAD_PARAMETERS)
{
    if (wcscmp (CurNode->left->data.var, MAIN_WORD) == 0)
    {
        Back->func_cond = main_f;
    }
    else
    {
        Back->func_cond = any_f;
    }

    fprintf (Back->file, LABEL "%ls:\n", CurNode->left->data.var);

    Back->Funcs->Table[Back->Funcs->top_index].Name = CurNode->left->data.var;

    Back->table_cond = none;
    create_param_var_table (CurNode->left->right, Back);

    pop_parameters (Back);

    Back->Funcs->top_index++;


    generate_statement (BACK_RIGHT_SON_FUNC_PARAMETERS);
    CLEAN_TABLE;

    fprintf (Back->file, "\n\n" SEP_LINE "\n\n\n");

    return;
}

void generate_node (BACK_FUNC_HEAD_PARAMETERS)
{
    if (CurNode->category == COperation)
    {
        generate_op_node (BACK_FUNC_PARAMETERS);
    }

    if (CurNode->category == CValue)
    {
        write_command (push, Back->file);

        fprintf (Back->file, " %lg\n", CurNode->data.val);
    }

    if (CurNode->category == CLine && CurNode->type == TVariable)
    {
        generate_push_var (BACK_FUNC_PARAMETERS);
//         write_command (push, Back->file);
//
//         int Index = find_var (BACK_FUNC_PARAMETERS);
//
//         fprintf (Back->file, " [%d]\n", Index);
    }

    return;
}

void generate_op_node (BACK_FUNC_HEAD_PARAMETERS)
{
    switch (CurNode->type)
    {
        #define DEF_OP(d_type, d_condition, d_tokenize, d_print, d_switch) \
        case d_type: \
            d_switch; \
            break;

        #include "Operations.h"

        #undef DEF_OP
    }

    return;
}

void generate_announce (BACK_FUNC_HEAD_PARAMETERS)
{
    generate_expression (BACK_RIGHT_SON_FUNC_PARAMETERS);

    add_to_var_table (BACK_LEFT_SON_FUNC_PARAMETERS);

    generate_pop_var (BACK_LEFT_SON_FUNC_PARAMETERS);

    incr_top_reg (Back);
//     write_command (pop, Back->file);
//
//     fprintf (Back->file, " [%d]\n\n", Back->RAM_top_index - 1);

    return;
}

void generate_equation (BACK_FUNC_HEAD_PARAMETERS)
{
    generate_expression (BACK_RIGHT_SON_FUNC_PARAMETERS);

    generate_pop_var (BACK_LEFT_SON_FUNC_PARAMETERS);

    return;
}

void generate_input (BACK_FUNC_HEAD_PARAMETERS)
{
    SNode* Node = CurNode->left;

    do
    {
        writeln_command (inp, Back->file);

        generate_pop_var (Node->left, Back);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void generate_output (BACK_FUNC_HEAD_PARAMETERS)
{
    SNode* Node = CurNode->left;

    do
    {
        generate_push_var (Node->left, Back);

        writeln_command (out, Back->file);

        Node = Node->right;
    } while (Node != NULL);

    return;
}

void generate_if (BACK_FUNC_HEAD_PARAMETERS)
{
    generate_postorder (BACK_LEFT_SON_FUNC_PARAMETERS);

    PUT (push);
    fprintf (Back->file, " %d\n", FALSE);

    PUT (je);
    fprintf (Back->file, " " LABEL "%d\n", Back->label_cnt);
    int Label_1 = Back->label_cnt;
    Back->label_cnt++;

    CurNode = CurNode->right;

    Back->table_cond = none;
    generate_statement (BACK_LEFT_SON_FUNC_PARAMETERS);
    CLEAN_TABLE;

    PUT (jump);
    fprintf (Back->file, " " LABEL "%d\n", Back->label_cnt);
    int Label_2 = Back->label_cnt;
    Back->label_cnt++;

    fprintf (Back->file,  LABEL "%d:\n", Label_1);

    CurNode = CurNode->right;

    if (CurNode != NULL)
    {
        if (CurNode->category == COperation && CurNode->type == TIf)
        {
            generate_if (BACK_FUNC_PARAMETERS);
        }
        else
        {
            Back->table_cond = none;
            generate_statement (BACK_FUNC_PARAMETERS);
            CLEAN_TABLE;
        }
    }
    fprintf (Back->file,  LABEL "%d:\n", Label_2);


    return;
}

void generate_while (BACK_FUNC_HEAD_PARAMETERS)
{
    int Label_1 = Back->label_cnt;
    fprintf (Back->file,  LABEL "%d:\n", Label_1);
    Back->label_cnt++;

    generate_postorder (BACK_LEFT_SON_FUNC_PARAMETERS);

    PUT (push);
    fprintf (Back->file, " %d\n", FALSE);

    PUT (je);
    int Label_2 = Back->label_cnt;
    fprintf (Back->file, " " LABEL "%d\n", Label_2);
    Back->label_cnt++;

    Back->table_cond = none;
    generate_statement (BACK_RIGHT_SON_FUNC_PARAMETERS);
    delete_var_table (Back);

    PUT (jump);
    fprintf (Back->file, " " LABEL "%d\n", Label_1);

    fprintf (Back->file,  LABEL "%d:\n", Label_2);

    return;
}

void generate_call (BACK_FUNC_HEAD_PARAMETERS)
{
    fprintf (Back->file, ";++" "\n\n");

    PUT (push);
    fprintf (Back->file, " " TOP_REG "\n");

    push_parameters (BACK_RIGHT_SON_FUNC_PARAMETERS);

    PUT (push);
    fprintf (Back->file, " " TOP_REG "\n");

    PUT (pop);
    fprintf (Back->file, " " SHIFT_REG "\n\n");

    PUT (call);
    fprintf (Back->file, " " LABEL "%ls\n", CurNode->left->data.var);

    PUT (pop);
    fprintf (Back->file, " " TOP_REG "\n");

    fprintf (Back->file, ";++" "\n\n");

    PUT (push);
    fprintf (Back->file, " " FUNC_REG "\n");

    return;
}

void generate_return (BACK_FUNC_HEAD_PARAMETERS)
{
    if (Back->func_cond == main_f)
    {
        writeln_command (hlt, Back->file);
    }
    else
    {
        generate_expression (BACK_LEFT_SON_FUNC_PARAMETERS);

        PUT (pop);
        fprintf (Back->file, " " FUNC_REG "\n");

        PUTLN (ret);
    }

    return;
}

void generate_expression (BACK_FUNC_HEAD_PARAMETERS)
{
    generate_postorder (BACK_FUNC_PARAMETERS);

    return;
}

void generate_postorder (BACK_FUNC_HEAD_PARAMETERS)
{
    if (!(CurNode->category == COperation && CurNode->type == T_Call) && CurNode->left != NULL)
    {
        generate_postorder (BACK_LEFT_SON_FUNC_PARAMETERS);
    }

    if (!(CurNode->category == COperation && CurNode->type == T_Call) && CurNode->right != NULL)
    {
        generate_postorder (BACK_RIGHT_SON_FUNC_PARAMETERS);
    }

    generate_node (BACK_FUNC_PARAMETERS);

    return;
}

void generate_pop_var (BACK_FUNC_HEAD_PARAMETERS)
{
    int Index = find_var (BACK_FUNC_PARAMETERS);

    PUT (push);
    fprintf (Back->file, " %d\n", Index);

    PUT (push);
    fprintf (Back->file, " " SHIFT_REG "\n");

    PUTLN (add);

    PUT (pop);
    fprintf (Back->file, " " COUNT_REG "\n");


    PUT (pop);
    fprintf (Back->file, " [" COUNT_REG "]\n\n");

    return;
}

void generate_push_var (BACK_FUNC_HEAD_PARAMETERS)
{
    int Index = find_var (BACK_FUNC_PARAMETERS);

    PUT (push);
    fprintf (Back->file, " %d\n", Index);

    PUT (push);
    fprintf (Back->file, " " SHIFT_REG "\n");

    PUTLN (add);

    PUT (pop);
    fprintf (Back->file, " " COUNT_REG "\n");

    PUT (push);
    fprintf (Back->file, " [" COUNT_REG "]\n\n");

    return;
}

//=============================================================================================================================================================================

void push_parameters (BACK_FUNC_HEAD_PARAMETERS)
{
    if (CurNode->right != NULL)
    {
        push_parameters (BACK_RIGHT_SON_FUNC_PARAMETERS);
    }

    if (CurNode != NULL)
    {
        generate_expression (BACK_LEFT_SON_FUNC_PARAMETERS);
    }

    // PUT (push);
    // fprintf (Back->file, " " TOP_REG "\n");

    return;
}

void pop_parameters (SBack* Back)
{
    PUT (push);
    fprintf (Back->file, " " SHIFT_REG "\n");

    // PUTLN (m_dup);
    // PUTLN (out);

    PUT (pop);
    fprintf (Back->file, " " COUNT_REG "\n\n");

    for (int i = 0; i < Back->Funcs->Table[Back->Funcs->top_index].parameters ; i++)
    {
        PUT (push);
        fprintf (Back->file, " " COUNT_REG "\n");

        PUT (push);
        fprintf (Back->file, " 1\n");

        PUTLN (add);

        PUT (pop);
        fprintf (Back->file, " " COUNT_REG "\n");

        PUT (pop);
        fprintf (Back->file, " [" COUNT_REG "]\n\n");
    }

//     PUT (push);
//     fprintf (Back->file, " " COUNT_REG "\n");
//
//     PUT (pop);
//     fprintf (Back->file, " " SHIFT_REG "\n");

    return;
}

void incr_top_reg (SBack* Back)
{
    PUT (push);
    fprintf (Back->file, " " TOP_REG "\n");

    PUT (push);
    fprintf (Back->file, " 1\n");

    PUTLN (add);

    PUT (pop);
    fprintf (Back->file, " " TOP_REG "\n\n");

    return;
}

void standard_if_jump (SBack* Back)
{
    fprintf (Back->file, " " LABEL "%d\n", Back->label_cnt);
    Back->label_cnt++;

    PUT (push);
    fprintf (Back->file, " %d\n", FALSE);

    PUT (jump);
    fprintf (Back->file, " " LABEL "%d\n", Back->label_cnt);

    fprintf (Back->file, LABEL "%d:\n", Back->label_cnt - 1);

    PUT (push);
    fprintf (Back->file, " %d\n", TRUE);

    fprintf (Back->file, LABEL "%d:\n", Back->label_cnt);

    Back->label_cnt++;

    return;
}

SNode* find_main (SNode* Node)
{
    if (NODE_IS_OP_AND__ T_Function)
    {
        if (Node->left != NULL &&
        Node->left->category == CLine && (wcscmp (MAIN_WORD, Node->left->data.var) == 0))
        {
            return Node->right->right;
        }
    }

    SNode* Main = NULL;

    if (Node->left != NULL)
    {
        Main = find_main (Node->left);
    }

    if ((Node->right != NULL) && (Main == NULL))
    {
        Main = find_main (Node->right);
    }

    return Main;
}

void write_command (ECommandNums eCommand, FILE* File)
{
    #define DEF_CMD(def_line, def_enum, ...) \
    else if (eCommand == def_enum) \
    { \
        fprintf (File, TAB  def_line); \
        return; \
    }

    if (0) {}

    #include "commands.h"

    else
    {
        MLA (0);
    }

    #undef DEF_CMD
}

void writeln_command (ECommandNums eCommand, FILE* File)
{
    //printf ("%d!\n", eCommand);

    write_command (eCommand, File);

    fprintf (File, "\n");

    return;
}

//=============================================================================================================================================================================

void add_to_var_table (BACK_FUNC_HEAD_PARAMETERS)
{
    if (Back->table_cond == none)
    {
        create_new_var_table (Back);
    }

    MLA (CurNode->category == CLine && CurNode->type == TVariable);

    SVarTable* Table = NULL;
    peek_from_stack (Back->VarStack, &Table);

    Table->Arr[Table->size].name  = CurNode->data.var; //copy address from node
    Table->Arr[Table->size].index = Back->RAM_top_index;

    Back->RAM_top_index++;
    Table->size++;

    return;
}

void create_new_var_table (SBack* Back)
{
    SVarTable* NewTable = (SVarTable*)  calloc (1,                  sizeof (SVarTable));
    NewTable->Arr       = (SVarAccord*) calloc (VAR_TABLE_CAPACITY, sizeof (SVarAccord));
    NewTable->size = 0;

    push_in_stack (Back->VarStack, NewTable);

    Back->table_cond = exist;

    return;
}

void create_param_var_table (BACK_FUNC_HEAD_PARAMETERS)
{
    Back->RAM_top_index = 1;

    do
    {
        if (CurNode->left != NULL)
        {
            add_to_var_table (CurNode->left->left, Back);
            Back->Funcs->Table[Back->Funcs->top_index].parameters++;
        }

        CurNode = CurNode->right;
    } while (CurNode != NULL);

    return;
}

void delete_var_table (SBack* Back)
{
    SVarTable* Table = NULL;

    pop_from_stack (Back->VarStack, &Table);

    free (Table->Arr);

    free (Table);

    return;
}

int find_var (BACK_FUNC_HEAD_PARAMETERS)
{
    int RetIndex = JUNK;

    MLA (CurNode->category == CLine && CurNode->type == TVariable);

    SVarTable* Table = NULL;

    int depth = 1;
    do
    {
        Table = Back->VarStack->data[Back->VarStack->size - depth];
        //printf ("=%p=%d/%d=\n", Table, depth, Back->VarStack->size);
        depth++;
    } while ((find_in_table (CurNode->data.var, Table, &RetIndex) == false) && (depth <= Back->VarStack->size));

    if (RetIndex < 0)
    {
        printf ("==ERROR==\n""No '%ls' found!\n", CurNode->data.var);
        MLA (0);
    }

    return RetIndex;
}

bool find_in_table (CharT* varName, SVarTable* Table, int* RetIndex)
{
    MLA (Table != NULL);

    for (int counter = 0; counter < Table->size; ++counter)
    {
        if (wcscmp (varName, Table->Arr[counter].name) == 0)
        {
            *RetIndex = Table->Arr[counter].index;

            return true;
        }
    }

    return false;
}

//=============================================================================================================================================================================

void execute_asm_file (FILE* ExFile)
{
    system ("BACKEND/BUILD/asm");

    system ("BACKEND/BUILD/proc");

    return;
}

//=============================================================================================================================================================================

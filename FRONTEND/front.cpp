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

const int StdErr = 1;
const int NoErr  = 0;


//=============================================================================================================================================================================

void _main (void)
{
    setlocale(LC_CTYPE, "");

    FILE* SourceText = fopen ("EXAMPLES/test.ger", "r");
    MLA (SourceText != NULL);

    SSrc Source = *(construct_source (&Source, SourceText));

    fclose (SourceText);

    SToken* TokenArr = lex_src (&Source);

    if (Source.Code != NULL)
    {
        free (Source.Code);
    }

    if (TokenArr != NULL)
    {
        free (TokenArr);
    }

    return;
}

//=============================================================================================================================================================================
//construct//
//=============================================================================================================================================================================

SSrc* construct_source (SSrc* Source, FILE* SourceText)
{
    //Source->Code = (CharT*) calloc (MAX_MEMORY, sizeof (*(Source->Code)));
    //Source->size = MAX_MEMORY;

    fwscanf (SourceText, L"%ml[^~]", &Source->Code); // TODO Ask Danya how to remove this shit!!
    wprintf (L"#%ls#", Source->Code);

    Source->ip   = 0;
    Source->size = wcslen (Source->Code);

    //fread (Source->Code, MAX_MEMORY, sizeof (CharT), SourceText);
    //fwscanf (SourceText, L"%m[^.]", &(Source->Code));
    //wprintf (L"\n'%ls'\n", Source->Code);

    return Source;
}

//=============================================================================================================================================================================
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

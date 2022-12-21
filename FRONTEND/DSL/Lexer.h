#define ERR "YOU ARE NOT SUPPOSED TO SEE THAT!"

DEF_LEX (TValue,
(*Lexem >= L'0' && *Lexem <= L'9'),
{
    double Value = parse_double (Lexem);

    TKN.category = CValue;
    TKN.data.val = Value;
},
ERR)

DEF_LEX (TAssign,
(*Lexem == L'='),
{
    TKN_IS_OP;
},
"=")

DEF_LEX (TFinish,
(*Lexem == L';'),
{
    TKN_IS_OP;
},
"FINISH")

DEF_LEX (TAdd,
(*Lexem == L'+'),
{
    TKN_IS_OP;
},
"ADD")

DEF_LEX (TSub,
(*Lexem == L'-'),
{
    TKN_IS_OP;
},
"SUB")

DEF_LEX (TMul,
(*Lexem == L'*'),
{
    TKN_IS_OP;
},
"MUL")

DEF_LEX (TDiv,
(*Lexem == L'/'),
{
    TKN_IS_OP;
},
"DIV")

DEF_LEX (TPow,
(*Lexem == L'/'),
{
    TKN_IS_OP;
},
"POW")

DEF_LEX (TOpenRoundBracket,
(*Lexem == L'('),
{
    TKN_IS_OP;
},
ERR)

DEF_LEX (TCloseRoundBracket,
(*Lexem == L')'),
{
    TKN_IS_OP;
},
ERR)

DEF_LEX (TOpenBracket,
(*Lexem == L'{'),
{
    TKN_IS_OP;
},
ERR)

DEF_LEX (TCloseBracket,
(*Lexem == L'}'),
{
    TKN_IS_OP;
},
ERR)

DEF_LEX (TComma,
(*Lexem == L','),
{
    TKN_IS_OP;
},
ERR)

DEF_LEX (TIf,
(wcscmp (Lexem, L"if") == 0),
{
    TKN_IS_OP;
},
"IF")

DEF_LEX (TElse,
(wcscmp (Lexem, L"else") == 0),
{
    TKN_IS_OP;
},
"ELSE")


DEF_LEX (TstdType,
(wcscmp (Lexem, L"var") == 0),
{
    TKN_IS_OP;
},
"stdType")

DEF_LEX (TnoType,
(wcscmp (Lexem, L"void") == 0),
{
    TKN_IS_OP;
},
"noType")

DEF_LEX (T_Statement,
(0),
{
    TKN_IS_OP;
},
"STTMNT")

DEF_LEX (T_Function,
(0),
{
    TKN_IS_OP;
},
"FUNC")

DEF_LEX (T_Parameters,
(0),
{
    TKN_IS_OP;
},
ERR)

DEF_LEX (T_Param,
(0),
{
    TKN_IS_OP;
},
"PARAM")

DEF_LEX (T_Type,
(0),
{
    TKN_IS_OP;
},
ERR)

DEF_LEX (T_Announce,
(0),
{
    TKN_IS_OP;
},
ERR)

DEF_LEX (T_Expression,
(0),
{
    TKN_IS_OP;
},
ERR)

DEF_LEX (TComment,
(*Lexem == L'#'),
{
    Tokens->number--;
},
ERR)

DEF_LEX (TVariable,
(*Lexem >= L'A' && *Lexem <= L'z'),
{
    TKN.category = CLine;
    TKN.data.var = wcsdup (Lexem);
},
ERR)

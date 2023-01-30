#define ERR "YOU ARE NOT SUPPOSED TO SEE THAT!"

DEF_LEX (TValue,
(*Lexem >= L'0' && *Lexem <= L'9'),
{
    double Value = parse_double (Lexem);

    TKN.category = CValue;
    TKN.data.val = Value;
},
"TValue")

DEF_LEX (TAssign,
LEXEM_IS (L"="),
{
    TKN_IS_OP;
},
"ASSIGN")

DEF_LEX (TFinish,
LEXEM_IS (L";"),
{
    TKN_IS_OP;
},
"FINISH")

DEF_LEX (TaAdd,
LEXEM_IS (L"+"),
{
    TKN_IS_OP;
},
"ADD")

DEF_LEX (TaSub,
LEXEM_IS (L"-"),
{
    TKN_IS_OP;
},
"SUB")

DEF_LEX (TaMul,
LEXEM_IS (L"*"),
{
    TKN_IS_OP;
},
"MUL")

DEF_LEX (TaDiv,
LEXEM_IS (L"/"),
{
    TKN_IS_OP;
},
"DIV")

DEF_LEX (TaPow,
LEXEM_IS (L"^"),
{
    TKN_IS_OP;
},
"POW")

DEF_LEX (TcLess,
LEXEM_IS (L"<"),
{
    TKN_IS_OP;
},
"LESS")

DEF_LEX (TcMore,
LEXEM_IS (L">"),
{
    TKN_IS_OP;
},
"MORE")

DEF_LEX (TcLessEq,
LEXEM_IS (L"<="),
{
    TKN_IS_OP;
},
"LESS OR EQUAL")

DEF_LEX (TcMoreEq,
LEXEM_IS (L">="),
{
    TKN_IS_OP;
},
"MORE OR EQUAL")

DEF_LEX (TcEqual,
LEXEM_IS (L"=="),
{
    TKN_IS_OP;
},
"EQUAL")

DEF_LEX (TcNotEq,
LEXEM_IS (L"!="),
{
    TKN_IS_OP;
},
"NOT EQUAL")

DEF_LEX (TcNot,
LEXEM_IS (L"!"),
{
    TKN_IS_OP;
},
"NOT")

DEF_LEX (TcAnd,
LEXEM_IS (L"&&"),
{
    TKN_IS_OP;
},
"AND")

DEF_LEX (TcOr,
LEXEM_IS (L"||"),
{
    TKN_IS_OP;
},
"OR")

DEF_LEX (TOpenRoundBracket,
LEXEM_IS (L"("),
{
    TKN_IS_OP;
},
"(")

DEF_LEX (TCloseRoundBracket,
LEXEM_IS (L")"),
{
    TKN_IS_OP;
},
")")

DEF_LEX (TOpenBracket,
LEXEM_IS (L"{"),
{
    TKN_IS_OP;
},
"{" ERR)

DEF_LEX (TCloseBracket,
LEXEM_IS (L"}"),
{
    TKN_IS_OP;
},
"}" ERR)

DEF_LEX (TComma,
LEXEM_IS (L","),
{
    TKN_IS_OP;
},
"," ERR)

DEF_LEX (TIf,
LEXEM_IS (L"if"),
{
    TKN_IS_OP;
},
"IF")

DEF_LEX (TElse,
LEXEM_IS (L"else"),
{
    TKN_IS_OP;
},
"ELSE")

DEF_LEX (TWhile,
LEXEM_IS (L"while"),
{
    TKN_IS_OP;
},
"WHILE")

DEF_LEX (TstdType,
LEXEM_IS (L"var"),
{
    TKN_IS_OP;
},
"stdType")

DEF_LEX (TnoType,
LEXEM_IS (L"void"),
{
    TKN_IS_OP;
},
"noType")

DEF_LEX (TInput,
LEXEM_IS (L"input"),
{
    TKN_IS_OP;
},
"INPUT")

DEF_LEX (TOutput,
LEXEM_IS (L"output"),
{
    TKN_IS_OP;
},
"OUTPUT")

DEF_LEX (TReturn,
LEXEM_IS (L"return"),
{
    TKN_IS_OP;
},
"RETURN")

DEF_LEX (T_Statement,
(0),
{
    TKN_IS_OP;
},
"_STATEMENT")

DEF_LEX (T_Function,
(0),
{
    TKN_IS_OP;
},
"_FUNC")

DEF_LEX (T_Parameters,
(0),
{
    TKN_IS_OP;
},
"_PARAMETERS")

DEF_LEX (T_Param,
(0),
{
    TKN_IS_OP;
},
"_PARAM")

DEF_LEX (T_Type,
(0),
{
    TKN_IS_OP;
},
"_TYPE" ERR)

DEF_LEX (T_Announce,
(0),
{
    TKN_IS_OP;
},
"_ANNOUNCE")

DEF_LEX (T_Equation,
(0),
{
    TKN_IS_OP;
},
"_EQUATION")

DEF_LEX (T_func_Announce,
(0),
{
    TKN_IS_OP;
},
"_FUNC_ANNOUNCE")

DEF_LEX (T_Crossroads,
(0),
{
    TKN_IS_OP;
},
"_CROSSROADS")

DEF_LEX (T_Expression,
(0),
{
    TKN_IS_OP;
},
"_EXPRESSION")

DEF_LEX (TComment,
LEXEM_IS (L"#"),
{
    TKN_IS_OP;
},
"TComment") //TODO fix comments

DEF_LEX (TVariable,
(*Lexem >= L'A' && *Lexem <= L'я'),
{
    TKN.category = CLine;
    TKN.data.var = wcsdup (Lexem);
},
"TVariable")





DEF_LEX (TuSin,
(0),
{
    TKN_IS_OP;
},
"UN_SIN")

DEF_LEX (TuCos,
(0),
{
    TKN_IS_OP;
},
"UN_COS")

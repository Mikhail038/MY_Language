#define ERR "YOU ARE NOT SUPPOSED TO SEE THAT!"

#define PUT(d_command) writeln_command ( d_command , Back->file)

DEF_OP (TValue,
(*Lexem >= L'0' && *Lexem <= L'9'),
{
    double Value = parse_double (Lexem);

    TKN.category = CValue;
    TKN.data.val = Value;
},
L"TValue",
{
})

DEF_OP (TAssign,
LEXEM_IS (L"="),
{
    TKN_IS_OP;
},
L"ASSIGN",
{
})

DEF_OP (TFinish,
LEXEM_IS (L";"),
{
    TKN_IS_OP;
},
L"FINISH",
{
})

DEF_OP (TaAdd,
LEXEM_IS (L"+"),
{
    TKN_IS_OP;
},
L"ADD",
{
    PUT (add);
})

DEF_OP (TaSub,
LEXEM_IS (L"-"),
{
    TKN_IS_OP;
},
L"SUB",
{
    PUT (sub);
})

DEF_OP (TaMul,
LEXEM_IS (L"*"),
{
    TKN_IS_OP;
},
L"MUL",
{
    PUT (mul);
})

DEF_OP (TaDiv,
LEXEM_IS (L"/"),
{
    TKN_IS_OP;
},
L"DIV",
{
    PUT (m_div);
})

DEF_OP (TaPow,
LEXEM_IS (L"^"),
{
    TKN_IS_OP;
},
L"POW",
{

})

DEF_OP (TcLess,
LEXEM_IS (L"<"),
{
    TKN_IS_OP;
},
L"LESS",
{
})

DEF_OP (TcMore,
LEXEM_IS (L">"),
{
    TKN_IS_OP;
},
L"MORE",
{
})

DEF_OP (TcLessEq,
LEXEM_IS (L"<="),
{
    TKN_IS_OP;
},
L"LESS OR EQUAL",
{
})

DEF_OP (TcMoreEq,
LEXEM_IS (L">="),
{
    TKN_IS_OP;
},
L"MORE OR EQUAL",
{
})

DEF_OP (TcEqual,
LEXEM_IS (L"=="),
{
    TKN_IS_OP;
},
L"EQUAL",
{
})

DEF_OP (TcNotEq,
LEXEM_IS (L"!="),
{
    TKN_IS_OP;
},
L"NOT EQUAL",
{
})

DEF_OP (TcNot,
LEXEM_IS (L"!"),
{
    TKN_IS_OP;
},
L"NOT",
{
})

DEF_OP (TcAnd,
LEXEM_IS (L"&&"),
{
    TKN_IS_OP;
},
L"AND",
{
})

DEF_OP (TcOr,
LEXEM_IS (L"||"),
{
    TKN_IS_OP;
},
L"OR",
{
})

DEF_OP (TOpenRoundBracket,
LEXEM_IS (L"("),
{
    TKN_IS_OP;
},
L"(",
{
})

DEF_OP (TCloseRoundBracket,
LEXEM_IS (L")"),
{
    TKN_IS_OP;
},
L")",
{
})

DEF_OP (TOpenBracket,
LEXEM_IS (L"{"),
{
    TKN_IS_OP;
},
L"{" ERR,
{
})

DEF_OP (TCloseBracket,
LEXEM_IS (L"}"),
{
    TKN_IS_OP;
},
L"}" ERR,
{
})

DEF_OP (TComma,
LEXEM_IS (L","),
{
    TKN_IS_OP;
},
L"," ERR,
{
})

DEF_OP (TIf,
LEXEM_IS (L"if"),
{
    TKN_IS_OP;
},
L"IF",
{
})

DEF_OP (TElse,
LEXEM_IS (L"else"),
{
    TKN_IS_OP;
},
L"ELSE",
{
})

DEF_OP (TWhile,
LEXEM_IS (L"while"),
{
    TKN_IS_OP;
},
L"WHILE",
{
})

DEF_OP (TstdType,
LEXEM_IS (L"var"),
{
    TKN_IS_OP;
},
L"stdType",
{
})

DEF_OP (TnoType,
LEXEM_IS (L"void"),
{
    TKN_IS_OP;
},
L"noType",
{
})

DEF_OP (TInput,
LEXEM_IS (L"input"),
{
    TKN_IS_OP;
},
L"INPUT",
{
})

DEF_OP (TOutput,
LEXEM_IS (L"output"),
{
    TKN_IS_OP;
},
L"OUTPUT",
{
})

DEF_OP (TReturn,
LEXEM_IS (L"return"),
{
    TKN_IS_OP;
},
L"RETURN",
{
    generate_return (BACK_FUNC_PARAMETERS);
})

DEF_OP (T_Statement,
(0),
{
    TKN_IS_OP;
},
L"_STATEMENT",
{
})

DEF_OP (T_Function,
(0),
{
    TKN_IS_OP;
},
L"_FUNC",
{
})

DEF_OP (T_Parameters,
(0),
{
    TKN_IS_OP;
},
L"_PARAMETERS",
{
})

DEF_OP (T_Param,
(0),
{
    TKN_IS_OP;
},
L"_PARAM",
{
})

DEF_OP (T_Type,
(0),
{
    TKN_IS_OP;
},
L"_TYPE" ERR,
{
})

DEF_OP (T_Announce,
(0),
{
    TKN_IS_OP;
},
L"_ANNOUNCE",
{
    generate_announce (BACK_FUNC_PARAMETERS);
})

DEF_OP (T_Equation,
(0),
{
    TKN_IS_OP;
},
L"_EQUATION",
{
    generate_equation (BACK_FUNC_PARAMETERS);
})

DEF_OP (T_func_Announce,
(0),
{
    TKN_IS_OP;
},
L"_FUNC_ANNOUNCE",
{
})

DEF_OP (T_Crossroads,
(0),
{
    TKN_IS_OP;
},
L"_CROSSROADS",
{
})

DEF_OP (T_Expression,
(0),
{
    TKN_IS_OP;
},
L"_EXPRESSION",
{
})

DEF_OP (TComment,
LEXEM_IS (L"#"),
{
    TKN_IS_OP;
},
L"TComment",
{
}) //TODO fix comments

DEF_OP (TVariable,
(*Lexem >= L'A' && *Lexem <= L'я'),
{
    TKN.category = CLine;
    TKN.data.var = wcsdup (Lexem);
},
L"TVariable",
{
})


/* Also add changes to BaseUnaryFunc.h */


DEF_OP (TuSqrt,
(0),
{
    TKN_IS_OP;
},
L"UN_SQRT",
{
    PUT (m_sqrt);
})

DEF_OP (TuSin,
(0),
{
    TKN_IS_OP;
},
L"UN_SIN",
{
    PUT (m_sin);
})

DEF_OP (TuCos,
(0),
{
    TKN_IS_OP;
},
L"UN_COS",
{
    PUT (m_cos);
})

DEF_OP (TuTan,
(0),
{
    TKN_IS_OP;
},
L"UN_TAN",
{
    PUT (m_dup);

    PUT (m_sin);
    PUT (m_cos);

    PUT (m_div);
})

DEF_OP (TuCeil,
(0),
{
    TKN_IS_OP;
},
L"UN_CEIL",
{
    PUT (m_ceil);
})

DEF_OP (TuFloor,
(0),
{
    TKN_IS_OP;
},
L"UN_FLOOR",
{
    PUT (m_floor);
})

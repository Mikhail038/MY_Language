#define ERR "YOU ARE NOT SUPPOSED TO SEE THAT!"




DEF_OP (TValue,
(*Lexem >= L'0' && *Lexem <= L'9'),
{
    double Value = parse_double (Lexem);

    TOKEN.category = CategoryValue;
    TOKEN.data.val = Value;
},
L"TValue",
{
})

DEF_OP (TypeAssign,
LEXEM_IS (L"="),
{
    TKN_IS_OP;
},
L"ASSIGN",
{
})

DEF_OP (TypeFinish,
LEXEM_IS (L";"),
{
    TKN_IS_OP;
},
L"FINISH",
{
})

DEF_OP (TypeArithAdd,
LEXEM_IS (L"+"),
{
    TKN_IS_OP;
},
L"ADD",
{
    PUTLN (add);
})

DEF_OP (TypeArithSub,
LEXEM_IS (L"-"),
{
    TKN_IS_OP;
},
L"SUB",
{
    PUTLN (sub);
})

DEF_OP (TypeArithMul,
LEXEM_IS (L"*"),
{
    TKN_IS_OP;
},
L"MUL",
{
    PUTLN (mul);
})

DEF_OP (TypeArithDiv,
LEXEM_IS (L"/"),
{
    TKN_IS_OP;
},
L"DIV",
{
    PUTLN (m_div);
})

DEF_OP (TypeArithPow,
LEXEM_IS (L"^"),
{
    TKN_IS_OP;
},
L"POW",
{

})

DEF_OP (TypeLogicLess,
LEXEM_IS (L"<"),
{
    TKN_IS_OP;
},
L"LESS",
{
    PUT (ja);

    standard_if_jump (Back);
})

DEF_OP (TypeLogicMore,
LEXEM_IS (L">"),
{
    TKN_IS_OP;
},
L"MORE",
{
    PUT (jb);

    standard_if_jump (Back);
})

DEF_OP (TypeLogicLessEqual,
LEXEM_IS (L"<="),
{
    TKN_IS_OP;
},
L"LESS OR EQUAL",
{
    PUT (jae);

    standard_if_jump (Back);
})

DEF_OP (TypeLogicMoreEqual,
LEXEM_IS (L">="),
{
    TKN_IS_OP;
},
L"MORE OR EQUAL",
{
    PUT (jbe);

    standard_if_jump (Back);
})

DEF_OP (TypeLogicEqual,
LEXEM_IS (L"=="),
{
    TKN_IS_OP;
},
L"EQUAL",
{
    PUT (je);

    standard_if_jump (Back);
})

DEF_OP (TypeLogicNotEqual,
LEXEM_IS (L"!="),
{
    TKN_IS_OP;
},
L"NOT EQUAL",
{
    PUT (jne);

    standard_if_jump (Back);
})

DEF_OP (TypeLogicNot,
LEXEM_IS (L"!"),
{
    TKN_IS_OP;
},
L"NOT",
{
})

DEF_OP (TypeLogicAnd,
LEXEM_IS (L"&&"),
{
    TKN_IS_OP;
},
L"AND",
{
    PUTLN (mul);

    PUT (push);
    fprintf (Back->file, " %d\n", MY_FALSE);

    PUT (jne);

    standard_if_jump (Back);
})

//TODO ABS ERROR THINK LATER
DEF_OP (TypeLogicOr,
LEXEM_IS (L"||"),
{
    TKN_IS_OP;
},
L"OR",
{
    PUTLN (add);

    PUT (push);
    fprintf (Back->file, " %d\n", MY_FALSE);

    PUT (jne);

    standard_if_jump (Back);
})

DEF_OP (TypeOpenRoundBracket,
LEXEM_IS (L"("),
{
    TKN_IS_OP;
},
L"(",
{
})

DEF_OP (TypeCloseRoundBracket,
LEXEM_IS (L")"),
{
    TKN_IS_OP;
},
L")",
{
})

DEF_OP (TypeOpenBracket,
LEXEM_IS (L"{"),
{
    TKN_IS_OP;
},
L"{" ERR,
{
})

DEF_OP (TypeCloseBracket,
LEXEM_IS (L"}"),
{
    TKN_IS_OP;
},
L"}" ERR,
{
})

DEF_OP (TypeComma,
LEXEM_IS (L","),
{
    TKN_IS_OP;
},
L"," ERR,
{
})

DEF_OP (TypeIf,
LEXEM_IS (L"if"),
{
    TKN_IS_OP;
},
L"IF",
{
    generate_if (BACK_FUNC_PARAMETERS);
})

DEF_OP (TypeElse,
LEXEM_IS (L"else"),
{
    TKN_IS_OP;
},
L"ELSE",
{
})

DEF_OP (TypeWhile,
LEXEM_IS (L"while"),
{
    TKN_IS_OP;
},
L"WHILE",
{
    generate_while (BACK_FUNC_PARAMETERS);
})

DEF_OP (TypeStdVarType,
LEXEM_IS (L"var"),
{
    TKN_IS_OP;
},
L"stdType",
{
})

DEF_OP (TypeVoidVarType,
LEXEM_IS (L"void"),
{
    TKN_IS_OP;
},
L"noType",
{
})

DEF_OP (TypeInput,
LEXEM_IS (L"in"),
{
    TKN_IS_OP;
},
L"INPUT",
{
    generate_input (BACK_FUNC_PARAMETERS);
})

DEF_OP (TypeOutput,
LEXEM_IS (L"out"),
{
    TKN_IS_OP;
},
L"OUTPUT",
{
    generate_output (BACK_FUNC_PARAMETERS);
})

DEF_OP (TypeReturn,
LEXEM_IS (L"return"),
{
    TKN_IS_OP;
},
L"RETURN",
{
    generate_return (BACK_FUNC_PARAMETERS);
})

DEF_OP (TypeLinkerStatement,
(0),
{
    TKN_IS_OP;
},
L"_STATEMENT",
{
})

DEF_OP (TypeLinkerFunction,
(0),
{
    TKN_IS_OP;
},
L"_FUNC",
{
    generate_function (BACK_FUNC_PARAMETERS);
})

DEF_OP (TypeLinkerCall,
(0),
{
    TKN_IS_OP;
},
L"_CALL",
{
    generate_call (BACK_FUNC_PARAMETERS);
})

DEF_OP (TypeLinkerFuncParameters,
(0),
{
    TKN_IS_OP;
},
L"_PARAMETERS",
{
})

DEF_OP (TypeLinkerParameter,
(0),
{
    TKN_IS_OP;
},
L"_PARAM",
{
})

DEF_OP (TypeVaraiableType,
(0),
{
    TKN_IS_OP;
},
L"_TYPE" ERR,
{
})

DEF_OP (TypeLinkerAnnounce,
(0),
{
    TKN_IS_OP;
},
L"_ANNOUNCE",
{
    generate_announce (BACK_FUNC_PARAMETERS);
})

DEF_OP (TypeLinkerEquation,
(0),
{
    TKN_IS_OP;
},
L"_EQUATION",
{
    generate_equation (BACK_FUNC_PARAMETERS);
})

DEF_OP (TypeLinkerFuncAnnounce,
(0),
{
    TKN_IS_OP;
},
L"_FUNC_ANNOUNCE",
{
})

DEF_OP (TypeLinkerCrossroads,
(0),
{
    TKN_IS_OP;
},
L"_CROSSROADS",
{
})

DEF_OP (TypeLinkerExpression,
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

DEF_OP (TypeVariable,
(*Lexem >= L'A' && *Lexem <= L'я'),
{
    TOKEN.category = CategoryLine;
    TOKEN.data.var = wcsdup (Lexem);
},
L"TypeVariable",
{
})


/* Also add changes to BaseUnaryFunc.h */


DEF_OP (TypeUnarySqrt,
(0),
{
    TKN_IS_OP;
},
L"UN_SQRT",
{
    PUTLN (m_sqrt);
})

DEF_OP (TypeUnarySin,
(0),
{
    TKN_IS_OP;
},
L"UN_SIN",
{
    PUTLN (m_sin);
})

DEF_OP (TypeUnaryCos,
(0),
{
    TKN_IS_OP;
},
L"UN_COS",
{
    PUTLN (m_cos);
})

DEF_OP (TypeUnaryTan,
(0),
{
    TKN_IS_OP;
},
L"UN_TAN",
{
    PUTLN (m_dup);

    PUTLN (m_sin);
    PUTLN (m_cos);

    PUTLN (m_div);
})

DEF_OP (TypeUnaryCeil,
(0),
{
    TKN_IS_OP;
},
L"UN_CEIL",
{
    PUTLN (m_ceil);
})

DEF_OP (TypeUnaryFloor,
(0),
{
    TKN_IS_OP;
},
L"UN_FLOOR",
{
    PUTLN (m_floor);
})

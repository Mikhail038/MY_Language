#include "back.h"
#define ERR "YOU ARE NOT SUPPOSED TO SEE THAT!"

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
    x86_add_stack();
})

DEF_OP (TaSub,
LEXEM_IS (L"-"),
{
    TKN_IS_OP;
},
L"SUB",
{
    x86_sub_stack();
})

DEF_OP (TaMul,
LEXEM_IS (L"*"),
{
    TKN_IS_OP;
},
L"MUL",
{
    x86_imul_stack();
})

DEF_OP (TaDiv,
LEXEM_IS (L"/"),
{
    TKN_IS_OP;
},
L"DIV",
{
    x86_idiv_stack();
})

DEF_OP (TaPow,
LEXEM_IS (L"^"),
{
    TKN_IS_OP;
},
L"POW",
{
    //TODO SMTH
})

DEF_OP (TcLess,
LEXEM_IS (L"<"),
{
    TKN_IS_OP;
},
L"LESS",
{
    elf_standard_if_jump (jl_);
})

DEF_OP (TcMore,
LEXEM_IS (L">"),
{
    TKN_IS_OP;
},
L"MORE",
{
    elf_standard_if_jump (jg_);
})

DEF_OP (TcLessEq,
LEXEM_IS (L"<="),
{
    TKN_IS_OP;
},
L"LESS OR EQUAL",
{
    elf_standard_if_jump (jle_);
})

DEF_OP (TcMoreEq,
LEXEM_IS (L">="),
{
    TKN_IS_OP;
},
L"MORE OR EQUAL",
{
    elf_standard_if_jump (jge_);
})

DEF_OP (TcEqual,
LEXEM_IS (L"=="),
{
    TKN_IS_OP;
},
L"EQUAL",
{
    elf_standard_if_jump (je_);
})

DEF_OP (TcNotEq,
LEXEM_IS (L"!="),
{
    TKN_IS_OP;
},
L"NOT EQUAL",
{
    elf_standard_if_jump (jne_);
})

//TODO not work
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
    x86_imul_stack();

    x86_push_i(FALSE);

    elf_standard_if_jump (jne_);
})

//TODO ABS ERROR THINK LATER
DEF_OP (TcOr,
LEXEM_IS (L"||"),
{
    TKN_IS_OP;
},
L"OR",
{
    x86_add_stack();

    x86_push_i(FALSE);

    elf_standard_if_jump (jne_);
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
    elf_generate_if (CurNode);
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
    elf_generate_while (CurNode);
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
LEXEM_IS (L"in"),
{
    TKN_IS_OP;
},
L"INPUT",
{
    elf_generate_input (CurNode);
})

DEF_OP (TOutput,
LEXEM_IS (L"out"),
{
    TKN_IS_OP;
},
L"OUTPUT",
{
    elf_generate_output (CurNode);
})

DEF_OP (TReturn,
LEXEM_IS (L"return"),
{
    TKN_IS_OP;
},
L"RETURN",
{
    elf_generate_return (CurNode);
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
    elf_generate_function (CurNode);
})

DEF_OP (T_Call,
(0),
{
    TKN_IS_OP;
},
L"_CALL",
{
    elf_generate_call (CurNode, RetValueMarker);
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
    elf_generate_announce (CurNode);
})

DEF_OP (T_Equation,
(0),
{
    TKN_IS_OP;
},
L"_EQUATION",
{
    elf_generate_equation (CurNode);
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
    PUTLN (m_sqrt);
})

DEF_OP (TuSin,
(0),
{
    TKN_IS_OP;
},
L"UN_SIN",
{
    PUTLN (m_sin);
})

DEF_OP (TuCos,
(0),
{
    TKN_IS_OP;
},
L"UN_COS",
{
    PUTLN (m_cos);
})

DEF_OP (TuTan,
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

DEF_OP (TuCeil,
(0),
{
    TKN_IS_OP;
},
L"UN_CEIL",
{
    PUTLN (m_ceil);
})

DEF_OP (TuFloor,
(0),
{
    TKN_IS_OP;
},
L"UN_FLOOR",
{
    PUTLN (m_floor);
})

#include "back.h"
#define ERR "YOU ARE NOT SUPPOSED TO SEE THAT!"

DEF_OP (TValue,
(*Lexem >= L'0' && *Lexem <= L'9'),
{
    double Value = parse_double (Lexem);

    TOKEN.category = ValueNode;
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
    x86_add_stack(Back);
})

DEF_OP (TypeArithSub,
LEXEM_IS (L"-"),
{
    TKN_IS_OP;
},
L"SUB",
{
    x86_sub_stack(Back);
})

DEF_OP (TypeArithMul,
LEXEM_IS (L"*"),
{
    TKN_IS_OP;
},
L"MUL",
{
    x86_imul_stack(Back);
})

DEF_OP (TypeArithDiv,
LEXEM_IS (L"/"),
{
    TKN_IS_OP;
},
L"DIV",
{
    x86_idiv_stack(Back);
})

DEF_OP (TypeArithPow,
LEXEM_IS (L"^"),
{
    TKN_IS_OP;
},
L"POW",
{
    //TODO SMTH
})

DEF_OP (TypeLogicLess,
LEXEM_IS (L"<"),
{
    TKN_IS_OP;
},
L"LESS",
{
    elf_standard_if_jump (Back, x86_jl);
})

DEF_OP (TypeLogicMore,
LEXEM_IS (L">"),
{
    TKN_IS_OP;
},
L"MORE",
{
    elf_standard_if_jump (Back, x86_jg);
})

DEF_OP (TypeLogicLessEqual,
LEXEM_IS (L"<="),
{
    TKN_IS_OP;
},
L"LESS OR EQUAL",
{
    elf_standard_if_jump (Back, x86_jle);
})

DEF_OP (TypeLogicMoreEqual,
LEXEM_IS (L">="),
{
    TKN_IS_OP;
},
L"MORE OR EQUAL",
{
    elf_standard_if_jump (Back, x86_jge);
})

DEF_OP (TypeLogicEqual,
LEXEM_IS (L"=="),
{
    TKN_IS_OP;
},
L"EQUAL",
{
    elf_standard_if_jump (Back, x86_je);
})

DEF_OP (TypeLogicNotEqual,
LEXEM_IS (L"!="),
{
    TKN_IS_OP;
},
L"NOT EQUAL",
{
    elf_standard_if_jump (Back, x86_jne);
})

//TODO Not is not working
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
    x86_imul_stack(Back);

    x86_push_imm(Back, MY_FALSE);

    elf_standard_if_jump (Back, x86_jne);
})

//TODO ABS ERROR THINK LATER
DEF_OP (TypeLogicOr,
LEXEM_IS (L"||"),
{
    TKN_IS_OP;
},
L"OR",
{
    x86_add_stack(Back);

    x86_push_imm(Back, MY_FALSE);

    elf_standard_if_jump (Back, x86_jne);
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
    elf_generate_if (Back, CurNode);
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
    elf_generate_while (Back, CurNode);
})

DEF_OP (TypeStdVarType,
LEXEM_IS (L"var"),
{
    TKN_IS_OP;
},
L"VAR_TYPE",
{
})

DEF_OP (TypeVoidVarType,
LEXEM_IS (L"void"),
{
    TKN_IS_OP;
},
L"VOID_TYPE",
{
})

DEF_OP (TypeInput,
LEXEM_IS (L"in"),
{
    TKN_IS_OP;
},
L"INPUT",
{
    elf_generate_call_input_function (Back, CurNode);
})

DEF_OP (TypeOutput,
LEXEM_IS (L"out"),
{
    TKN_IS_OP;
},
L"OUTPUT",
{
    elf_generate_call_output_function (Back, CurNode);
})

DEF_OP (TypeReturn,
LEXEM_IS (L"return"),
{
    TKN_IS_OP;
},
L"RETURN",
{
    elf_generate_return (Back, CurNode);
})

DEF_OP (TypeStatement,
(0),
{
    TKN_IS_OP;
},
L"STATEMENT",
{
})

DEF_OP (TypeFunction,
(0),
{
    TKN_IS_OP;
},
L"FUNCTION",
{
    elf_generate_function (Back, CurNode);
})

DEF_OP (TypeCall,
(0),
{
    TKN_IS_OP;
},
L"CALL",
{
    elf_generate_call (Back, CurNode, RetValueMarker);
})

// DEF_OP (TypeFuncParameters,
// (0),
// {
//     TKN_IS_OP;
// },
// L"PARAMETERS",
// {
// })

DEF_OP (TypeParameter,
(0),
{
    TKN_IS_OP;
},
L"PARAMETER",
{
})

// DEF_OP (TypeVaraiableType,
// (0),
// {
//     TKN_IS_OP;
// },
// L"TYPE" ERR,
// {
// })

DEF_OP (TypeAnnounce,
(0),
{
    TKN_IS_OP;
},
L"ANNOUNCEMENT",
{
    elf_generate_announce (Back, CurNode);
})

DEF_OP (TypeAssignment,
(0),
{
    TKN_IS_OP;
},
L"ASSIGNMENT",
{
    elf_generate_assignment (Back, CurNode);
})

DEF_OP (TypeFuncAnnounce,
(0),
{
    TKN_IS_OP;
},
L"FUNCTION_ANNOUNCEMENT",
{
})

DEF_OP (TypeBranch,
(0),
{
    TKN_IS_OP;
},
L"BRANCH",
{
})

DEF_OP (TypeExpression,
(0),
{
    TKN_IS_OP;
},
L"EXPRESSION",
{
})

// DEF_OP (TComment,
// LEXEM_IS (L"#"),
// {
//     TKN_IS_OP;
// },
// L"TComment",
// {
// }) //TODO fix comments

DEF_OP (TypeVariable,
(*Lexem >= L'A' && *Lexem <= L'я'),
{
    TOKEN.category = NameNode;
    TOKEN.data.var = wcsdup (Lexem);
},
L"TYPE",
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

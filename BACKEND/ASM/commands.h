 #define POP(name) \
    double name = 0; \
     /*printf ("  get poped %lg ip %d \n", name, CPU->ip);*/ \
    pop_from_stack (CPU->stack, &name); \
    //printf ("   poped %lg ip %d \n", name, CPU->ip);

 #define PUSH(name) \
    push_in_stack (CPU->stack, name); \
    //printf ("   pushed %lg\n", name);


//---------------------------------------------------------------------------

#define PRS_STD \
    fprintf (Code->listing_file, "    %0.4d 0x%0.2X %s\n", \
             Code->ip - 1, Code->ArrCode[Code->ip - 1], ArrCommands[counter].name); \
    return 0;

#define REP_STD \
    add_to_array (Array, ArrCommands[counter].name); \
    Code->ip++; \
    Array->Buffer[Array->pointer] = '\0'; \
    Array->pointer++;

//---------------------------------------------------------------------------
#define PRS_JMP \
    if (parse_jump (Source, Code, ArrCommands[counter].name) != 0) \
    { \
        return StdError; \
    } \
    else return 0;

#define REP_JMP \
    add_to_array (Array, ArrCommands[counter].name); \
    reparse_jump (Array, Code, ArrCommands[counter].name);

#define STD_JUMP \
    int label = 0; \
    CPU->ip++; \
    for (int counter = 0; counter < sizeof (int); counter++) \
    { \
        ((unsigned char*) &label)[counter] = CPU->Array[CPU->ip]; \
        CPU->ip++; \
    } \
    CPU->ip = label;

    /*printf ("   jumped %d from %d\n", label, CPU->ip);*/

//===========================================================================

DEF_CMD ("hlt", hlt, 0,
{
    //printf ("halted in %d\n", CPU->ip);
    do_dump (CPU);

    return 1;
},
{
    PRS_STD
},
{
    add_to_array (Array, ArrCommands[counter].name);
    Code->ip++;
    Array->Buffer[Array->pointer] = '\0';
    Array->pointer++;
})

DEF_CMD ("push", push, 1,
{
    run_push (CPU);
},
{
    //printf ("!!!");
    if (parse_push_or_pop (Source, Code, "push") != 0)
    {
        return -1;
    }
    else return 0;
},
{
    add_to_array (Array, ArrCommands[counter].name);
    reparse_push_or_pop (Array, Code, ArrCommands[counter].name);
})

DEF_CMD ("pop", pop, 2,
{
    run_pop (CPU);
},
{
    if (parse_push_or_pop (Source, Code, "pop") != 0)
    {
        return -1;
    }
    else return 0;
},
{
    add_to_array (Array, ArrCommands[counter].name);
    reparse_push_or_pop (Array, Code, ArrCommands[counter].name);
})

DEF_CMD ("add", add, 3,
{
    POP (a);
    POP (b);

    PUSH (a + b);

    CPU->ip++;
},
{
    PRS_STD
},
{
    REP_STD

})

DEF_CMD ("sub", sub, 4,
{
    POP (a);
    POP (b);

    PUSH (b - a);

    CPU->ip++;
},
{
    PRS_STD
},
{
    REP_STD
})

DEF_CMD ("mul", mul, 5,
{
    POP (a);
    POP (b);

    PUSH (a * b);

    CPU->ip++;

},
{
    PRS_STD
},
{
    REP_STD
})

DEF_CMD ("div", m_div, 6,
{
    POP (a);
    POP (b);


    //assert (a != 0);
    if (a == 0)
    {
        printf ("my ass! (zero div) ip = %d\n", CPU->ip);
        //exit (0);
    }

    PUSH (b / a);


    CPU->ip++;

},
{
    PRS_STD
},
{
    REP_STD
})

DEF_CMD ("inp", inp, 7,
{
    double x = 0;

    printf ("Enter number to push:\n");
    scanf ("%lg", &x);

    push_in_stack (CPU->stack, x);

    //printf ("   inputed %lg\n", x);

    CPU->ip++;

},
{
    PRS_STD
},
{
    REP_STD
})

DEF_CMD ("out", out, 8,
{
    POP (x);

    printf ("\n\n""out:  %lg\n", x);
    CPU->ip++;

},
{
    PRS_STD
},
{
    REP_STD
})

DEF_CMD ("dump", dump, 9,
{
    //printf ("Work in progress\n");

    //exit (0); //////////////////////////
    CPU->ip++;
},
{
    PRS_STD
},
{
    REP_STD
})


DEF_CMD ("dup", m_dup, 10,
{
    POP (a);

    PUSH (a);
    PUSH (a);

    CPU->ip++;

},
{
    PRS_STD
},
{
    REP_STD
})

DEF_CMD ("jump", jump, 11,
{
    STD_JUMP


},
{
    PRS_JMP
},
{
    REP_JMP
}
) //jump

DEF_CMD ("ja", ja, 12,
{
    POP(a);
    POP(b);

    if (a > b)
    {
        STD_JUMP
    }
    else
    {
        CPU->ip += 5;
    }


},
{
    PRS_JMP
},
{
    REP_JMP
}
) //ja

DEF_CMD ("jb", jb, 13,
{
    POP(a);
    POP(b);

    if (a < b)
    {
        STD_JUMP
    }
    else
    {
        CPU->ip += 5;
    }


},
{
    PRS_JMP
},
{
    REP_JMP
}
) //jb

DEF_CMD ("jae", jae, 14,
{
    POP(a);
    POP(b);

    if (a >= b)
    {
        STD_JUMP
    }
    else
    {
        CPU->ip += 5;
    }
},
{
    PRS_JMP
},
{
    REP_JMP
}) //jae

DEF_CMD ("jbe", jbe, 15,
{
    POP(a);
    POP(b);

    if (a <= b)
    {
        STD_JUMP
    }
    else
    {
        CPU->ip += 5;
    }


},
{
    PRS_JMP
},
{
    REP_JMP
}) //jbe

DEF_CMD ("je", je, 16,
{
    POP(a);
    POP(b);

    if (a == b)
    {
        STD_JUMP
    }
    else
    {
        CPU->ip += 5;
    }


},
{
    PRS_JMP
},
{
    REP_JMP
}) //je

DEF_CMD ("jne", jne, 17,
{
    POP(a);
    POP(b);

    if (a != b)
    {
        STD_JUMP
    }
    else
    {
        CPU->ip += 5;
    }


},
{
    PRS_JMP
},
{
    REP_JMP
}) //jne


DEF_CMD ("call", call, 18,
{
    //printf ("!\n");

    int label = 0;

    CPU->ip++;

    for (int i = 0; i < sizeof (int); i++)
    {
        ((unsigned char*) &label)[i] = CPU->Array[CPU->ip];
        CPU->ip++;
    }
    push_in_stack (CPU->addres_stack, CPU->ip);

    //printf ("   called %d from %d\n", label, CPU->ip);
    CPU->ip = label;

},
{
    PRS_JMP
},
{
    REP_JMP
})

DEF_CMD ("ret", ret, 19,
{
    double d_label = 0;

    pop_from_stack (CPU->addres_stack, &d_label);

    int label = (int) d_label;

    //printf ("   returned %d from %d\n", label, CPU->ip);
    CPU->ip = label;

},
{
    PRS_STD
},
{
    add_to_array (Array, ArrCommands[counter].name);
    Code->ip++;
    Array->Buffer[Array->pointer] = '\0';
    Array->pointer++;

    Array->Buffer[Array->pointer] = '\0';
    Array->pointer++;
})

DEF_CMD ("sqrt", m_sqrt, 20,
{
    POP (a);

    PUSH (sqrt (a));

    CPU->ip++;
},
{
    PRS_STD
},
{
    REP_STD
})

DEF_CMD ("sin", m_sin, 21,
{
    POP (a);

    PUSH (sin (a));

    CPU->ip++;
},
{
    PRS_STD
},
{
    REP_STD
})

DEF_CMD ("cos", m_cos, 22,
{
    POP (a);

    PUSH (cos (a));

    CPU->ip++;
},
{
    PRS_STD
},
{
    REP_STD
})

DEF_CMD ("ceil", m_ceil, 23,
{
    POP (a);

    PUSH (ceil (a));

    CPU->ip++;
},
{
    PRS_STD
},
{
    REP_STD
})

DEF_CMD ("floor", m_floor, 24,
{
    POP (a);

    PUSH (floor (a));

    CPU->ip++;
},
{
    PRS_STD
},
{
    REP_STD
})

DEF_CMD ("vsetx", vsetx, 25,
{
    CPU->ip++;

    for (int i = 0; i < sizeof (int); i++)
    {
        ((unsigned char*) &width)[i] = CPU->Array[CPU->ip];
        CPU->ip++;
    }

    //printf ("   set vx %d\n", width);
},
{
    seek (Source);

    char* Name = &(Source->Buffer[Source->pointer]);

    int i = 0;
    for (; i < Source->amnt_symbols; i++)
    {
        if ((Name[i] == ' ') || (Name[i] == '\n') || (Name[i] == '\0'))
        {
            Name[i] = '\0';
            break;
        }
    }

    if ((Name[0] <= '9') && ((Name[0] >= '1')))
    {
        parse_int (Source, Code, "vsetx");
    }

    return 0;
},
{
    add_to_array (Array, ArrCommands[counter].name);

    Code->ip++;

    reparse_int (Array, Code);

    Array->Buffer[Array->pointer] = '\0';
    Array->pointer++;
})

DEF_CMD ("vsety", vsety, 26,
{
    CPU->ip++;

    for (int i = 0; i < sizeof (int); i++)
    {
        ((unsigned char*) &height)[i] = CPU->Array[CPU->ip];
        CPU->ip++;
    }

    //printf ("   set vy %d\n", height);
},
{
    seek (Source);

    char* Name = &(Source->Buffer[Source->pointer]);

    int counter = 0;
    for (; counter < Source->amnt_symbols; counter++)
    {
        if ((Name[counter] == ' ') || (Name[counter] == '\n') || (Name[counter] == '\0'))
        {
            Name[counter] = '\0';
            break;
        }
    }

    if ((Name[0] <= '9') && ((Name[0] >= '1')))
    {
        parse_int (Source, Code, "vsety");
    }

    return 0;
},
{
    add_to_array (Array, ArrCommands[counter].name);

    Code->ip++;

    reparse_int (Array, Code);

    Array->Buffer[Array->pointer] = '\0';
    Array->pointer++;
})

DEF_CMD ("updscr", updscr, 27,
{
    for (int i = 0; i < height - 1; i++)
    {
        for (int j = 0; j < width - 1; j++)
        {
            if (CPU->RAM[start + j +  width * i ] >= ' ')
            {
                printf ("%c", (char) CPU->RAM[start + j + width * i]);
            }
            else
            {
                printf (".");
            }
        }
        printf("\n");
    }

    CPU->ip++;
},
{
    PRS_STD
},
{
    REP_STD
})

DEF_CMD ("clrscr", clrscr, 28,
{
    //usleep (0.00001);
    system ("clear");

    CPU->ip++;
},
{
    PRS_STD
},
{
    REP_STD
})

// DEF_CMD ("clrbuf", clrbuf, 29,
// {
//     POP (a);
//
//     PUSH (floor (a));
//
//     CPU->ip++;
// },
// {
//     PRS_STD
// },
// {
//     REP_STD
// })

#undef STD_JUMP

#undef POP

#undef PUSH

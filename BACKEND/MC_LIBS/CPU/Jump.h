 #define POP(name) \
    double name = 0; \
    pop_from_stack (CPU->stack, &name); \
    printf ("   poped %lg\n", name);

#define STD_JUMP \
    int label = 0; \
    for (int i = 0; i < sizeof (int); i++) \
    { \
        ((unsigned char*) &label)[i] = CPU->Array[CPU->ip]; \
        CPU->ip++; \
    } \
    printf ("   jumped %d from %d\n", label, CPU->ip); \
    CPU->ip = label;

JMP_CMD (0,
{
    STD_JUMP
}
) //jump

JMP_CMD (1,
{
    POP(a);
    POP(b);

    if (a > b)
    {
        STD_JUMP
    }
    else
    {
        CPU->ip += 4;
    }
}
) //ja

JMP_CMD (2,
{
    POP(a);
    POP(b);

    if (a < b)
    {
        STD_JUMP
    }
    else
    {
        CPU->ip += 4;
    }
}
) //jb

JMP_CMD (5,
{
    POP(a);
    POP(b);

    if (a >= b)
    {
        STD_JUMP
    }
    else
    {
        CPU->ip += 4;
    }
}
) //jae

JMP_CMD (6,
{
    POP(a);
    POP(b);

    if (a <= b)
    {
        STD_JUMP
    }
    else
    {
        CPU->ip += 4;
    }
}
) //jbe

JMP_CMD (4,
{
    POP(a);
    POP(b);

    if (a == b)
    {
        STD_JUMP
    }
    else
    {
        CPU->ip += 4;
    }
}
) //je

JMP_CMD (7,
{
    POP(a);
    POP(b);

    if (a != b)
    {
        STD_JUMP
    }
    else
    {
        CPU->ip += 4;
    }
}
) //jne

DEF_LEX ((Lexem[0] >= L'0' && Lexem[0] <= L'9'),
{
    wprintf (L"s\n");
})

DEF_LEX ((Lexem[0] >= 'A' && Lexem[0] <= 'z'),
{
    printf ("s\n");
})

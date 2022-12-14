#include "stack.h"

int main ()
{
    long int Capacity = 0;

    scanf ("%ld", &Capacity);

    StructStack stack = {};

    if (STACKCTOR (&stack, Capacity) != 0)
    {
        stack_destructor (&stack);
        return 1;
    }
    INIT (stack.birth);

    double a = 1 + 256*2 + 256*256*3;
    printf ("--%lg--\n", a);
    push_in_stack (&stack, a);

    double b = 0;
    if (pop_from_stack (&stack, &b) != 0)
    {
        stack_destructor (&stack);
        return 1;
    }
    printf ("-%lg-\n", b);

    double c = 0;
    if (peek_from_stack (&stack, &c) != 0)
    {
        stack_destructor (&stack);
        return 1;
    }
    printf ("-%lg-\n", c);

    stack_destructor (&stack);

    return 0;
}

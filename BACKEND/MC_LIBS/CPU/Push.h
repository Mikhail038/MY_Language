#define push_log //do {printf ("%0.4d pushed %lg\n", CPU->ip, x);} while (0)

PUSH_CMD (0,
{
    double x = 0;

    for (int i = 0; i < sizeof (double); i++)
    {
        ((unsigned char*) &x)[i] = CPU->Array[CPU->ip];
        CPU->ip++;
    }

    push_in_stack (CPU->stack, x);
    push_log;
}) //push from code

PUSH_CMD (2,
{
    double x = 0;

    x = CPU->Regs[CPU->Array[CPU->ip] - 1];

    push_in_stack (CPU->stack, x);
    //printf ("pushed %lg\n", x);

    CPU->ip++;
    push_log;

}) //push from register

PUSH_CMD (4,
{
    double x = 0;

    int ram_array_element_number = 0;

    for (int i = 0; i < sizeof (int); i++)
    {
        ((unsigned char*) &ram_array_element_number)[i] = CPU->Array[CPU->ip];
        CPU->ip++;
    }

    x = CPU->RAM[ram_array_element_number];

    push_in_stack (CPU->stack, x);
    push_log;
}) //push from RAM


PUSH_CMD (6,
{
    double x = 0;

    int ram_array_element_number = (int) CPU->Regs[CPU->Array[CPU->ip] - 1];

    x = CPU->RAM[ram_array_element_number];

    push_in_stack (CPU->stack, x);
    CPU->ip++;
    push_log;
}) //push from RAM, number of data is in register

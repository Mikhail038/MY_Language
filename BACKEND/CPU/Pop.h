#define pop_log(x) //do {printf ("%0.4d poped %lg\n", CPU->ip, (x));} while (0)


POP_CMD (0,
{
    double x = 0;
    //printf ("pop ip %d \n", CPU->ip);

    pop_from_stack (CPU->stack, &x);
    pop_log(x);
}) //pop in void

POP_CMD (2,
{
    int i = 0;
    //printf ("pop ip %d \n", CPU->ip);


    i = CPU->Array[CPU->ip];
    CPU->ip++;

    pop_from_stack (CPU->stack, &CPU->Regs[i - 1]);

    pop_log(CPU->Regs[i - 1]);
    //printf ("%0.4d poped in reg %lg\n", CPU->ip, CPU->Regs[i - 1]);
}) //pop in register

POP_CMD (4,
{
    int ram_array_element_number = 0;
    //printf ("pop ip %d \n", CPU->ip);


    for (int i = 0; i < sizeof (int); i++)
    {
        ((unsigned char*) &ram_array_element_number)[i] = CPU->Array[CPU->ip];
        CPU->ip++;
    }

    pop_from_stack (CPU->stack, &CPU->RAM[ram_array_element_number]);
    pop_log(CPU->RAM[ram_array_element_number]);
}) //pop in RAM

POP_CMD (6,
{
    int ram_array_element_number = (int) CPU->Regs[CPU->Array[CPU->ip] - 1];
    //printf ("pop ip %d \n", CPU->ip);

    //printf ("%d %d--\n", CPU->Array[CPU->ip] - 1, ram_array_element_number);

    CPU->ip++;

    pop_from_stack (CPU->stack, &CPU->RAM[ram_array_element_number]);
    pop_log(CPU->RAM[ram_array_element_number]);
}) //pop in RAM, number of data is in register

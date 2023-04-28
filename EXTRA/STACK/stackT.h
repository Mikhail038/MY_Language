
#pragma once

//===================================================================================================================================================================

#include <stdio.h>
#include "MYassert.h"

//===================================================================================================================================================================

#define CapacityMulDivCoefficient 2

//===================================================================================================================================================================

template <typename StackDataType>
struct SStack
{
    int             size            = 0;
    int             capacity        = 0;
    StackDataType*  data            = NULL;
};

//===================================================================================================================================================================

template <typename StackDataType>
bool stack_is_full (SStack<StackDataType>* stack)
{
    return !(stack->capacity * sizeof (stack->data) - stack->size * sizeof (stack->data));
}

template <typename StackDataType>
bool stack_is_empty (SStack<StackDataType>* stack)
{
    if (stack->size < 1)
    {
        return true;
    }

    return false;
}

template <typename StackDataType>
static int make_stack_bigger (SStack<StackDataType>* stack)
{
    if (stack_variator (stack) != 0)
    {
        return 1;
    }

    //printf ("-------'%p' '%d' '%d' '%d'\n", stack->data, stack->capacity, sizeof (*stack->data), CapacityMulDivCoefficient);
    stack->data = (StackDataType*) realloc (stack->data, (stack->capacity > 0) ? stack->capacity * CapacityMulDivCoefficient * sizeof (*stack->data) : (stack->capacity + 1) * CapacityMulDivCoefficient * sizeof (*stack->data) );

    stack->capacity = (stack->capacity > 0) ? stack->capacity * CapacityMulDivCoefficient : (stack->capacity + 1) * CapacityMulDivCoefficient;
    //printf ("-------'%p' '%d' '%d' '%d'\n", stack->data, stack->capacity, sizeof (*stack->data), CapacityMulDivCoefficient);


    if (stack == NULL)
    {
        return 1;
    }

    return 0;
}

template <typename StackDataType>
static int make_stack_smaller (SStack<StackDataType>* stack)
{
    if (stack_variator (stack) != 0)
    {
        return 1;
    }

    //printf ("-------'%p' '%d' of '%d'   '%d' '%d'\n", stack->data, stack->size, stack->capacity, sizeof (*stack->data), CapacityMulDivCoefficient);
    stack->data = (StackDataType*) realloc (stack->data, sizeof (*stack->data) * ( stack->capacity / CapacityMulDivCoefficient));
    stack->capacity /= CapacityMulDivCoefficient;

    //printf ("-------'%p' '%d' of '%d'   '%d' '%d'\n", stack->data, stack->size, stack->capacity, sizeof (*stack->data), CapacityMulDivCoefficient);

    if (stack->data == NULL)
    {
        return 1;
    }

    return 0;
}

template <typename StackDataType>
static int stack_variator (SStack<StackDataType>* stack)
{
    MLA (stack != NULL);
    MLA (stack->data != NULL);

    return 0;
}

template <typename StackDataType>
int stack_constructor (SStack<StackDataType>* stack, int Capacity)
{
    MLA (Capacity >= 0);
    MLA (stack != NULL);
    MLA (stack != NULL);

    stack->capacity = Capacity;
    stack->size = 0;

    stack->data = (StackDataType*) calloc (1, stack->capacity * sizeof (*stack->data));

    if (stack_variator (stack) != 0)
    {
        return 1;
    }

    return 0;
}

template <typename StackDataType>
int push_in_stack (SStack<StackDataType>* stack, StackDataType x)
{
    if (stack_variator (stack) != 0)
    {
        return 1;
    }

     if (stack_is_full (stack))
    {
        if (make_stack_bigger (stack) != 0)
        {
            return 1;
        }
    }

    //printf ("push %lg in %d of %d !!\n", x, stack->size, stack->capacity);
    stack->data[stack->size] = x;
    stack->size++;

    return 0;
}

template <typename StackDataType>
int peek_from_stack (SStack<StackDataType>* stack, StackDataType* x)
{
    if (stack_variator (stack) != 0)
    {
        return 1;
    }

    if (stack_is_empty (stack))
    {
        printf ("Stack is empty!\n");
        //exit (0);
    }
    else
    {
        *x = stack->data[stack->size - 1];
    }

    return 0;
}

template <typename StackDataType>
int pop_from_stack (SStack<StackDataType>* stack, StackDataType* x)
{
    if (stack_variator (stack) != 0)
    {
        return 1;
    }

    if (stack_is_empty (stack))
    {
        printf ("Stack is empty!\n");
        //exit (0);
    }
    else
    {
        *x = stack->data[stack->size - 1];

        stack->size--;
    }

    if ((stack->size * CapacityMulDivCoefficient * CapacityMulDivCoefficient <= stack->capacity) && (stack->capacity > 10))
    {
        if (make_stack_smaller (stack) != 0)
        {
            fprintf (stderr, "Error %s %s %d !\n", LOCATION);
        }
    }

    return 0;
}

template <typename StackDataType>
int stack_destructor (SStack<StackDataType>* stack)
{
    if (stack->data != NULL)
    {
        free (stack->data);
    }

    free (stack);
    stack = NULL;

    return 0;
}

//===================================================================================================================================================================

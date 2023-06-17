
#include "array.h"
#include "MYassert.h"
#include "colors.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

//===================================================================================================================================================================

bool array_is_full (MyArray* Array)
{
    return !(Array->capacity - Array->size);
}

bool array_is_empty (MyArray* Array)
{
    return (Array->size < 1);
}

int make_array_bigger (MyArray* Array)
{
    #ifdef DEBUG
    printf (KGRN "|MyArray|" KNRM "[big^] " KBLU "realloc made" KNRM "\n");
    #endif

    if (array_validator (Array) != 0)
    {
        return 1;
    }

    Array->capacity = (Array->capacity > 0) ?
                        Array->capacity * CapacityMulDivCoefficient :
                        (Array->capacity + 1) * CapacityMulDivCoefficient;

    Array->data = realloc (Array->data, Array->capacity * Array->data_size);

    if (Array == NULL)
    {
        return 1;
    }

    return 0;
}

int make_array_smaller (MyArray* Array)
{
    if (array_validator (Array) != 0)
    {
        return 1;
    }

    //printf ("-------'%p' '%d' of '%d'   '%d' '%d'\n", stack->data, stack->size, stack->capacity, sizeof (*stack->data), CapacityMulDivCoefficient);

    Array->capacity /= CapacityMulDivCoefficient;
    Array->data = realloc (Array->data, Array->data_size * Array->capacity);

    //printf ("-------'%p' '%d' of '%d'   '%d' '%d'\n", stack->data, stack->size, stack->capacity, sizeof (*stack->data), CapacityMulDivCoefficient);

    if (Array->data == NULL)
    {
        return 1;
    }

    return 0;
}

int array_validator (MyArray* Array)
{
    MY_LOUD_ASSERT (Array != NULL);
    MY_LOUD_ASSERT (Array->data != NULL);

    return 0;
}

int array_constructor (MyArray* Array, const size_t DataSize, const size_t Capacity)
{
    MY_LOUD_ASSERT (Array != NULL);

    Array->data_size = DataSize;

    Array->capacity = Capacity;
    Array->size = 0;

    Array->data = calloc (1, Array->capacity * Array->data_size);

    if (array_validator (Array) != 0)
    {
        return 1;
    }

    return 0;
}

int put_in_array (MyArray* Array, void* Data)
{
    if (array_validator (Array) != 0)
    {
        return 1;
    }

    if (array_is_full (Array))
    {
        if (make_array_bigger (Array) != 0)
        {
            return 1;
        }
    }

    size_t Index = 0;
    void* InsertPlace = NULL;

    if (find_free_place_in_array (Array, &Index) == true)
    {
        InsertPlace = (char*) Array->data + Index * Array->data_size;
    }
    else
    {
        MY_LOUD_ASSERT(false);
    }

    #ifdef DEBUG
    printf (KGRN "|MyArray|" KNRM "[put]  index %lu, InsertPlace %p, Data %p\n", Index, InsertPlace, Data);
    #endif

    memcpy (InsertPlace, Data, Array->data_size);

    Array->size++;

    return 0;
}

int take_from_array (MyArray* Array, void* Data, size_t Index)
{
    if (array_validator (Array) != 0)
    {
        return 1;
    }

    if (array_is_empty (Array))
    {
        printf ("Array is empty!\n");
        //exit (0);
        return 0;
    }

    #ifdef DEBUG
    printf (KGRN "|MyArray|" KNRM "[take] index %lu, Data %p\n", Index, Data);
    #endif

    memcpy(Data, (char*) Array->data + Array->data_size * Index, Array->data_size);

    return 0;
}

int pop_from_array (MyArray* Array, void* Data, size_t Index)
{
    take_from_array(Array, Data, Index);

    Array->size--;

    if ((Array->size * CapacityMulDivCoefficient * CapacityMulDivCoefficient <= Array->capacity) && (Array->capacity > 10))
    {
        if (make_array_smaller (Array) != 0)
        {
            fprintf (stderr, "Error %s %s %d !\n", LOCATION);
        }
    }

    return 0;
}

int array_destructor (MyArray* Array)
{
    if (Array->data != NULL)
    {
        free (Array->data);
    }

    free (Array);
    Array = NULL;

    return 0;
}

bool find_in_array (MyArray* Array, void* Data, size_t* Index)
{
    for (size_t cnt = 0; cnt != Array->capacity; ++cnt)
    {
        #ifdef DEBUG
        printf (KGRN "|MyArray|" KNRM "[find] %lu/%lu\n", cnt, Array->capacity);
        #endif

        if (memcmp ((char*) Array->data + cnt * Array->data_size, Data, Array->data_size) == 0)
        {
            *Index = cnt;

            return true;
        }
    }

    return false;
}

bool find_free_place_in_array (MyArray* Array, size_t* Index)
{
    void* EmptyData = calloc(1, Array->data_size);

    bool RetValue = find_in_array(Array, EmptyData, Index);

    free (EmptyData);

    return RetValue;
}

void free_array (MyArray* Array) //if elements of array are pointers
{
    for (size_t cnt = 0; cnt != Array->capacity; ++cnt)
    {
        void** DataPtr = (void**) ((char*) Array->data + cnt * Array->data_size);

        free (*DataPtr);

        #ifdef DEBUG
        printf (KGRN "|MyArray|" KNRM "freed %p %lu/%lu\n", DataPtr, cnt, Array->size);
        #endif
    }
}

bool check_not_null (const void* Data, const size_t DataSize)
{
    for (size_t cnt = 0; cnt != DataSize; ++cnt)
    {
        if (((char*) Data)[cnt] != 0)
        {
            return true;
        }
    }

    return false;
}

//===================================================================================================================================================================

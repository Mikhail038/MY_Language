
#include "array.h"
#include "MYassert.h"
#include "colors.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

//===================================================================================================================================================================

bool array_is_full (const MyArray* Array)
{
    return !(Array->capacity - Array->size);
}

bool array_is_empty (const MyArray* Array)
{
    return (Array->size < 1);
}

bool make_array_bigger (MyArray* Array)
{
    if (!array_is_full (Array))
    {
        return false;
    }

    #ifdef DEBUG
    printf (KGRN "|MyArray|" KNRM "[big^] " KBLU "realloc made" KNRM "\n");
    #endif

    if (array_validator (Array) != 0)
    {
        return true;
    }

    Array->capacity = (Array->capacity == 0) ?
                        Array->capacity * CapacityMulDivCoefficient :
                        CapacityMulDivCoefficient;

    Array->data = realloc (Array->data, Array->capacity * Array->data_size);

    if (Array == NULL)
    {
        return true;
    }

    return false;
}

bool make_array_smaller (MyArray* Array)
{
    if (!((Array->size * CapacityMulDivCoefficient * CapacityMulDivCoefficient <= Array->capacity) && (Array->capacity > 10)))
    {
        return false;
    }

    if (array_validator (Array) != 0)
    {
        return true;
    }


    Array->capacity /= CapacityMulDivCoefficient;

    MY_LOUD_ASSERT(Array->capacity != 0);
    Array->data = realloc (Array->data, Array->data_size * Array->capacity);


    if (Array->data == NULL)
    {
        return true;
    }

    return false;
}

bool array_validator (const MyArray* Array)
{
    MY_SILENT_ASSERT (Array != NULL);
    MY_SILENT_ASSERT (Array->data != NULL);

    return false;
}

bool array_constructor (MyArray* Array, const size_t DataSize, const size_t Capacity)
{
    MY_SILENT_ASSERT (Array != NULL);

    Array->data_size = DataSize;

    Array->capacity = Capacity;
    Array->size = 0;

    Array->data = calloc (1, Array->capacity * Array->data_size);

    if (array_validator (Array) == true)
    {
        return true;
    }

    return false;
}

bool put_in_tail_of_array (MyArray* Array, const void* Data)
{
    if (array_validator (Array) == true)
    {
        return true;
    }


    if (make_array_bigger (Array) == true)
    {
        return true;
    }


    void* InsertPlace = NULL;

    InsertPlace = (char*) Array->data + Array->size * Array->data_size;

    #ifdef DEBUG_2
    printf (KGRN "|MyArray|" KNRM "[put]  index %lu, InsertPlace %p, Data %p\n", Index, InsertPlace, Data);
    #endif

    memcpy (InsertPlace, Data, Array->data_size);

    Array->size++;

    return false;
}

bool take_from_array (MyArray* Array, void* Data, const size_t Index)
{
    if (array_validator (Array) == true)
    {
        return true;
    }

    if (array_is_empty (Array))
    {
        printf ("Array is empty!\n");
        //exit (0);
        return false;
    }

    #ifdef DEBUG_2
    printf (KGRN "|MyArray|" KNRM "[take] index %lu, Data %p\n", Index, Data);
    #endif

    memcpy(Data, (char*) Array->data + Array->data_size * Index, Array->data_size);

    return false;
}

bool pop_from_array (MyArray* Array, void* Data, const size_t Index)
{
    take_from_array(Array, Data, Index);

    Array->size--;

    if (make_array_smaller (Array) != 0)
    {
        fprintf (stderr, "Error %s %s %d !\n", LOCATION);
    }


    return false;
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

bool find_in_array (const MyArray* Array, const void* Data, size_t* Index)
{
    for (size_t cnt = 0; cnt != Array->capacity; ++cnt)
    {
        #ifdef DEBUG_2
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
    void* EmptyData[Array->data_size];

    bool RetValue = find_in_array(Array, EmptyData, Index);

    return RetValue;
}

void free_array (MyArray* Array) //if elements of array are pointers
{
    for (size_t cnt = 0; cnt != Array->capacity; ++cnt)
    {
        void** DataPtr = (void**) ((char*) Array->data + cnt * Array->data_size);

        free (*DataPtr);

        #ifdef DEBUG_2
        printf (KGRN "|MyArray|" KNRM "freed %p %lu/%lu\n", DataPtr, cnt, Array->size);
        #endif
    }

    return;
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

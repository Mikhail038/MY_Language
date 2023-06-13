#pragma once
//===================================================================================================================================================================
//13.06.23
//===================================================================================================================================================================

#include <cstddef>
#include <cstring>
#include <stdio.h>
#include "MYassert.h"

//===================================================================================================================================================================

#define CapacityMulDivCoefficient 2

//===================================================================================================================================================================

struct MyArray
{
    size_t  data_size   = 0;
    size_t  size        = 0;
    size_t  capacity    = 0;
    void*   data        = NULL;
};

//===================================================================================================================================================================

bool array_is_full (MyArray* Array);

bool array_is_empty (MyArray* Array);

int make_array_bigger (MyArray* Array);

int make_array_smaller (MyArray* Array);

int array_variator (MyArray* Array);

int array_constructor (MyArray* Array, size_t Size, size_t Capacity = 20); //TODO

int put_in_array (MyArray* Array, void* Data);

int take_from_array (MyArray* Array, void* Data, size_t Index);

int pop_from_array (MyArray* Array, void* Data, size_t Index);

int array_destructor (MyArray* Array);

bool find_in_array (MyArray* Array, void* Data, size_t* Index);

bool find_free_place_in_array (MyArray* Array, size_t* Index);

void free_array (MyArray* Array);

bool check_not_null (void* Data, size_t DataSize);

//===================================================================================================================================================================

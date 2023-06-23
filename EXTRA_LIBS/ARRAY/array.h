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

bool array_is_full (const MyArray* Array);

bool array_is_empty (const MyArray* Array);

bool make_array_bigger (MyArray* Array);

bool make_array_smaller (MyArray* Array);

bool array_validator (const MyArray* Array);

bool array_constructor (MyArray* Array, const size_t Size, const size_t Capacity = 20);

bool put_in_tail_of_array (MyArray* Array, const void* Data);

bool take_from_array (MyArray* Array, void* Data, const size_t Index);

bool pop_from_array (MyArray* Array, void* Data, const size_t Index);

int array_destructor (MyArray* Array);

bool find_in_array (const MyArray* Array, const void* Data, size_t* Index);

bool find_free_place_in_array (MyArray* Array, size_t* Index);

void free_array (MyArray* Array);

bool check_not_null (void* Data, size_t DataSize);

//===================================================================================================================================================================

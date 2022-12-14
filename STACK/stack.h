#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include "MYassert.h"

//---------------------------------------------------------------------------------------
// 1   - error
// 2   - Capacity < 0
// 4   - stack null ptr
// 8   - size > capacity
// 16  - birds are dead
// 32  - birth file null
// 64  - birth func null
// 128 - source file null
// 256 - source func null
//---------------------------------------------------------------------------------------
#ifndef PURPLE
#ifndef RED
#ifndef ORANGE
#ifndef YELLOW
#ifndef GREEN

#define GREEN

#endif
#endif
#endif
#endif
#endif



#define LOCATION __FILE__,__PRETTY_FUNCTION__,__LINE__



#define DOTXT_1(Message) #Message+1

#define STACKCTOR(stack, Capacity) stack_constructor (stack, Capacity, DOTXT_1 (stack))

#ifdef PURPLE
#define BGN                                                                \
        do                                                                 \
        {                                                                  \
            fprintf (stderr, /*"I am alive!"*/"%-92s|\n", __PRETTY_FUNCTION__); \
        } while (0)

#define END                                                                 \
        do                                                                  \
        {                                                                   \
            fprintf (stderr, /*"I am dead! "*/"%-92s^ \n", __PRETTY_FUNCTION__); \
        } while (0)
#endif

#ifndef PURPLE

#define BGN
#define END

#endif

#define MSA(Condition, ReturnNum)                     \
        do                                            \
        {                                             \
            if (!(Condition))                         \
            {                                         \
                stack->err += ReturnNum;              \
            }                                         \
        } while (0)

#define INIT(stack)                                                             \
        do                                                                    \
        {                                                                     \
            initialize_info (stack, LOCATION, DOTXT (stack));                                  \
        } while (0)

typedef double StackDataType;

typedef double CanaryType;

typedef struct
{
    char* name = NULL;
    char* file = NULL;
    char* func = NULL;
    int   line;
} StructInfo;

typedef struct
{
    CanaryType      front_canary    = 0;
    unsigned int    hash            = 0;
    unsigned int    hash_data       = 0;
    int             size            = 0;
    int             capacity        = 0;
    StackDataType*  data            = NULL;
    CanaryType      canary          = 0;
    int             err             = 0;
    StructInfo*     birth           = NULL;
    StructInfo*     source          = NULL;
    CanaryType      end_canary      = 0;
} StructStack;


#ifdef GREEN
    static const int PROTECTION_LEVEL = 0;
#endif

#ifdef YELLOW
    static const int PROTECTION_LEVEL = 1;
#endif

#ifdef ORANGE
    static const int PROTECTION_LEVEL = 2;
#endif

#ifdef RED
    static const int PROTECTION_LEVEL = 3;
#endif

#ifdef PURPLE
    static const int PROTECTION_LEVEL = 4;
#endif

//unsigned int hash_h31 (const void* array, int size);

//unsigned int djb33_hash(const char* s, int len);

unsigned int HashFAQ6 (const char* Str, int Size);

bool stack_is_full (StructStack* stack);

bool stack_is_empty (StructStack* stack);

void swap_byte_by_byte (void* FirstData, void* SecondData, int Size);

int compare_byte_by_byte (void* FirstData, void* SecondData, int Size);

void copy_byte_by_byte (void* FirstData, void* SecondData, int Size);

static void create_canary (StructStack* stack);

int check_stack_front_canary (StructStack* stack);

int check_stack_end_canary (StructStack* stack);

int check_data_front_canary (StructStack* stack);

int check_data_end_canary (StructStack* stack);

int check_data_hash (StructStack* stack);

int check_stack_hash (StructStack* stack);

int make_hash (StructStack* stack);

static int make_stack_bigger (StructStack* stack);

static void poison (void* Victim, int Size);

static void poison_array (void* VictimArray, int ArraySize, int Size);

static int stack_variator (StructStack* stack);

void stack_dump (StructStack* stack);

int initialize_info (StructInfo* info, const char* File, const char* Function, int Line, const char* Name);

static void print_head (StructStack* stack, FILE* stream);

static void print_error (const char* ErrorName, StructInfo* info, FILE* Stream);

static void print_stack_data_double (StructStack* stack);

void move_canary (StructStack* stack, int OldCapasity);

/*!
    @brief Function that initializes stack

    @param[in] Amount maximum amount of objects in stack
    @param[in] Size size of one object

    @return returns pointer to the stack structure

*/
int stack_constructor (StructStack* stack, int Capacity, const char* Name);

int push_in_stack (StructStack* stack, StackDataType x);

int peek_from_stack (StructStack* stack, StackDataType* x);

int pop_from_stack (StructStack* stack, StackDataType* x);

int stack_destructor (StructStack* stack);

#endif

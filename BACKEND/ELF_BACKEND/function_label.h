
#include <cstdlib>
#include <cstring>
#include <cstdio>

//TODO do not start your defines' name from "_" or "__"
#define _LOCATION __PRETTY_FUNCTION__, __FILE__, __LINE__

// TODO make more universal debug define, I've described it in elf_back.cpp:51
#ifdef DEBUG_3
#define START_ \
do \
{ \
    printf ("==%s %s:%d\n", _LOCATION);\
} \
while (0)
#else
#define START_
#endif



const int MAX_CALLS_OF_ONE_LABEL = 50;
const int NULL_FINISH = 0;

struct JumpLabel final
{
    size_t  amount;
    size_t* start;
    size_t finish;

    JumpLabel(const size_t Finish = NULL_FINISH) : amount(0), finish(Finish)
    {
        START_;

        start = (size_t*) calloc (MAX_CALLS_OF_ONE_LABEL, sizeof (size_t));
    }

    JumpLabel(const size_t FirstStart, const size_t Finish) : amount(1), finish(Finish)
    {
        START_;

        start = (size_t*) calloc (MAX_CALLS_OF_ONE_LABEL, sizeof (size_t));

        start[0] = FirstStart;
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------------

    // TODO you can write coppy-constructor with using of copy-assigment
    //      same for move-constructor
    
    JumpLabel (const JumpLabel& other) :
        amount(other.amount),
        finish(other.finish)
    {
        START_;

        start = (size_t*) calloc (MAX_CALLS_OF_ONE_LABEL, sizeof (size_t));
        memcpy(start, other.start, MAX_CALLS_OF_ONE_LABEL);
    }

    JumpLabel& operator= (JumpLabel&& other)
    {
        START_;

        if (this == &other)
        {
            return *this;
        }

        finish = other.finish;
        amount = other.amount;
        start = other.start;

        other.start = nullptr;

        return *this;
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------------

    JumpLabel (JumpLabel&& other) :
        amount(other.amount),
        start(other.start),
        finish(other.finish)
    {
        START_;

        other.start = nullptr;
    }

    JumpLabel& operator= (const JumpLabel& other)
    {
        START_;

        if (this == &other)
        {
            return *this;
        }

        finish = other.finish;
        amount = other.amount;

        start = (size_t*) calloc (MAX_CALLS_OF_ONE_LABEL, sizeof (size_t));
        memcpy(start, other.start, MAX_CALLS_OF_ONE_LABEL);

        return *this;
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------------

    ~JumpLabel()
    {
        START_;

        free (start);
    }
};

#undef _START
#undef _LOCATION

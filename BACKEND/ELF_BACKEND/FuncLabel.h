
#include <cstdlib>
#include <cstring>
#include <cstdio>

#define LOCATION __PRETTY_FUNCTION__, __FILE__, __LINE__

#define START_  printf ("==%s %s:%d\n", LOCATION)
#define START_

#define MAX_CALLS_OF_ONE_FUNC 50
#define NULL_FINISH 0

typedef struct FuncLabel
{
    size_t  amount = 0;
    size_t* start;
    // std::list<size_t> start;
    size_t finish = NULL_FINISH;

    FuncLabel()
    {
        START_;

        start = (size_t*) calloc (MAX_CALLS_OF_ONE_FUNC, sizeof (size_t));
    }

    FuncLabel(size_t Finish)
    {
        START_;

        finish = Finish;
        start = (size_t*) calloc (MAX_CALLS_OF_ONE_FUNC, sizeof (size_t));
        amount = 0;
    }

    FuncLabel(size_t FirstStart, size_t Finish)
    {
        START_;

        finish = Finish;
        start = (size_t*) calloc (MAX_CALLS_OF_ONE_FUNC, sizeof (size_t));

        start[0] = FirstStart;
        amount = 1;
    }

    FuncLabel (const FuncLabel& other)
    {
        START_;

        finish = other.finish;
        amount = other.amount;

        start = (size_t*) calloc (MAX_CALLS_OF_ONE_FUNC, sizeof (size_t));
        memcpy(start, other.start, MAX_CALLS_OF_ONE_FUNC);
    }

    FuncLabel& operator= (const FuncLabel& other)
    {
        START_;

        if (this == &other)
        {
            return *this;
        }

        finish = other.finish;
        amount = other.amount;

        start = (size_t*) calloc (MAX_CALLS_OF_ONE_FUNC, sizeof (size_t));
        memcpy(start, other.start, MAX_CALLS_OF_ONE_FUNC);

        return *this;
    }

    FuncLabel& operator= (FuncLabel&& other)
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

    FuncLabel (FuncLabel&& other)
    {
        START_;

        finish = other.finish;
        amount = other.amount;
        start = other.start;

        other.start = nullptr;
    }

    ~FuncLabel()
    {
        START_;

        free (start);
    }
};


#include <cstdlib>
#include <cstring>
#include <cstdio>

#define LOCATION __PRETTY_FUNCTION__, __FILE__, __LINE__

#define START_  printf ("==%s %s:%d\n", LOCATION)

#undef START_
#define START_

const int MAX_CALLS_OF_ONE_LABEL = 50;
const int NULL_FINISH = 0;

struct JumpLabel
{
    size_t  amount = 0;
    size_t* start;
    size_t finish = NULL_FINISH;

    JumpLabel()
    {
        START_;

        start = (size_t*) calloc (MAX_CALLS_OF_ONE_LABEL, sizeof (size_t));
    }

    JumpLabel(size_t Finish)
    {
        START_;

        finish = Finish;
        start = (size_t*) calloc (MAX_CALLS_OF_ONE_LABEL, sizeof (size_t));
    }

    JumpLabel(size_t FirstStart, size_t Finish)
    {
        START_;

        finish = Finish;
        start = (size_t*) calloc (MAX_CALLS_OF_ONE_LABEL, sizeof (size_t));

        start[0] = FirstStart;
        amount = 1;
    }

    JumpLabel (const JumpLabel& other)
    {
        START_;

        finish = other.finish;
        amount = other.amount;

        start = (size_t*) calloc (MAX_CALLS_OF_ONE_LABEL, sizeof (size_t));
        memcpy(start, other.start, MAX_CALLS_OF_ONE_LABEL);
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

    JumpLabel (JumpLabel&& other)
    {
        START_;

        finish = other.finish;
        amount = other.amount;
        start = other.start;

        other.start = nullptr;
    }

    ~JumpLabel()
    {
        START_;

        free (start);
    }
};

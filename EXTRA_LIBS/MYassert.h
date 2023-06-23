#ifndef MY_ASSERT_H
#define MY_ASSERT_H

#include <stdio.h>
#include <stdlib.h>
#include "colors.h"

#include <errno.h>

#define LOCATION __PRETTY_FUNCTION__, __FILE__, __LINE__

#define DOTXT(Message) #Message

#define MCP(Message)                                                                                       \
        do                                                                                                 \
        {                                                                                                  \
            fprintf (stderr, KNRM "errno [%d] Failed check " KRED Kbright DOTXT (Message) " "                              \
            KNRM "in " KYLW "%s" KNRM " in " KWHT Kunderscore "%s:%d\n" KNRM, errno,                             \
             __PRETTY_FUNCTION__, __FILE__, __LINE__ );                                                    \
        } while (0)


#define MY_COOLER_ASSERT(Condition, ReturnNum)                     \
        do                                            \
        {                                             \
            if (!(Condition))                         \
            {                                         \
                MCP (Condition);                      \
                return ReturnNum;                     \
            }                                         \
        } while (0)

#define MY_SILENT_ASSERT(Condition)                     \
        do                                            \
        {                                             \
            if (!(Condition))                         \
            {                                         \
                MCP (Condition);                      \
            }                                         \
        } while (0)


#define MY_LOUD_ASSERT(Condition)                                \
        do                                            \
        {                                             \
            if (!(Condition))                         \
            {                                         \
                MCP (Condition);                      \
                exit (0);                             \
            }                                         \
        } while (0)

#define MTokAss(Condition)                                \
        do                                            \
        {                                             \
            if (!(Condition))                         \
            {                                         \
                MCP (Condition);                      \
                wprintf (L"==[%d] %d %d==\n", Tokens->number, Tokens->array[Tokens->number].category, Tokens->array[Tokens->number].type); \
                exit (0);                             \
            }                                         \
        } while (0)

#define MCE(Condition, ReturnNum)                     \
        do                                            \
        {                                             \
            if (!(Condition))                         \
            {                                         \
                Errors += ReturnNum;                  \
            }                                         \
        } while (0)

#endif

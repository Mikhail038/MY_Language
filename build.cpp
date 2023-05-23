
#include <cstdlib>
#include <cstdio>
#include <cstring>

int main (int argc, char** argv)
{
    const char* FileName = ((argc > 1) && (strlen (argv[1]) > 3)) ? argv[1] : "EXAMPLES/code.ger";
    if (argc > 2)
    {
        FileName = argv[2];
    }

    size_t length = strlen (FileName) + 60;

    char* Command = (char*) calloc (sizeof (*FileName), length);

    sprintf (Command, "./front %s", FileName);

    system (Command);

    free (Command);

    system ("./back_elf");
}

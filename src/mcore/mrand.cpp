
#include <stdio.h>
#include <stdlib.h>
#include "mrand.h"

char* rand_str(char *dest, int len)
{
    char *s = dest;
    char charset[] = "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (len-- > 0) {
        size_t index = (rand() / RAND_MAX) % (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';

    return s;
}

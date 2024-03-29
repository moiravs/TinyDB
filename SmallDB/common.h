#ifndef __COMMON_H
#define __COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int _checked(int ret)
{
    if (ret < 0)
    {
        printf("ERROR number : %d %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return ret;
}

// The macro allows us to retrieve the name of the calling function
#define checked(call) _checked(call)

// Même macro que checked mais pour write() (où 0 signifie
// aussi une erreur).
#define checked_wr(call) _checked((call)-1)

#define MAX_CLIENT 20

#endif // __COMMON_H
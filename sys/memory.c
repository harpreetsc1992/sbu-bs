#include <sys/memory.h>

void
*memset(
        void *b,
        int c,
        int len
       )
{
    unsigned char *p = b;

    while(len > 0)
    {
        *p = c;
        p++;
        len--;
    }

    return b;
}

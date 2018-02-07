#ifndef _K_UTILS_H
#define _K_UTILS_H

#include <sys/defs.h>
#include <sys/page_alloc.h>

#define ROUNDDOWN(a, n)                     \
({                                          \
    uint64_t __a = (uint64_t) (a);          \
    (__typeof__(a)) (__a - __a % (n));          \
})

#define ROUNDUP(a, n)                                       \
({                                                          \
    uint64_t __n = (uint64_t) (n);                          \
    (__typeof__(a)) (ROUNDDOWN((uint64_t) (a) + __n - 1, __n)); \
})

#define ALIGN_DOWN(x) (x & ~(PAGE_SIZE - 1))

char *
itoa(
     int value,
     char *str,
     int base
	);

uint64_t
stoi(
     const char *str
    );

uint64_t
power(
      uint64_t base, int pow
     );

uint64_t
oct2dec(
        uint64_t oct
       );

#endif

#ifndef _USER_MEMORY_H
#define _USER_MEMORY_H

#include <stdio.h>
#include <sys/defs.h>

void
memcpy(
       void *dest,
       void *src,
       size_t n
      );

void
memmove(
        void *dest,
        void *src,
        size_t n
       );

#endif

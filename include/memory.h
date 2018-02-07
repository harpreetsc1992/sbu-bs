#ifndef MEMORY_H
#define MEMORY_H

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

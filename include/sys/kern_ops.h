#ifndef _KERN_OPS_H
#define _KERN_OPS_H

#include <sys/defs.h>

void
kmemcpy(
       void *dest,
       void *src,
       size_t n
       );

void
kmemmove(
        void *dest,
        void *src,
        size_t n
        );

#endif

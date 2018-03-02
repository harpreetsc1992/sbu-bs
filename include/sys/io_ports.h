#ifndef _IO_PORTS_H
#define _IO_PORTS_H

#include <sys/defs.h>

void
outb(
     uint16_t port,
     uint8_t val
    );

unsigned char
inb(
    uint16_t port
   );

#endif

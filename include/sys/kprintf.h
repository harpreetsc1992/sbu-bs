#ifndef _KPRINTF_H
#define _KPRINTF_H

#include <sys/kern_ops.h>

#define TABSPACE   4
#define HOR_BITS  80
#define VERT_BITS 24
#define TIMER_X   60
#define VIDEO_ADDR 0xFFFFFFFF800B8000

extern int x, y;
extern unsigned short *placetextat;
extern short print_to_screen;

void
placechar(
          unsigned char c,
          int x,
          int y,
          int attr
         );

void
print_timer(
            const char *fmt,
            ...
           );

void
kprintf(
        const char *fmt,
		...
	   );

int
putc(
	 char c
	);

#endif

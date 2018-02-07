#ifndef _KERN_OPS_H
#define _KERN_OPS_H

#include <sys/defs.h>

extern char *check;

char *
kstrtok(
        char *str,
        char *delim
       );

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

uint64_t 
kstrlen(
		const char *str
	   );

void
kstrcpy(
		char *dst,
		const char *src
	   );

void
kstrncpy(
          char *dst,
          const char *src,
          uint64_t n
         );

char*
kstrcat(
		char *str1,
		const char *str2
	   );

int
kstrcmp(
        const char *str1,
        const char *str2
       );

int
kstrncmp(
         const char *str1,
         const char *str2,
         uint64_t n
        );
#endif

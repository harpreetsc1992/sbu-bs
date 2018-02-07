#ifndef _STRING_H
#define _STRING_H
#include <sys/defs.h>

extern char *check;

char *
strtok(
       char *str,
       char *delim
      );

int
strcmp(
       const char *str1,
       const char *str2
      );

int
strncmp(
        const char *str1,
        const char *str2,
        uint64_t n
       );

uint64_t
strlen(
       const char *str
      );

void
strcpy(
       char *dst,
       const char *src
      );

void
strncpy(
        char *dst,
        const char *src,
        uint64_t n
       );

char*
strcat(
       char *str1,
       const char *str2
      );

#endif

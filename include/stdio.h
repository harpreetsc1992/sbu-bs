#ifndef _STDIO_H
#define _STDIO_H

static const int EOF = -1;

int putchar(int c);
int puts(const char *s);
void printf(const char *fmt, ...);

char *gets(char *s);

#endif

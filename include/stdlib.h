#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/defs.h>

#define ARR_SIZE 512

int main(int argc, char *argv[], char *envp[]);
int exit(int status);

void *malloc(size_t size);
void free(void *ptr);

#endif

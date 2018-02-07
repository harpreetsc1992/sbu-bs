#include <stdio.h>
#include <sys/syscall.h>

int
putchar(
		int c
	   )
{
	return __syscall1(20, c);
}

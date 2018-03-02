#include <stdlib.h>
#include <stdio.h>

void _start(uint64_t *stack) {
	char **argv;
	char **envp;
	int argc;
	int res;

	__asm__ volatile(
			"mov %%rbx, %0\t\n"
			: "=r" (stack)
			);

	argc = (int)*((uint64_t *)((uint64_t)stack - 0x8));
	int last = 10;
	
	argv = NULL;
	envp = NULL;
	
	int tmp = argc;
	if (0 == argc || 1 == argc)	
	{
			argv = (char **)((uint64_t)stack - 0x8 * last);
			*argv = (char *)*(char **)((uint64_t)argv);
//			argv[0] = *argv;//(char *)*((uint64_t *)((uint64_t)stack - 0x10));
			last++;
			envp = (char **)((uint64_t)stack - 0x8 * last);
//			*envp = (char *)*((uint64_t)envp); 
	}
	else
	{
		argv = (char **)((uint64_t)stack - (0x8 * last));
		while (--tmp)	
		{
			*(argv + (last - 2))= *(char **)((uint64_t)stack - (0x8 * last));
			last++;
		}
		envp = (char **)((uint64_t)stack - (0x8 * last));
		*envp = (char *)((uint64_t)stack - (0x8 * last)); 
	}
/*
	__asm__ __volatile__(
				"mov %0, %%rbx\t\n"
				:: "r" ((uint64_t)*argv)
				);
*/
//	__asm__ volatile(
//			"mov %0, %%rbx\t\n"
//			:: "r" (stack)
//			);
	
	res = main(argc, argv, envp);

	exit(res);
}

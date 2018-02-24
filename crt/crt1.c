#include <stdlib.h>

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
	int last = 2;
	
	argv = NULL;
	envp = NULL;
	
	int tmp = argc;
	if (0 == argc)	
	{
			argv = (char **)((uint64_t *)((uint64_t)stack - 0x10));
			last++;
			envp = (char **)((uint64_t *)((uint64_t)stack - 0x18));
	}
	else
	{
		argv = (char **)((uint64_t *)((uint64_t)stack - (0x8 * last)));
		while (--tmp)	last++;
		
		envp = (char **)((uint64_t *)((uint64_t)stack - (0x8 * last)));
	}
	
	res = main(argc, argv, envp);

	exit(res);
}

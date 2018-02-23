#include <stdlib.h>

void _start(uint64_t *stack) {
	char **argv;
	char **envp;
	char *argc;
	int res;

	__asm__ volatile(
			"mov %%rbx, %0\t\n"
			: "=r" (stack)
			);

	argc = (char *)((uint64_t)&stack[ARR_SIZE - 2]);
	int tmp = *argc;
	int last = 3;
	
	argv = NULL;
	envp = NULL;
	
	if (0 == argc)	
	{
			argv = (char **)((uint64_t)&stack[ARR_SIZE - last]);
			last++;
			envp = (char **)((uint64_t)&stack[ARR_SIZE - last]);
	}
	else
	{
		while (tmp--)
		{
			argv = (char **)((uint64_t)&stack[ARR_SIZE - last]);
			last++;
		}
		envp = (char **)((uint64_t)&stack[ARR_SIZE - last]);
	}
/*
	if ((*argc >= 1) || (*argc <= 9))
	{
		argv = (char **)((uint64_t)stack + 0x18);
		envp = (char **)((uint64_t)stack + 0x20);
	}
	else
	{
		argc = (uint64_t *)((uint64_t)stack + 0x20);
		argv = (char **)((uint64_t)stack + 0x28);
		envp = (char **)((uint64_t)stack + 0x30);
	}
*/	
	res = main((int)*argc, argv, envp);

	exit(res);
}

#include <stdlib.h>

void _start(uint64_t *stack) {
	char **argv;
	char **envp;
	uint64_t *argc;
	int res;

	__asm__ __volatile__(
			"movq %rsp, %rdi\t\n"
			);

	argc = (uint64_t *)((uint64_t)((void *) stack) + 0x10);
	if ((*argc >= 1) || (*argc <= 9))
	{
		argv = (char **)((uint64_t)((void *) stack) + 0x18);
		envp = (char **)((uint64_t)((void *) stack) + 0x20);
	}
	else
	{
		argc = (uint64_t *)((uint64_t)((void *) stack) + 0x20);
		argv = (char **)((uint64_t)(void *) stack + 0x28);
		envp = (char **)((uint64_t)(void *) stack + 0x30);
	}
	
	res = main((int)*argc, argv, envp);

	exit(res);
}

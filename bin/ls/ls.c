#include <stdio.h>
#include <string.h>
#include <sys/process.h>
#include <unistd.h>
#include <sys/tarfs.h>
#include <stdlib.h>
#include <sys/timer.h>

int main(int argc, char **argv, char **envp)
{
//	printf("%p\n", argv[0]);
	char *pwd = argv[0];
	uint64_t dir_add;

	if (pwd == NULL)
	{
		dir_add = opendir("");
	}
	if (pwd[0] == '/')
	{
		dir_add = opendir(pwd + 1);
	}
	else
	{
		dir_add = opendir(pwd);
	}
    read_dir(dir_add);
	printf("\n");
	return 0;
}

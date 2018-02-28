#include <stdio.h>
#include <string.h>
#include <sys/process.h>
#include <unistd.h>
#include <sys/tarfs.h>
#include <stdlib.h>
#include <sys/timer.h>

int main(int argc, char **argv, char **envp)
{
	char *pwd = "/bin";
	uint64_t dir_add = opendir(pwd + 1);
    read_dir(dir_add);
	printf("\n");
}

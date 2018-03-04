#include <stdio.h>
#include <string.h>
#include <sys/process.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char **argv, char **envp)
{
	printf("\n");
	char *time = argv[0];
    uint64_t ticks = stoi(time);
    sleepshell(ticks);
	return 0;
}

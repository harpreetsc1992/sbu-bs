#include <stdio.h>
#include <string.h>
#include <sys/process.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char **argv, char **envp)
{
    uint64_t ticks = stoi(argv[0]);
    sleepshell(ticks);
	return 0;
}

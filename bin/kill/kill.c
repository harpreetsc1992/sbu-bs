#include <stdio.h>
#include <string.h>
#include <sys/process.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char **argv, char **envp)
{
	int i = 0;
	while (*argv[i++] != ' ');
	char *pid = &(*argv[i]);
	int id = stoi(pid);

	kill(id);
}

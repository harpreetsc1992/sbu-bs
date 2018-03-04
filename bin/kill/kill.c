#include <stdio.h>
#include <string.h>
#include <sys/process.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char **argv, char **envp)
{
	int j = 0;
	while (argv[0][j] != ' ')
	{
		if ('\0' != (argv[0][j + 1]))
			j++;
		else 
			break;
	}
	char *pid = &(argv[0][j + 1]);
	int id = stoi(pid);

	kill((uint64_t)id);
	printf("\n");	
	return 0;
}

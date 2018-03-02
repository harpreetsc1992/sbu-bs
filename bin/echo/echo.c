#include <stdio.h>
#include <string.h>
#include <sys/process.h>
#include <unistd.h>
#include <sys/tarfs.h>
#include <stdlib.h>
#include <sys/timer.h>

int main(int argc, char **argv, char **envp)
{
	char *path = "/bin";
	printf("\n");
	int i = 0;
	char *x = malloc(10);
	*x = 'P';
	*(x + 1) = 'A';
	*(x + 2) = 'T';
	*(x + 3) = 'H';
	*(x + 4) = '\0';
	printf("%s\n", argv[0]);
    if (argv[0] != NULL && strncmp(argv[0], x, 5) == 0)
    {
        printf("%s\n", path);
		return 1;
    }
	printf("Reached here\n");
	while (argv[i] != NULL)
	{
		printf("%s ", argv[i]);
		i++;
	}
	printf("\n");
}

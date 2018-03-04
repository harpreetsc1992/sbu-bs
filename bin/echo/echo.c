#include <stdio.h>
#include <string.h>
#include <sys/process.h>
#include <unistd.h>
#include <sys/tarfs.h>
#include <stdlib.h>
#include <sys/timer.h>

int main(int argc, char **argv, char **envp)
{
	char str[24];
	strcpy(str, (char *)argv[0]);
	printf("%s", str);

	printf("\n");
	return 0;
}

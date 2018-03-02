#include <stdio.h>
#include <string.h>
#include <sys/process.h>
#include <unistd.h>
#include <sys/tarfs.h>
#include <stdlib.h>

int
main(
	 int argc,
	 char **argv,
	 char **envp
	)
{
	char str[32];
/*	__asm__ __volatile__(
				"mov %%rbx, %0\t\n"
				: "=r" (argv[0])
				);
*/  strcpy(str, (char *)argv[0]);
	printf ("%s\n", str);
    int len = strlen(str);
    FILE *fd;
    char buf[64];
    int j = 0;
    int bytes_read;

    str[len] = '\0';

    fd = open(str);

    bytes_read = read(fd, buf, 256);
    while (bytes_read-- != 0)
    {
        printf("%c", buf[j++]);
    }

    printf("\n");
}

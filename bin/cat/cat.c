#include <stdio.h>
#include <string.h>
#include <sys/process.h>
#include <unistd.h>
#include <sys/tarfs.h>
#include <stdlib.h>
#include <sys/timer.h>

int
main(
	 int argc,
	 char **argv,
	 char **envp
	)
{
//	__asm__ __volatile__(
//			"mov %%rbx, %0\t\n"
//			: "=r" ((uint64_t) argv)
//			);
	char str[32];
    strcpy(str, argv[1]);
	printf ("%s\n", str);
    int len = strlen(str);
    FILE *fd;
    char buf[64];
    int j = 0;
    int bytes_read;
//
//    while(*name != '\0')
//    {
//      str[len++] = *name;
//		name++;
//    }

    str[len] = '\0';

    fd = open(str);
//    printf("\n %s", fd->filename);

    bytes_read = read(fd, buf, 256);
    while (bytes_read-- != 0)
    {
        printf("%c", buf[j++]);
    }

    printf("\n");
}

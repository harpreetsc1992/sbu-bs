#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>

pid_t
fork(
	)
{
	return (pid_t)__syscall0(4);
}

int
exit(
	 int status 
	)
{
	return __syscall1(5, status); 
}

uint64_t
sleep(
	  uint64_t seconds
	 )
{
	return (uint64_t)__syscall1(6, seconds);
}

void*
malloc(
	   uint64_t size
	  )
{
	return (void *)(__syscall1(7, size));
}

pid_t
getpid(
	  )
{
	return __syscall0(8);
}

pid_t
getppid(
	   )
{
	return __syscall0(9);
}

void
kill(
	 uint64_t pid
	)
{
	__syscall1(21, (uint64_t) pid);
}

int
execvpe(
		const char *file, 
		char *const argv[], 
		char *const envp[]
	   )
{
	return __syscall3(10, (uint64_t) file, (uint64_t) argv, (uint64_t) envp);
}

uint64_t
opendir(
		char* dir
	   )
{
	uint64_t addr = __syscall1(12, (uint64_t) dir);
	return addr;
}

void
read_dir(
		 uint64_t dir
		)
{
	__syscall1(13, (uint64_t)(dir));
}

FILE*
open(
	 char* file_name
	)
{
	uint64_t addr = __syscall1(14, (uint64_t) file_name);
	if (addr == 0)
	{
		return NULL;
	}
	return (FILE *) addr;
}

ssize_t
read(
	 FILE* fd,
	 char *buf,
	 size_t count
	)
{
	uint64_t bytes_read = (uint64_t)__syscall3(15, (uint64_t) fd, (uint64_t) count, (uint64_t) buf);
	return bytes_read;
}

int
waitpid(
	 	int pid,
		int status
	   )
{
	return __syscall2(16, pid, status);
}

void
ps(
  )
{
	__syscall0(17);
}
/*
int 
closef

int 
closed
*/
void
shutdown(
		)
{
	__syscall0(19);
}

void
sleepshell(
		   uint64_t ticks
		  )
{
	__syscall1(18, ticks);
}

void
clear(
	 )
{
	__syscall0(22);
}

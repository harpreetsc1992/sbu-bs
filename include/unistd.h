#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/defs.h>

FILE* open(char *file_name);
int close(int fd);
ssize_t read(FILE* fd, char *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int unlink(const char *pathname);
void read_dir(uint64_t dir);

int chdir(const char *path);
char *getcwd(char *buf, size_t size);

pid_t fork();
int execvpe(const char *file, char *const argv[], char *const envp[]);
pid_t wait(int *status);
int waitpid(int pid, int status);

uint64_t sleep(uint64_t seconds);

pid_t getpid(void);
pid_t getppid(void);

void sleepshell(uint64_t ticks);

void ps();
// OPTIONAL: implement for ``on-disk r/w file system (+10 pts)''
off_t lseek(int fd, off_t offset, int whence);
int mkdir(const char *pathname, mode_t mode);

// OPTIONAL: implement for ``signals and pipes (+10 pts)''
int pipe(int pipefd[2]);

#endif

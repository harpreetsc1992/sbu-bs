#include <stdio.h>
#include <unistd.h>

int main (int argc, char **argv, char **envp)
{
  int a = 100;
  int i = 0;
  pid_t pid = fork();
  //printf("Problem");
  if (pid == 0) {
    while (1) {
      a++;
      if (a == 100000000) {
        // a = 0;
        //write(1, "Parent\n", 7);
        printf("Parent %d\n\n", a+i);
        i++;
        a = 0;
        i %= 10;
      }
    }
  } else {
    while (1) {
      a++;
      if (a == 100000000) {
        // a = 0;
        //write(1, "Child\n", 6);
        printf("Child %d\n\n", a+i);
        a = 0;
        //int *f = (int*)0xffffffff;
        //*f = 1;
        i++;
        i %= 10;
      }
    }
  }
  return 0;
}

#include <stdio.h>
#include <string.h>
#include <sys/process.h>
#include <unistd.h>
#include <sys/tarfs.h>
#include <stdlib.h>
#include <sys/timer.h>

//char path[100] = "bin/";
//char pwd[100] = "~";

char
*trim(
	  char *str
	 )
{
	char *str_end;

	if (*str == 0)
	{
		return str;
	}

	while (*str == ' ') 
	{
		str++;
	}

	// Trim spaces at the end
	str_end = str + strlen(str) - 1;
	while (str_end > str && *str_end == ' ')
	{
		str_end--;
	}

	*(str_end + 1) = '\0';

	return str;
}

int
get_file(
		 char *name, 
		 char *buf
		)
{
	char path[32];
	char *pwd = "bin/";
	strcpy(path, pwd);
	int path_len = strlen(path);
	
	if (*name == '\0')
	{
		goto null_path;
	}

	while (*name != '\0')
	{
		path[path_len++] = *name;
		name++;
	}
	path[path_len] = '\0';
	
	FILE* fd = open("bin/cat");//path + 1);
	if (fd) return 1;
		
//	if (fd != 0)
//	{
//		read(fd, buf, 512);
//		return 1;
//	}

null_path:
	return 0;
}

void
execute_command(
				char *cmd,
				char *arg,
				char bg_flag
			   )
{
	char *pwd = "/";
	char *path = "bin";
	if ((strncmp(cmd, "ls", 2)) == 0)
	{
		printf("%s\n", cmd);
		uint64_t dir_add = opendir(pwd + 1);
		read_dir(dir_add);
	}
	else if ((strncmp(cmd, "cat", 3)) == 0)
	{
		char *file = arg;
		printf ("Cat %s\n", cmd);
		int pid = fork();
		if (pid != 0)
		{
        	if (bg_flag != '&') 
			{
            	waitpid(pid, 0);
        	}
			else 
			{
				printf("Background Process\n");
            	return;
        	}

    	} 
		else 
		{
        	execvpe("bin/cat", &file, NULL);
        	exit(1);
		}
	}   

	else if ((strncmp(cmd, "ps", 2)) == 0)
	{
		printf ("PS%s\n", cmd);
		printf("\n");
		ps();
	}  
	else if ((strncmp(cmd, "sleep", 5)) == 0)
	{
		printf("\n");
		char *str = trim(cmd + 5);
		if (*str != '\0')
		{
			int ticks = stoi(str);
			sleepshell(ticks);
		}
	}
	else if ((strncmp(cmd, "echo", 4)) == 0)
	{
		printf("\n");
		char *str = trim(cmd + 4);
		if (*str != '\0' && strncmp(str, "PATH", 5) == 0)
		{
			printf("%s", path);
		}
		else if (*str != '\0')
		{
			printf("%s", str);
		}   
	}

	else if ((strncmp(cmd, "help", 4)) == 0)
	{
	    printf("\nSupported commands are: ls, cat, echo, sleep, ps, help");
	}

	else
	{
		printf("\nCommand not found.");
	}
}

int main(int argc, char *argv[], char *envp[]) 
{
	printf("sbush> ");
	char input[16];
	char bg_flag;
	while ((strncmp(input, "exit", 4)) != 0)
	{
//		scanf("%s", input);
		strcpy(input, "cat test");
		int len = strlen(input);
		bg_flag = input[len - 1];
		if ((strncmp(input, "sh ", 3)) == 0)
		{
//			printf("\n");
			char *str = trim(input + 2);
			char buf[16];
			int file_exists = get_file(str, buf);
			if (file_exists)
			{
				int i;
				char temp[10];
				int k =0;
				int first = 0;
				for (i = 0; i < 16; i++)
				{
					if (buf[i] == '\0')
					{
					    break;
					}
					else if (buf[i] == '\n')
					{
						temp[k] = '\0';
						k = 0;
						if (first == 0)
						{
							if (strcmp(trim(temp), "#!/bin/sh") != 0)
							{
								break;
							}
							else
							{
								first = 1;
							}
						}
						else
						{
							execute_command(trim(temp), "", bg_flag);
						}
					}
					else
					{
						temp[k++] = buf[i];
					}
				}
			}
		}
	    else
		{
			char *str = input;//trim(input + 2);
            char buf[16];

			int file_exists = get_file(str, buf);
			if(file_exists) execute_command("cat", "test", '\0');//bg_flag);    
		}
	}
    printf("\nExiting...");
    exit(-1);
    return 0;
}


#include <stdio.h>
#include <string.h>
#include <sys/process.h>
#include <unistd.h>
#include <sys/tarfs.h>
#include <stdlib.h>
#include <sys/timer.h>

char path[100] = "bin/";
char pwd[100] = "~";

void
cat(
	char *name
   )
{
	char str[100];
	strcpy(str, pwd);
	int len = strlen(str);
	FILE *fd;
	char buf[256];
	int j = 0;
	int bytes_read;

	for ( ; *name != '\0'; name++)
	{
		str[len++] = *name;
	}

	str[len] = '\0';

	printf("\n"); 
	fd = open(str + 1);
	printf("\n %s", fd->filename);

	bytes_read = read(fd, 256, buf);
	while (bytes_read-- != 0)
	{
		printf("%c", buf[j++]);
	}

	printf("\n");
}

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
	char path[100];
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
	
	FILE* fd = open(path + 1);
	if (fd != 0)
	{
		read(fd, 512, buf);
		return 1;
	}

null_path:
	return 0;
}

void
execute_command(
				char *cmd
			   )
{
/*
	if ((strncmp(cmd, "cd", 2)) == 0 && (strncmp(cmd, "cd ..", 5)) != 0)
	{ 
		char *directory = trim(cmd + 2);
		if (*directory == '\0' || (strcmp(directory, "/") == 0) || (strcmp(directory, "/") == 0) )
		{
			pwd[0] = '~';
			pwd[1] = '\0';
		}
		else
		{
			addToPWD(directory);
			if (opendir(pwd + 1) == 0)
			{
				cd_up(pwd + 1);
			}
		}
	}
*/
	if ((strncmp(cmd, "ls", 2)) == 0)
	{
		uint64_t dir_add = opendir(pwd + 1);
		read_dir(dir_add);
	}
/*
	else if ((strncmp(cmd, "cd ..", 5)) == 0)
	{
		if (!(strlen(pwd) == 1))
		{
			cd_up(pwd + 1);       
        }
	}
*/
	else if ((strncmp(cmd, "cat", 3)) == 0)
	{
		char *file = trim(cmd + 3);
		cat(file);
	}   
/*
	else if ((strncmp(input, "vi", 2)) == 0)
	{
		char *file = trim(cmd + 3);
		vi(file);
	} 
*/
	else if ((strncmp(cmd, "ps", 2)) == 0)
	{
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
/*
	else if((strncmp(cmd, "pwd", 4)) == 0)
	{
	    printf("\n%s", pwd);
	}
*/
	else if ((strncmp(cmd, "help", 4)) == 0)
	{
	    printf("\nSupported commands are: ls, cat, echo, sleep, ps, help");
	}
/*
	else if((strncmp(cmd, "shutdown", 4)) == 0)
	{
	    printf("\nShutdown");
	    shutdown();
	}
*/
	else
	{
		printf("\nCommand not found.");
	}
}

int main(int argc, char *argv[], char *envp[]) 
{
	puts("sbush> ");
	char input[100];
	while ((strncmp(input, "exit", 4)) != 0)
	{
//		scanf("%s", input); 
		if ((strncmp(input, "sh ", 3)) == 0)
		{
//			printf("\n");
			char *str = trim(input + 2);
			char buf[512];
			int file_exists = get_file(str, buf);
			if (file_exists)
			{
				int i;
				char temp[100];
				int k =0;
				int first = 0;
				for (i = 0; i < 512; i++)
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
							execute_command(trim(temp));
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
			execute_command(input);    
		}
	}
    printf("\nExiting...");
    exit(-1);
    return 0;
}

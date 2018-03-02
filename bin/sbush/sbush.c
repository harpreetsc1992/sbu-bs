#include <stdio.h>
#include <string.h>
#include <sys/process.h>
#include <unistd.h>
#include <sys/tarfs.h>
#include <stdlib.h>
#include <sys/timer.h>

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
	
	FILE* fd = open("bin/ls");//path + 1);
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
	if (pwd);
	if ((strncmp(cmd, "ls", 2)) == 0)
	{
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
        	execvpe("bin/ls", NULL, NULL);
        	exit(1);
		}	
	}
	else if ((strncmp(cmd, "cat", 3)) == 0)
	{
		char *file = malloc(16); 
		strcpy(file, arg);
		if (file[0] == '/')
		{
			file = file + 1;
		}
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
        	execvpe("bin/ps", NULL, NULL);
        	exit(1);
		}
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
//		char **text = malloc(10 * sizeof(char *));
		char *tmp = malloc(16);
		strcpy(tmp, arg); 
//		char tmp[16];
//		char *tmp1;
//		strcpy(tmp, arg);
/*		int count = 0, i = 0;
		int len[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		printf("In echo\n");
		while (*arg != '\0')
		{
			if ((*arg == ' ') && (*(arg + 1) != '\0')) 
			{
				count++;
			}
			else
			{
				len[count]++;
			}
			arg = arg + 1;
		}
		for (int se = 0; se <= count; se++)
		{
			text[se] = malloc(8);
		}
		while (i <= count)
		{
			tmp1 = text[i];
			while (*tmp == ' ')
			{
				tmp++;
			}
			while (len[i]--)
			{
				*tmp1++ = *tmp++;
			}
			*(tmp1) = '\0';
			printf("%s\n", text[i]);
			i++;
		}
*/		int pid = fork();
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
//			printf("%s\n", text[0]);
//			printf("%s\n", text[1]);
//			printf("%s\n", text[2]);
//			printf("%s\n", tmp);
        	execvpe("bin/echo", &tmp, NULL);
        	exit(1);
		}
	}
	else if ((strncmp(cmd, "kill", 4)) == 0)
	{
		char *flag = &cmd[5];

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
        	execvpe("bin/kill", &flag, NULL);
        	exit(1);
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
	char input[16];
	char bg_flag;
	while ((strncmp(input, "exit", 4)) != 0)
	{
//		scanf("%s", input);
		strcpy(input, "ps");
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
			if(file_exists) execute_command("cat", "/bin/test", '\0');//bg_flag);    
		}
	}
    printf("\nExiting...");
    exit(-1);
    return 0;
}


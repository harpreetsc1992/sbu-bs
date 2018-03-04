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
	char path[16];
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
	
	FILE* fd = open(path);//path + 1);
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
		char *file = malloc(16); 
		strcpy(file, arg);
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
        	execvpe("bin/ls", &file, NULL);
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
		char *str = malloc(8);
		strcpy(str, arg);	
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
	       	execvpe("bin/sleep", &str, NULL);
        	exit(1);
		}
	}
	else if ((strncmp(cmd, "echo", 4)) == 0)
	{
		char *tmp = malloc(16);
		strcpy(tmp, arg); 
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
	       	execvpe("bin/echo", &tmp, NULL);
        	exit(1);
		}
	}
	else if ((strncmp(cmd, "kill", 4)) == 0)
	{
		char *tmp = malloc(16);
		strcpy(tmp, arg); 

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
        	execvpe("bin/kill", &tmp, NULL);
        	exit(1);
		}	
	}
	else if ((strncmp(cmd, "clear", 4)) == 0)
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
        	execvpe("bin/clear", NULL, NULL);
        	exit(1);
		}	
	}	
	else if ((strncmp(cmd, "help", 4)) == 0)
	{
	    printf("ls, cat, echo, sleep, ps, kill, clear\n");
	}

	else
	{
		printf("\nCommand not found.");
	}
}

int main(int argc, char *argv[], char *envp[]) 
{
	char *input = malloc(32);
	char bg_flag;
	int bytes;
	while ((strncmp(input, "exit", 4)) != 0)
	{
		int i = 0;
		printf("sbush> ");
		bytes = read(STDIN, (char *) input, 0);
		
		if (input[0] == '\n' || input[0] == ' ' || input[0] == '\0' || bytes == 0) continue;	
		int len = strlen(input);
		bg_flag = input[len - 1];
		while (input[i] != ' ')
		{
			if (input[i] == '\0')
			{
				break;
			}
			i++;
		}
		char *str = malloc(6);
		strncpy(str, input, i);
		char *arg = malloc(16);
		strncpy(arg, (char *)(input + i + 1), len - i + 1);
		
        char buf[16];
		int file_exists = get_file(str, buf);
		if(file_exists) execute_command(str, arg, bg_flag);    
	}
    printf("\nExiting...");
    exit(-1);
    return 0;
}


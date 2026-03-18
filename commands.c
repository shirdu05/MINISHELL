#include "header.h"
char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
	"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
	"exit", "exec", "shopt", "caller", "true", "type", "hash", "jobs", "fg", "bg", "bind", "help", NULL};
struct stop arr[50];
//int stop_count=0;
char *get_command(char  *input_string)
{
	static char command[100];
	int i=0;
	while(input_string[i]!=' ' && input_string[i]!='\0')
	{
		command[i]=input_string[i];
		i++;
	}
	command[i]='\0';
	return command;
}

int check_command_type(char *command)
{
	for(int i=0;builtins[i]!=NULL;i++)
	{
 		if((strcmp(command,builtins[i])==0))
		{
			return BUILTIN;
		}
	}
	for(int i=0;ext_cmd[i]!=NULL;i++)
	{
		if(strcmp(command,ext_cmd[i])==0)
		{
			return EXTERNAL;
		}
	}
	return NO_COMMAND;
}

void extract_external_commands(char **external_commands)
{
	int fd=open("ext_cmds.txt",O_RDONLY);
	if(fd==-1)
	{
		printf("Failed to open ext commands file\n");
		return;
	}
	char ch;
	char command[50];
	int i=0,index=0;
	while(read(fd,&ch,1)!=0 && index<152)
	{
		if(ch!='\n')
		{
			command[i++]=ch;
		}
		else
		{
			command[i]='\0';
			external_commands[index]=malloc(strlen(command)+1);
			strcpy(external_commands[index], command);
			index++;
			i=0;
		}
	}
	external_commands[index]=NULL;
	close(fd);
}
void execute_internal_commands(char *input_string)
{
	int status;
	if(strcmp(input_string, "exit")==0)
	{
		exit(0);
	}
	else if(strcmp(input_string, "pwd")==0)
	{
		char path[50];
		getcwd(path,50);
		printf("%s\n",path);
	}
	else if(strncmp(input_string, "cd", 2)==0)
	{
		char path[50];
		if(chdir(input_string+3)==-1)
		{
			perror("cd");
			return;
		}
		getcwd(path,50);
		printf("%s\n",path);
	}
	else if(strcmp(input_string, "echo $$")==0)
	{
		printf("%d\n",getpid());
	}
	else if(strcmp(input_string, "echo $?")==0)
	{
		if(WIFEXITED(status))
		{
			printf("Exited with status %d\n",WEXITSTATUS(status));
		}
	}
	else if(strcmp(input_string, "echo $SHELL")==0)
	{
		printf("%s\n",getenv("SHELL")); 
	}
	else if(strcmp(input_string, "jobs")==0)
	{
		for(int i=0;i<3;i++)
		{
			printf("[%d] %s\n",i+1,arr[ind].name);
		}
	}
	else if(strcmp(input_string, "fg")==0)
	{
		kill(arr[ind-1].spid,SIGCONT);
		waitpid(arr[ind].spid,&status,WUNTRACED);
	}
	else if(strcmp(input_string, "bg")==0)
	{
		signal(SIGCHLD,handler);
		kill(arr[ind-1].spid,SIGCONT);
		ind--;
		if(SIGCHLD)
		{
			waitpid(-1,&status,WNOHANG);
		}
	}
}

void execute_external_commands(char *input_string)
{

	char *argv[50];
	int argc = 0;

	char *token = strtok(input_string, " ");
	while (token != NULL)
	{
		argv[argc++] = token;
		token = strtok(NULL, " ");
	}
	argv[argc] = NULL;

	int pipe_count = 0, status;
	int cmd[argc + 1];

	cmd[0] = 0;
	int ind = 1;
 
	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "|") == 0)
		{
			argv[i] = NULL;
			cmd[ind++] = i + 1;
			pipe_count++;
		}
	}

	int fd[2];
	int in_fd = 0;
	for (int i = 0; i <= pipe_count; i++)
	{
		pipe(fd);
		pid_t pid = fork();

		if (pid == 0)
		{
			dup2(in_fd, 0);
			if (i != pipe_count)
			dup2(fd[1], 1);

			close(fd[0]);
			close(fd[1]);

			execvp(argv[cmd[i]], argv + cmd[i]);
			perror("execvp");
			exit(1);
		}
		else
		{
			wait(&status);
			close(fd[1]);
			in_fd = fd[0];
		}
	}
}

#include "header.h"
extern char prompt[]; 
int pid;
extern char input_string[50];
int ind=0;
void scan_input(char *prompt,char *input_string)
{
	signal(SIGINT,handler);
/*extract_external_command(2d)*/
	while(1)
	{
		printf("%s",prompt);
		scanf("%[^\n]",input_string);
		signal(SIGTSTP,handler);
		getchar();
		if(strncmp(input_string,"PS1=",4)==0)
		{
			if(input_string[4]!=' ')
			{
				strcpy(prompt,input_string+4);
			}
			else
			{
				printf("Space encounter\n");
				printf("command not found\n");
				return;
			}
		}
		char *command=get_command(input_string);
		int type=check_command_type(command);
		if(type==BUILTIN)
		{
			/*intenal;*/
			execute_internal_commands(input_string);
		}
		else if(type==EXTERNAL)
		{
			//external;
			pid=fork();
			if(pid==-1)
			{
				perror("fork failed");
				return;
			}
			else if(pid==0)
			{
				//it should terminate and print the prompt
				signal(SIGINT,SIG_DFL);
				signal(SIGTSTP,SIG_DFL);
				execute_external_commands(input_string);
				exit(0);
			}
			else
			{
				int status;
				waitpid(pid,&status,WUNTRACED);
			}
		}
		else
		{
			printf("No commands found\n");
			/*No cmd*/
			return;
		}
	}
}
void handler(int sig)
{
	int status;
	if(sig==SIGINT)
	{
		//need to avoid print bcoz there will 2 prompts from parent 
		if(pid==0)
		{
		printf("%s",prompt);
		getchar();
		}
	}
	else if(sig==SIGTSTP)
	{
		if(pid==0)
		{
		printf("%s\n",prompt);
		}
		else
		{
			arr[ind].spid=pid;
			strcpy(arr[ind].name,input_string);
			ind++;	
		}
	}
	else if(sig==SIGCHLD)
	{
		waitpid(-1,&status,WNOHANG);
	}
}

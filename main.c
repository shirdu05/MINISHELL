#include "header.h"
char *ext_cmd[152];
char prompt[25]="minishell$: ";
char input_string[50];
int main()
{
	system("clear");
	 //prompt[25]="Minishell$: ";
	//char input_string[50];
/*for (int i = 0; i < 152; i++)
    ext_cmd[i] = NULL;*/

	extract_external_commands(ext_cmd);
	scan_input(prompt,input_string);
	//execute_external_commands(input_string);
	//extract_internal_commands(input_string);
}

/*
* @author Kenneth Peter Fernandes
*/
#include "cmdLine.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "unistd.h"

pid_t pid;
int ret;
int status;

/*
* Funtion to process the command after receiving the command in tokens
* @oaram tokens - Contains the shell commmand split into separate tokens
*/
void processCmd(char **tokens)
{

    if ((pid = fork()) == 0)
    { //Child process block
        status = 0;

        if (execvp(tokens[0], tokens) == -1)
        {
            fprintf(stderr, "Failure while performing exec.\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    { //Parent process block
        waitpid(pid, &status, 0);
    }
}

/*
* The entry point of the program
*/
int main()
{
    char **tokens;

    // Intializing the tokens array by allocating memory
    initCmdLine();
    while (1)
    {
        printf("mysh>");

        // Retrieving the command in terms of separate tokens
        tokens = getCmdLine();

        if (strcmp(tokens[0], "exit") == 0)
            break;
        // Invoking the function to process the shell command
        processCmd(tokens);
    }

    

    // Invoking the function to free up memory
    freeCmdLine();
    return 0;
}

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

void processCmd(char **token)
{

    
    if ((pid = fork()) == 0)
    {
        //Child process
        status = 0;

        if (execvp(token[0], token) == -1)
        {
            fprintf(stderr, "Failure while performing exec.\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        //Parent process
        waitpid(pid, &status, 0);
        
    }
}

int main()
{
    char **tokens;
    
    initCmdLine();
    while (1)
    {
        printf("mysh>");
        tokens = getCmdLine();
        if (strcmp(tokens[0], "exit") == 0)
            break;
        processCmd(tokens);
    }
    free(tokens);
    freeCmdLine();
    return 0;
}

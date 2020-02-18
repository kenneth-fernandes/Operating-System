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
#include "sys/stat.h"
#include "fcntl.h"

pid_t pid;
int ret;
int status;

/*
* Function to retrieve index of a particular character
* @param - tokens - Contains the array of command line statement tokens
* @return - Index of the character or else -1 if the character is not present
*/
int getCharacterIndex(char **tokens, char *ch)
{
    int index = 0;
    while (tokens[index] != NULL)
    {
        if (strcmp(tokens[index], ch) == 0)
        {
            return index;
        }
        index += 1;
    }
    return -1;
}
/*
* Funtion to process the command after receiving the command in tokens
* @param tokens - Contains the shell commmand split into separate tokens
*/
void processCmdLine(char **tokens)
{

    if ((pid = fork()) == 0)
    { //Child process block
        status = 0;
        int index, index1, index2, outputFileDescriptor, inputFileDescriptor;

        if ((index1 = getCharacterIndex(tokens, ">")) > 0 && (index2 = getCharacterIndex(tokens, "<")) > 0)
        { // Code block for Input / Output Redirection

            // Setting the ">"/"<" character to NULL in order for tokens to executed in execvp()
            tokens[index1] = NULL;
            tokens[index2] = NULL;

            // Initializing the input/output file descriptor
            inputFileDescriptor = open(tokens[index2 + 1], O_RDONLY);
            outputFileDescriptor = open(tokens[index1 + 1], O_CREAT | O_TRUNC | O_WRONLY);

            // Replacing the standard input/output with input/output file descriptor
            dup2(inputFileDescriptor, 0);
            dup2(outputFileDescriptor, 1);

            // Closing the unused input / output file descriptor
            close(inputFileDescriptor);
            close(outputFileDescriptor);
        }
        else if ((index = getCharacterIndex(tokens, ">")) > 0)
        { // Code block for Output Redirection

            // Setting the ">" character to NULL in order for tokens to executed in execvp()
            tokens[index] = NULL;

            // Initializing the output file descriptor
            outputFileDescriptor = open(tokens[index + 1], O_CREAT | O_TRUNC | O_WRONLY);

            // Replacing the standard output with output file descriptor
            dup2(outputFileDescriptor, 1);

            // Closing the unused output file descriptor
            close(outputFileDescriptor);
        }
        else if ((index = getCharacterIndex(tokens, "<")) > 0)
        { // Code for Input Redirection block

            // Setting the "<" character to NULL in order for tokens to executed in execvp()
            tokens[index] = NULL;

            // Initializing the input file descriptor
            inputFileDescriptor = open(tokens[index + 1], O_RDONLY);

            // Replacing the standard input with input file descriptor
            dup2(inputFileDescriptor, 0);

            // Closing the unused input file descriptor
            close(inputFileDescriptor);
        }

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
        printf("mysh > ");

        // Retrieving the command in terms of separate tokens
        tokens = getCmdLine();

        if (strcmp(tokens[0], "exit") == 0)
            break;
        // Invoking the function to process the shell command
        processCmdLine(tokens);
    }

    // Invoking the function to free up memory
    freeCmdLine();
    return 0;
}

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
#include <assert.h>

pid_t pid;
pid_t pid1;
int ret;
int status;

/**
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
/**
 * Function to get separated array tokens
 * @param - tokens -  Contains the array of command line statement tokens
 *        - &index - Pointer to index or position of the current command to be executed
 * @return - Subset of tokens array split after "|"
 */
char **getSperatedTokenArr(char **tokens, int *index)
{
    char **a = malloc(sizeof(tokens[0]) * 32);
    int i = 0;
    while (tokens[*index] != NULL)
    {
        if (strcmp(tokens[*index], "|") == 0)
        {
            break;
        }

        a[i] = tokens[*index];
        *index = *index + 1;
        i += 1;
    }

    a[i] = NULL;
    return a;
}

/**
 * Function to execute pipe functionality
 * @param - tokens - Contains the array of command line statement tokens
 *        - index - position of the current command to be executed
 *        - inputFileDescriptor - file descriptor
 */
void processPipeFunction(char **tokens, int index, int inputFileDescriptor)
{

    // Invoke the function to get temporary tokens array
    char **a = getSperatedTokenArr(tokens, &index);

    // Checking if there is any commands to be executed
    if (tokens[index] == NULL)
    { // Last commands left in the tokens parameter

        //Reading from inputFileDescriptor, and writing to STDOUT_FILENO
        if (inputFileDescriptor != STDIN_FILENO)
        {
            dup2(inputFileDescriptor, STDIN_FILENO);
            close(inputFileDescriptor);
        }
        execvp(a[0], a);
    }
    else
    {
        // File descriptors
        int fileDescriptors[2];

        // Creating the pipe passing in file descriptors
        pipe(fileDescriptors);
        int pid1;

        if ((pid1 = fork()) == 0)
        { // Child process block
            close(fileDescriptors[0]);

            // Reading from inputFileDescriptor
            if (inputFileDescriptor != STDIN_FILENO)
            {
                dup2(inputFileDescriptor, STDIN_FILENO);
                close(inputFileDescriptor);
            }

            // Writing to fileDescriptors[1]
            if (fileDescriptors[1] != STDOUT_FILENO)
            {
                dup2(fileDescriptors[1], STDOUT_FILENO);
                close(fileDescriptors[1]);
            }

            execvp(a[0], a);
            free(a);
        }
        else if (pid1 > 0)
        { // Parent process block

            // Closing the unused file descriptors
            close(fileDescriptors[1]);
            close(inputFileDescriptor);
            waitpid(pid1, &status, 0);
            // Recursively calling the pipe function to process further commands
            processPipeFunction(tokens, (index + 1), fileDescriptors[0]);
        }
        else
        {
            //fork() Error block
            fprintf(stderr, "Failure while performing fork()1.\n");
            exit(EXIT_FAILURE);
        }
    }
}

/**
* Funtion to process the command after receiving the command in tokens
* @param tokens - Contains the shell commmand split into separate tokens
*/
void processCmdLine(char **tokens)
{

    if ((pid = fork()) == 0)
    { //Child process block
        status = 0;
        int index, index1, index2, outputFileDescriptor, inputFileDescriptor;

        if ((index1 = getCharacterIndex(tokens, ">")) >= 0 && (index2 = getCharacterIndex(tokens, "<")) >= 0)
        { // Code block for Input / Output Redirection

            // Setting the ">"/"<" character to NULL in order for tokens to executed in execvp()
            tokens[index1] = NULL;
            tokens[index2] = NULL;

            // Initializing the input/output file descriptor
            inputFileDescriptor = open(tokens[index2 + 1], O_RDONLY);
            outputFileDescriptor = open(tokens[index1 + 1], O_CREAT | O_TRUNC | O_WRONLY);

            // Replacing the standard input/output with input/output file descriptor
            dup2(inputFileDescriptor, STDIN_FILENO);
            dup2(outputFileDescriptor, STDOUT_FILENO);

            // Closing the unused input / output file descriptor
            close(inputFileDescriptor);
            close(outputFileDescriptor);
        }
        else if ((index = getCharacterIndex(tokens, ">")) >= 0)
        { // Code block for Output Redirection

            // Setting the ">" character to NULL in order for tokens to executed in execvp()
            tokens[index] = NULL;

            // Initializing the output file descriptor
            outputFileDescriptor = open(tokens[index + 1], O_CREAT | O_TRUNC | O_WRONLY);

            // Replacing the standard output with output file descriptor
            dup2(outputFileDescriptor, STDOUT_FILENO);

            // Closing the unused output file descriptor
            close(outputFileDescriptor);
        }
        else if ((index = getCharacterIndex(tokens, "<")) >= 0)
        { // Code for Input Redirection block

            // Setting the "<" character to NULL in order for tokens to executed in execvp()
            tokens[index] = NULL;

            // Initializing the input file descriptor
            inputFileDescriptor = open(tokens[index + 1], O_RDONLY);

            // Replacing the standard input with input file descriptor
            dup2(inputFileDescriptor, STDIN_FILENO);

            // Closing the unused input file descriptor
            close(inputFileDescriptor);
        }
        else if ((index = getCharacterIndex(tokens, "|")) >= 0)
        {
            processPipeFunction(tokens, 0, STDIN_FILENO);
            return;
        }

        if (execvp(tokens[0], tokens) == -1)
        {
            fprintf(stderr, "Failure while performing exec.\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (pid > 0)
    { //Parent process block
        waitpid(pid, &status, 0);
    }

    else
    {
        //fork() Error block
        fprintf(stderr, "Failure while performing fork().\n");
        exit(EXIT_FAILURE);
    }
}

/**
* The entry point of the program
*/
int main()
{
    char **tokens;

    // Intializing the tokens array by allocating memory
    initCmdLine();
    while (1)
    {
        printf("\nmysh > ");

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

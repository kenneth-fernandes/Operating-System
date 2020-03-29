#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 256
#define PROCESS_ARR_LENGTH 256
char *outputString;

int main()
{
    int readingFileStatus = 1, fileDescriptor, processCount = 1;

    printf("\nUser Program: Starting the user program to test the device.\n");

    fileDescriptor = open("/dev/process_lst", O_RDWR);

    if (fileDescriptor < 0)
    {
        perror("\nUser Program: Failed to open the device.\n");
        return errno;
    }

    printf("\nUser Program: Reading from the device.\n");
    printf("\n===============================================================================================================\n");
    while (readingFileStatus != 0)
    {
        outputString = malloc(sizeof(char *) * BUFFER_LENGTH);
        readingFileStatus = read(fileDescriptor, outputString, (sizeof(char *) * BUFFER_LENGTH));
        if (readingFileStatus < 0)
        {
            perror("\nUser Program: Failed to read the message from the device.");
            return errno;
        }
        if (readingFileStatus != 0)
            printf("\nUser Program: %s", outputString);
        free(outputString);
    }
    printf("\n===============================================================================================================\n");

    close(fileDescriptor);

    printf("\nUser Program: End of the program\n");
    return 0;
}
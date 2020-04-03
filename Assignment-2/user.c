#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 256
#define PROCESS_ARR_LENGTH 256
char *output_string;

int main()
{
    int bytes_read = 1, file_descriptor, process_count = 1;
    output_string = malloc(sizeof(char *) * BUFFER_LENGTH);

    printf("\nUser Program: Starting the user program to test the device.\n");

    file_descriptor = open("/dev/process_lst", O_RDWR);

    if (file_descriptor < 0)
    {
        perror("\nUser Program: Failed to open the device.\n");
        return errno;
    }

    printf("\nUser Program: Reading from the device.\n");
    printf("\n===============================================================================================================\n");
    while (bytes_read != 0)
    {
        memset(output_string, 0, sizeof(char *) * BUFFER_LENGTH);
        bytes_read = read(file_descriptor, output_string, (sizeof(char *) * BUFFER_LENGTH));
        if (bytes_read < 0)
        {
            perror("\nUser Program: Failed to read the message from the device.\n");
            free(output_string);
            return errno;
        }
        if (bytes_read != 0) {
            printf("\nUser Program: %s\n", output_string);
            printf("\nUser Program: Bytes Read - %d\n", bytes_read);
        }

    }
    printf("\n===============================================================================================================\n");
    free(output_string);
    close(file_descriptor);

    printf("\nUser Program: End of the program\n");
    return 0;
}
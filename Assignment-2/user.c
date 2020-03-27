#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 256
#define PROCESS_ARR_LENGTH 256
static char receive[BUFFER_LENGTH];

int main()
{
    int ret, fileDescriptor;
    
    printf("Starting device test code example...\n");
    
    fileDescriptor = open("/dev/process_lst", O_RDWR);
    printf("1111Starting device test code example...\n");
    if (fileDescriptor < 0)
    {
        perror("Failed to open the device!!");
        return errno;
    }

    printf("Reading from the device...\n");
    for (int i = 0; i <= PROCESS_ARR_LENGTH; i += 1)
    {

        ret = read(fileDescriptor, receive, BUFFER_LENGTH);
        if (ret < 0)
        {
            perror("Failed to read the message from the device.");
            close(fileDescriptor);
            return errno;
        }
        if (strcmp(receive, "exit") == 0)
        {
            break;
        }
        printf("%s", receive);
    }
    close(fileDescriptor);

    printf("End of the program\n");
    return 0;
}
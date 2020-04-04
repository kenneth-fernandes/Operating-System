# Operating System - Assignment 2

# Name:
- Kenneth Peter Fernandes
- <kferna11@binghamton.edu>
- B00813852

# Description:
- To implement Character Device Driver to list processes using Kernel programming.

- User space program for a single file descriptor [IMPLEMENTED].

- User space program for a multiple file descriptor [NOT-IMPLEMENTED].

# Plan of action (as on 03/16/2020):
- Currently, I am looking at various online resources that would help me understand kernel programming.

- I have added two files (Note: Partial code has been written):
    
    > dev_ps.c - For creating dev/process_list character device that returns the information for all currently running processes.

    > user.c - For opening the character device and printing the list of processes retrieved from the character device.

- Most of the time would be spent in understanding basic kernel programming, after which I would be able to move ahead with the feature implementation.

- After understanding the basic concepts of Kernel Programming, I will continue with developing the first part of the assignment of creating the character device, following which I will work on the development of the next part of the assignment.

- As of now, I plan to complete the assignment by the given due date : 03/27/2020

# Execution of plan (as on 04/03/2020):
- The first job I did is understanding the kernel module and how the it is installed and used by any user program.

- Started off by writing a simple code to just run the driver.

- Once I understood of how the device driver works, my next task was to create the structure of my device driver and the file operation functions.

- But due to certain "challenges" that I faced, it lead to delay in submission.

- Apologies for the delay.

# Implementation (Working of program):
- The dev_ps program has open(), read() and close() file-operations functions.

- The read() function of the dev_ps program appears to loop through the process but I actually read it one at a time but storing the next process in a temporary storage using the next_task(process) macro.

- Earlier I had created a char** pointer variable to store all the process details, but it might take more space so I implemented using a char* pointer variable buffer.

- So with each read from my user program, each process details with the number of bytes read is sent and printed on the kernel and user space console.

- Once there are no processes remaining, the read function sends the number of bytes read as zero which terminates the user program.


# Challenges faced:
- The reason for delaying the assignment is by my computer system crashed, which is right now with the service center. Luckily, after couple of days, I was able to work on my friends spare laptop.

- As the code written was on my local system, I had to write the program again as I had not uploaded to the online repository.

- Also, I was facing challenges with segmentation fault occuring at the kernel leading to restarting my system.

- Due to incorrect kernel memory allocation, the device driver was in use and only way was to restart the system.

- Once I resloved this issue, it was too late for me to submit the program. 

- Also, multiple submissions were around the earlier deadline which lead to the delay as I had to set up the computer system in order to work on all my assignments.

# Compling the program:
- To compile the dev_ps.c kernel module code
 > sudo make 

- To insert the module to kernel
 > sudo insmod dev_ps.ko 

- To build the user program user.c
 > sudo make build

# Executing the program:
 > sudo make run

# Cleaning the executable:
- To clean all executables
 > sudo make clean

- To degregister the kernel module
 > sudo rmmod dev_ps

# Declaration:
- The program has been tested and works fine on csvb machine.

- There was issue with the access_ok() function on my local machine and it does not compile.

- Also, the access_ok(VERIFY_WRITE, user_space_buffer, length) works fine on csvb machine but not on my local machine as the arguments it takes 2 rather than 3.

# Date: 04/03/2020

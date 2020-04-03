# Operating System - Assignment 2

# Name:
- Kenneth Peter Fernandes
- <kferna11@binghamton.edu>
- B00813852

# Description:
- To implement Character Device Driver to list processes using Kernel programming.

# Plan of action (as on 03/16/2020):
- Currently, I am looking at various online resources that would help me understand kernel programming.

- I have added two files (Note: Partial code has been written):
    
    > dev_ps.c - For creating dev/process_list character device that returns the information for all currently running processes.

    > user.c - For opening the character device and printing the list of processes retrieved from the character device.

- Most of the time would be spent in understanding basic kernel programming, after which I would be able to move ahead with the feature implementation.

- After understanding the basic concepts of Kernel Programming, I will continue with developing the first part of the assignment of creating the character device, following which I will work on the development of the next part of the assignment.

- As of now, I plan to complete the assignment by the given due date : 03/27/2020

# Execution of the plan (as on 04/03/2020):
- I
# Challenges faced:
- The reason for delaying the assignment is by my computer system crashed, which is right now with the service center.
- As the code written was on my local system, I had to write the program again as I had not uploaded to the online repository.
- Also, I was facing challenges with segmentation fault occuring at the kernel leading to restarting my system.


# Compling the program:
- To compile the dev_ps.c kernel module code
 > make 

- To insert the module to kernel
 > sudo insmod dev_ps.ko 

- To build the user program user.c
 > make build

# Executing the program:
- make run

# Cleaning the executable:
- To clean all executables
 > make clean

- To degregister the kernel module
 > sudo rmmod dev_ps

# Date: 04/03/2020

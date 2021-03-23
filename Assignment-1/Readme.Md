# Operating System - Assignment 1

# Name: Kenneth Peter Fernandes


# Description:
- The program basically works as a basic shell of UNIX which is written in C.

- The features that have been implemented are:
    > Input Redirection: [Tested on CSVB]
    eg. grep text < input.txt

    > Output Redirection: [Tested on CSVB]
    eg. ls > output.txt

    > Pipes: [Except I/O Redirection, rest functionality Tested on CSVB]
    eg. cat input.txt | grep text | wc -w

- Code has been tested on the CSVB server.

- Note: 
    > Input/Output Redirection in combination with pipes has not been tested. 
    > Some commands such as cd, echo "$USER", etc. would not work in this program.

# Compling the program:
- make build

# Executing the program:
- make run or ./mysh

# Cleaning the executable:
- make clean

# Date: 02/28/2020

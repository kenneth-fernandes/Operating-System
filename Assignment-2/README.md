# Operating System - Assignment 2

# Name: Kenneth Peter Fernandes

# Description:
- To implement kernel Kprobe mechanism for :

    - list out prints the virtual addresses that cause page faults to the system log.

    - Plot tabular scatter-plot for recording page faults occuring at page virtual address v/s the time of its occurrenc.

# Compling the program:
- To compile the dev_ps.c kernel module code
 > sudo make 

- To insert the modules to kernel
 > sudo insmod pf_probe_A.ko 
 > sudo insmod pf_probe_B.ko u_pid=<Process_ID>

# Executing the program:
 > sudo make run

# De-registering:
- To degregister the kernel module
 > sudo rmmod pf_probe_A
 > sudo rmmod pf_probe_B

# View Output:
> dmesg | tail -300

# Date: 04/03/2020

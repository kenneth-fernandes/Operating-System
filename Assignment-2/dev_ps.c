/* Required linux module and kernel header file */
#include <linux/module.h>
#include <linux/kernel.h>

/* File operations like open/close and read/ write to device*/
#include <linux/fs.h>

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/string.h>

/* Module information register */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kenneth Fernandes");
MODULE_DESCRIPTION("Character device for listing current active processes.");
MODULE_VERSION("1.0");

// Buffer size constant
#define BUFFER_SIZE 512

// Stroes the device regsitration status
static int device_reg_status;

static struct task_struct *temp_process;
static char *result_buffer;

/* Function declaration for character device open() file-operation */
static int device_open(struct inode *, struct file *);
/* Function declaration for character device read() file-operation */
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
/* Function declaration for character device close() file-operation */
static int device_close(struct inode *, struct file *);

/* Function declaration to get the status based on the value of type long from process->status */
static char *getProcessStatus(long);

/* Struct of file-operations for defining the character device */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .read = device_read,
    .release = device_close};

/* Struct of miscdevice -  character device */
static struct miscdevice process_lst_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "process_lst",
    .fops = &fops};

/* Function implementation for open() function of character device */
static int device_open(struct inode *inodep, struct file *filep)
{
    // Allocating memory for storing the process details
    result_buffer = kmalloc(sizeof(char *) * BUFFER_SIZE, GFP_KERNEL);
    printk(KERN_INFO "\nProcess_Lst_Device: %s", "Device has been opened.");

    // Storing the process after the init_task process mentioned into a temporay storage
    temp_process = next_task(&init_task);
    return 0;
}

/* Function implementation for read() function of character device */
static ssize_t device_read(struct file *file, char *user_space_buffer, size_t length, loff_t *offset)
{
    int result_buffr_content_len = 0;
    int cpy_to_usr_status;
    struct task_struct *process;
    printk(KERN_INFO "\nProcess_Lst_Device: %s", "Reading the Process list device.");

    // Iterating through each process or task
    for_each_process(process)
    {
        // Checking if the current process is pointing to the correct process stored tempProcess using next_task()
        if (process == temp_process)
        {
            // Intializng the buffer to 0 and writing the process details to the buffer
            memset(result_buffer, 0, sizeof(char *) * BUFFER_SIZE);
            sprintf(result_buffer, "PROCESS = %s | PID = %d | PPID = %d | CPU = %d | STATE = %s",
                    process->comm, process->pid, process->real_parent->pid, task_cpu(process), getProcessStatus(process->state));
            printk(KERN_INFO "\nProcess_Lst_Device:  %s - %s", "Process read", result_buffer);

            // Verifying whether the user space pointer is valid
            if (access_ok(VERIFY_WRITE, user_space_buffer, strlen(user_space_buffer)))
            {
                // If user space pointer is valid, then perfoming the copy_to_user
                cpy_to_usr_status = copy_to_user(user_space_buffer, result_buffer, strlen(result_buffer));
                if (cpy_to_usr_status != 0)
                { // Failure to copy kernel buffer contents to user space buffer.
                    printk(KERN_INFO "\nProcess_Lst_Device:  %s", "Failed to copy kernel buffer contents to user space buffer.");
                    return -EFAULT;
                }
            }
            else
            {
                printk(KERN_INFO "\nProcess_Lst_Device:  %s", "Failed to verify the user space.");
                return -1;
            }
            result_buffr_content_len = strlen(result_buffer);
            // Storing the next process into a temporay storage
            temp_process = next_task(process);
            break;
        }
    }
    return result_buffr_content_len;
}

/* Function implementation for close() function of character device */
static int device_close(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "\nProcess_Lst_Device: %s", "Closing the device.");

    // Storing the init_task process pointer into the temporay storage
    temp_process = &init_task;

    // De-allocatin the memory
    kfree(result_buffer);
    return 0;
}

/* Defining the function that is called when module is installed */
int __init init_module()
{
    // Registering the character device
    device_reg_status = misc_register(&process_lst_device);

    if (device_reg_status < 0)
    { // Device registration failure block
        printk(KERN_ALERT "\nProcess_Lst_Device: %s", "Failed to register the device.");
        return device_reg_status;
    }
    printk(KERN_INFO "\nProcess_Lst_Device: %s", "Registered device correctly.");

    return 0;
}

/* Defining the function that is called when module is un-installed */
void __exit cleanup_module()
{
    // De-registering the character device
    misc_deregister(&process_lst_device);
    printk(KERN_INFO "\nProcess_Lst_Device: %s", "De-registering the device.");
}

/* Function implementation to get the status based on the value of type long */
static char *getProcessStatus(long processState)
{
    switch (processState)
    {
        //0
    case TASK_RUNNING:
        return "TASK_RUNNING";
        //1
    case TASK_INTERRUPTIBLE:
        return "TASK_INTERRUPTIBLE";
        //2
    case TASK_UNINTERRUPTIBLE:
        return "TASK_UNINTERRUPTIBLE";
        //4
    case __TASK_STOPPED:
        return "__TASK_STOPPED";
        //8
    case __TASK_TRACED:
        return "__TASK_TRACED";
        //16
    case EXIT_DEAD:
        return "EXIT_DEAD";
        //32
    case EXIT_ZOMBIE:
        return "EXIT_ZOMBIE";
        //48
    case EXIT_TRACE:
        return "EXIT_ZOMBIE, EXIT_DEAD";
        //64
    case TASK_PARKED:
        return "TASK_PARKED";
        //128
    case TASK_DEAD:
        return "TASK_DEAD";
        //256
    case TASK_WAKEKILL:
        return "TASK_WAKEKILL";
        //512
    case TASK_WAKING:
        return "TASK_WAKING";
        //1024
    case TASK_NOLOAD:
        return "TASK_NOLOAD";
        //2048
    case TASK_NEW:
        return "TASK_NEW";
        //4096
    case TASK_STATE_MAX:
        return "TASK_STATE_MAX";
        //258
    case TASK_KILLABLE:
        return "TASK_WAKEKILL, TASK_UNINTERRUPTIBLE";
        //260
    case TASK_STOPPED:
        return "TASK_WAKEKILL, __TASK_STOPPED";
        //264
    case TASK_TRACED:
        return "TASK_WAKEKILL, __TASK_TRACED";
        //1026
    case TASK_IDLE:
        return "TASK_UNINTERRUPTIBLE, TASK_NOLOAD";
        //3
    case TASK_NORMAL:
        return "TASK_INTERRUPTIBLE, TASK_UNINTERRUPTIBLE";
        //127
    case TASK_REPORT:
        return "TASK_RUNNING, TASK_INTERRUPTIBLE, TASK_UNINTERRUPTIBLE, __TASK_STOPPED, __TASK_TRACED, EXIT_DEAD, EXIT_ZOMBIE, TASK_PARKED";
    default:
        return "INVALID";
    }
}

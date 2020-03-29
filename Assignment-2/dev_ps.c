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

MODULE_LICENSE("GPL");                                                        ///< The license type -- this affects available functionality
MODULE_AUTHOR("Kenneth Fernandes");                                           ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Character device for listing current active processes."); ///< The description -- see modinfo
MODULE_VERSION("1.0");                                                        ///< A version number to inform users

/* Definition of the structure task_struct FOR for processes/tasks in sched.h  */
#define BUFFER_SIZE 512
static int deviceRegterdStatus;
static struct task_struct *tempProcess;

// The prototype functions for the character driver -- must come before the struct definition
static int device_open(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static int device_close(struct inode *, struct file *);
static char *getProcessStatus(long);

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .read = device_read,
    .release = device_close};

static struct miscdevice process_lst_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "process_lst",
    .fops = &fops};

static int device_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "\nProcess_Lst_Device: Device has been opened.");
    tempProcess = next_task(&init_task);
    return 0;
}
static ssize_t device_read(struct file *file, char *userOutputBuffer, size_t length, loff_t *offset)
{
    int resBufferContentLen = 0;
    struct task_struct *process;
    printk(KERN_INFO "\nProcess_Lst_Device:  %s", "Reading the Process list device.");

    for_each_process(process)
    {
        if (process == tempProcess)
        {
            char *resultBuffer = kmalloc(sizeof(char *) * BUFFER_SIZE, GFP_KERNEL);
            int cpyToUsrStatus;
            sprintf(resultBuffer, "PROCESS = %s | PID = %d | PPID = %d | CPU = %d | STATE = %s",
                    process->comm, process->pid, process->real_parent->pid, task_cpu(process), getProcessStatus(process->state));
            printk(KERN_INFO "\nProcess_Lst_Device:  Process read - %s", resultBuffer);
            cpyToUsrStatus = copy_to_user(userOutputBuffer, resultBuffer, strlen(resultBuffer));
            if (cpyToUsrStatus != 0)
            {
                printk(KERN_INFO "\nProcess_Lst_Device:  %s", "\nFailed to copy buffer contents from kernel to user.");
                kfree(resultBuffer);
                return -EFAULT;
            }

            resBufferContentLen = strlen(resultBuffer);
            kfree(resultBuffer);
            tempProcess = next_task(process);
            break;
        }
    }
    return resBufferContentLen;
}
static int device_close(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "\nProcess_Lst_Device: Closing the device.");
    tempProcess = &init_task;
    return 0;
}

/* Defining the function that is called when module is installed */
int __init init_module()
{
    deviceRegterdStatus = misc_register(&process_lst_device);

    if (deviceRegterdStatus < 0)
    {
        printk(KERN_ALERT "\nProcess_Lst_Device: Failed to register the device.\n");
        return deviceRegterdStatus;
    }
    printk(KERN_INFO "\nProcess_Lst_Device: Registered device correctly.");
    tempProcess = next_task(&init_task);

    /*0 1 2 4 8 16 32 48 64 128 256 512 1024 2048 4096 258 260 264 1026 3 127
    printk("\n%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d ", TASK_RUNNING, TASK_INTERRUPTIBLE, TASK_UNINTERRUPTIBLE,
           __TASK_STOPPED, __TASK_TRACED, EXIT_DEAD, EXIT_ZOMBIE, EXIT_TRACE, TASK_PARKED, TASK_DEAD, TASK_WAKEKILL, TASK_WAKING,
           TASK_NOLOAD, TASK_NEW, TASK_STATE_MAX, TASK_KILLABLE, TASK_STOPPED, TASK_TRACED, TASK_IDLE, TASK_NORMAL, TASK_REPORT);*/

    return 0;
}

/* Defining the function that is called when module is un-installed */
void __exit cleanup_module()
{
    misc_deregister(&process_lst_device);
    printk(KERN_INFO "\nProcess_Lst_Device: %s", "De-registering the device.");
}

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
        return "TASK_RUNNING, TASK_INTERRUPTIBLE, TASK_UNINTERRUPTIBLE, __TASK_STOPPED,  __TASK_TRACED, EXIT_DEAD, EXIT_ZOMBIE, TASK_PARKED";
    default:
        return "INVALID";
    }
}

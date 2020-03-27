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

/* Definition of the structure task_struct FOR for processes/tasks in sched.h  */
struct task_struct *process;
#define CHAR_BUFFER_SIZE 256
static int majorNumber;
static int resultBuffrIndex = 0;
static int readBuffrIndex = 0;
static char **resultBufferArr;

// The prototype functions for the character driver -- must come before the struct definition
static int device_open(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .read = device_read};

static struct miscdevice process_lst_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "process_lst",
    .fops = &fops};

static ssize_t device_read(struct file *file, char *out, size_t len, loff_t *offset)
{
    printk(KERN_INFO "%s", "\n Loading the Character Device for listing processes. \n");
    if (readBuffrIndex <= resultBuffrIndex)
    {
        copy_to_user(out, resultBufferArr[readBuffrIndex], strlen(resultBufferArr[readBuffrIndex]) + 1);
        readBuffrIndex += 1;
    }
    return 0;
}

static int device_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "Process_Lst_Device: Device has been opened");

    resultBufferArr = kmalloc((sizeof(char *) * CHAR_BUFFER_SIZE), GFP_KERNEL);

    for_each_process(process)
    {
        sprintf(resultBufferArr[resultBuffrIndex], "\n PROCESS = %s | PID = %d | PPID = %d | CPU = %d | STATE = %ld ",
                process->comm, process->pid, process->real_parent->pid, task_cpu(process), process->state);
        resultBuffrIndex += 1;
        if (resultBuffrIndex == CHAR_BUFFER_SIZE - 2)
        {
            break;
        }
        /*printk(KERN_INFO "\n PROCESS = %s | PID = %d | PPID = %d | CPU = %d | STATE = %ld ",
               process->comm, process->pid, process->real_parent->pid, task_cpu(process), process->state);*/
    }
    sprintf(resultBufferArr[resultBuffrIndex], "exit");

    return 0;
}

/* Defining the function that is called when module is installed */
int __init init_module()
{
    majorNumber = misc_register(&process_lst_device);

    if (majorNumber < 0)
    {
        printk(KERN_ALERT "Process_Lst_Device: Failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "Process_Lst_Device: Registered correctly with major number %d\n", majorNumber);

    return 0;
}

/* Defining the function that is called when module is un-installed */
void __exit cleanup_module()
{
    misc_deregister(&process_lst_device);
    printk(KERN_INFO "%s", "\n Un-installing the Character Device for listing processes. \n");
    kfree(resultBufferArr);
}

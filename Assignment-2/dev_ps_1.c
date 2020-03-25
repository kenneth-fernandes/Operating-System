/* Required linux module and kernel header file */
#include <linux/module.h>
#include <linux/kernel.h>

/* File operations like open/close and read/ write to device*/
#include <linux/fs.h>

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

/* Definition of the structure task_struct FOR for processes/tasks in sched.h  */
struct task_struct *process;

// The prototype functions for the character driver -- must come before the struct definition
static int dev_open(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .read = device_read};

static struct miscdevice process_lst_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "Process List Device",
    .fops = &fops};

/* Defining the function that is called when module is installed */
int __init init_module()
{
    printk(KERN_INFO "%s", "\n Loading the Character Device for listing processes. \n");

    for_each_process(process)
    {
        printk(KERN_INFO "\n PROCESS = %s | PID = %d | PPID = %d | CPU = %d | STATE = %ld ",
               process->comm, process->pid, process->real_parent->pid, task_cpu(process), process->state);
    }

    return 0;
}

/* Defining the function that is called when module is un-installed */
void __exit cleanup_module()
{
    printk(KERN_INFO "%s", "\n Un-installing the Character Device for listing processes. \n");
}

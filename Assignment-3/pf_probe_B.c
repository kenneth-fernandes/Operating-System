#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/time.h>

#define MAX_SYMBOL_LEN 64
#define MAX_TIME_COUNT 70
#define MAX_ADDRESS_COUNT 30

static char symbol[MAX_SYMBOL_LEN] = "__handle_mm_fault";
static int u_pid = 0;
static int addr_count = 0;
static ktime_t start_time;
static ktime_t end_time;
static ktime_t sample_time;
static ktime_t *ktime_ptr;
static ktime_t *plot_time_ptr;
static unsigned long int *addr_ptr;

module_param_string(symbol, symbol, sizeof(symbol), S_IRUGO);
module_param(u_pid, int, S_IRUGO);

/*Funtion declarations for processing the plotiing of scatter plot table*/
static void set_time_scalaing_parameters(void);
static void set_time_plot_arr(void);
static void print_address_time_plotting(void);
static void free_memory(void);
static void print_fault_address_to_address_label_mapping(void);
static void print_time_to_time_label_mapping(void);

static struct kprobe kp = {
    .symbol_name = symbol,
};

/*
 Kprobres pre-handler function where all the process's page fault information is retrieved
*/
static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
// Code for finding out process page fault address for X86
#ifdef CONFIG_X86

    if (current->pid == u_pid)
    {
        // Storing data for only 30 page faults observations
        if (addr_count < MAX_ADDRESS_COUNT)
        {

            ktime_ptr[addr_count] = ktime_get();
            addr_ptr[addr_count] = regs->si;
            addr_count += 1;
        }
    }

#endif

    return 0;
}

/* Function implementation for initializing the start, end times for the page faults and finding 
out the sample time for scalling time for plotting the scatter plot */
static void set_time_scalaing_parameters(void)
{
    start_time = ktime_ptr[0] - ktime_ptr[0];
    end_time = ktime_sub(ktime_ptr[addr_count - 1], ktime_ptr[0]);
    sample_time = end_time / MAX_TIME_COUNT;
}

/* Function implementation for initializing the scalled time from the time when page faults had occured*/
static void set_time_plot_arr(void)
{
    int i;
    plot_time_ptr[0] = start_time;
    for (i = 1; i < MAX_TIME_COUNT - 1; i += 1)
    {
        plot_time_ptr[i] = plot_time_ptr[i - 1] + sample_time;
    }
    plot_time_ptr[MAX_TIME_COUNT - 1] = end_time;
}

/* Function implementation for printing the scatter plot in tabular form */
static void print_address_time_plotting(void)
{
    int i, j;
    printk(KERN_INFO "\n");
    printk(KERN_INFO "\n=================== SCATTER PLOT (MEMORY ADDRESS V/S TIME) ========================\n\n");
    // Code for printing the Address labels for plotting table
    printk(KERN_CONT "| Addr |");
    for (i = 0; i < addr_count; i += 1)
    {
        if (i < 10)
        {
            printk(KERN_CONT " 0%d |", i);
        }
        else
        {
            printk(KERN_CONT " %d |", i);
        }
    }
    printk(KERN_CONT "\n");
    // Code for plotting the points on the graph
    for (i = 0; i < MAX_TIME_COUNT; i += 1)
    {
        // Code for printing the Time label for plotting table
        if (i < 10)
        {
            printk(KERN_CONT "| T0%d  |", i);
        }
        else
        {
            printk(KERN_CONT "| T%d  |", i);
        }
        // Code for finding a match page fault occured at a particular time with the sample time
        for (j = 0; j < addr_count; j += 1)
        {
            ktime_t ktime = ktime_ptr[j] - ktime_ptr[0];

            if (ktime == plot_time_ptr[i])
            {
                printk(KERN_CONT "  * |");
            }
            else if (ktime > plot_time_ptr[i - 1] && ktime <= plot_time_ptr[i])
            {
                printk(KERN_CONT "  * |");
            }
            else
            {
                printk(KERN_CONT "    |");
            }
        }
        printk(KERN_CONT "\n");
    }
}

static void free_memory(void)
{
    kfree(ktime_ptr);
    kfree(plot_time_ptr);
    kfree(addr_ptr);
    addr_count = 0;
}

/* Function implementation for printing table for page fault virtual address to address label mapping for the scatter plot */
static void print_fault_address_to_address_label_mapping(void)
{
    int i;
    printk(KERN_INFO "\n");
    printk(KERN_INFO "\n=================== ADDRESS LABEL TO PAGE FAULT VIRTUAL ADDRESS MAPPING TABLE ========================\n\n");
    printk(KERN_INFO "|\t%s\t|\t\t%s\t\t|\t\t%s\t|\t%s\t|", "FAULT ADDRESS LABEL", "FAULT ADDRESS", "KERNEL TIME (ns)", "KERNEL TIME SCALLED (ns)");
    for (i = 0; i < addr_count; i += 1)
    {
        printk(KERN_CONT "\n");
        if (i < 10)
        {
            printk(KERN_CONT "|\t\t0%d\t\t|\t\t%lu\t\t|\t\t%lld\t\t|\t\t%lld\t\t\t|", i, addr_ptr[i], ktime_to_ns(ktime_ptr[i]),
                   ktime_to_ns(ktime_ptr[i] - ktime_ptr[0]));
        }
        else
        {
            printk(KERN_CONT "|\t\t%d\t\t|\t\t%lu\t\t|\t\t%lld\t\t|\t\t%lld\t\t\t|", i, addr_ptr[i], ktime_to_ns(ktime_ptr[i]),
                   ktime_to_ns(ktime_ptr[i] - ktime_ptr[0]));
        }
    }
}

/* Function implementation fo printing table for scalled time to time label mapping for the scatter plot */
static void print_time_to_time_label_mapping(void)
{
    int i;
    printk(KERN_INFO "\n");
    printk(KERN_INFO "\n=================== KERNEL TIME LABEL TO KERNEL SAMPLE TIME TABLE ========================\n\n");
    printk(KERN_INFO "|\t%s\t|\t\t%s\t|", "KERNEL TIME LABEL", "KERNEL TIME (ns)");
    for (i = 0; i < MAX_TIME_COUNT; i += 1)
    {
        printk(KERN_CONT "\n");
        if (i < 10)
        {
            printk(KERN_CONT "|\t\tT0%d\t\t|\t\t%lld\t\t\t|", i, ktime_to_ns(plot_time_ptr[i]));
        }
        else
        {
            printk(KERN_CONT "|\t\tT%d\t\t|\t\t%lld\t\t\t|", i, ktime_to_ns(plot_time_ptr[i]));
        }
    }
}
/*
* Intialization function of kprobe
*/
static int __init kprobe_init(void)
{
    int ret;
    kp.pre_handler = handler_pre;

    ret = register_kprobe(&kp);
    if (ret < 0)
    {
        pr_err("register_kprobe failed, returned %d\n", ret);
        return ret;
    }
    pr_info("Planted kprobe at %p\n", kp.addr);

    ktime_ptr = kmalloc(sizeof(ktime_t *) * MAX_ADDRESS_COUNT, GFP_KERNEL);
    if (!ktime_ptr)
    {
        pr_err("Error while allocating memory to pointer storing time at which page fault occurred!\n");
        return -1;
    }
    addr_ptr = kmalloc(sizeof(unsigned long int *) * MAX_ADDRESS_COUNT, GFP_KERNEL);
    if (!ktime_ptr)
    {
        pr_err("Error while allocating memory to pointer storing Fault Addresses at which page fault has occurred!\n");
        return -1;
    }
    plot_time_ptr = kmalloc(sizeof(ktime_t *) * MAX_TIME_COUNT, GFP_KERNEL);
    if (!ktime_ptr)
    {
        pr_err("Error while allocating memory to pointer storing time-scalled plottings for the scatter plots!\n");
        return -1;
    }

    return 0;
}

/*
* Exit function of kprobe
*/
static void __exit kprobe_exit(void)
{
    if (addr_count > 0)
    {
        set_time_scalaing_parameters();

        set_time_plot_arr();

        print_address_time_plotting();

        print_fault_address_to_address_label_mapping();

        print_time_to_time_label_mapping();
    }
    free_memory();

    unregister_kprobe(&kp);
    pr_info("kprobe at %p unregistered\n", kp.addr);
}

module_init(kprobe_init)
    module_exit(kprobe_exit)
        MODULE_LICENSE("GPL");
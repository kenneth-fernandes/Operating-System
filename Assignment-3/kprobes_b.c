#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/time.h>

#define MAX_SYMBOL_LEN 64
static char symbol[MAX_SYMBOL_LEN] = "__handle_mm_fault";
static int u_pid = 0;
static int time_count = 0;
static int addr_count = 0;
static ktime_t *ktime_ptr;
static ktime_t *plot_time_ptr;
static unsigned long int *addr_ptr;

module_param_string(symbol, symbol, sizeof(symbol), S_IRUGO);
module_param(u_pid, int, S_IRUGO);

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
        if (time_count < 70)
        {
            ktime_ptr[time_count] = ktime_get();
            plot_time_ptr[time_count] = ktime_ptr[time_count] - ktime_ptr[0];
            time_count += 1;
        }
        if (addr_count < 30)
        {
            addr_ptr[addr_count] = regs->si;
            addr_count+=1;
        }
    }

#endif

    return 0;
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

    ktime_ptr = kmalloc(sizeof(ktime_t *) * 70, GFP_KERNEL);
    plot_time_ptr = kmalloc(sizeof(ktime_t *) * 70, GFP_KERNEL);
    addr_ptr = kmalloc(sizeof(unsigned long int *) * 30, GFP_KERNEL);

    return 0;
}

/*
* Exit function of kprobe
*/
static void __exit kprobe_exit(void)
{
    int i;
    for (i = 0; i < time_count; i += 1)
    {
        pr_info("%d %llu %llu", i, ktime_ptr[i], plot_time_ptr[i]);
    }
    time_count = 0;
    for (i = 0; i < addr_count; i += 1)
    {
        pr_info("%d %lu", i, addr_ptr[i]);
    }
    addr_count = 0;

    kfree(ktime_ptr);
    kfree(plot_time_ptr);
    kfree(addr_ptr);

    unregister_kprobe(&kp);
    pr_info("kprobe at %p unregistered\n", kp.addr);
}

module_init(kprobe_init)
    module_exit(kprobe_exit)
        MODULE_LICENSE("GPL");
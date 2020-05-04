#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>

#define MAX_SYMBOL_LEN 64
static char symbol[MAX_SYMBOL_LEN] = "__handle_mm_fault";
static int u_pid = 0;

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
    //pr_info("inside pre-handler %d",current->pid);
    if (current->pid == u_pid)
    {
        pr_info("<%s> pre_handler: FaultAddress = %lu, Pid = %d",
                p->symbol_name, regs->si, u_pid);
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
    return 0;
}

/*
* Exit function of kprobe
*/
static void __exit kprobe_exit(void)
{
    unregister_kprobe(&kp);
    pr_info("kprobe at %p unregistered\n", kp.addr);
}

module_init(kprobe_init)
    module_exit(kprobe_exit)
        MODULE_LICENSE("GPL");
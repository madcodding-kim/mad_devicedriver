/*
 *    example_tasklet.c
 Here is an example tasklet module. The tasklet_fn function runs for a few 
seconds and in the mean time execution of the example_tasklet_init function 
continues to the exit point.
when triggered from interrupt tasklets usually is selected by user.
tasklet is not parrallel it is syncronous with main task.atomic_add_subtract
it is deprecated because of the feture that it can not access the user space.
 */
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
/*    Macro    DECLARE_TASKLET_OLD    exists    for    compatibility.
 *    See    https://lwn.net/Articles/830964/
 */
#ifndef DECLARE_TASKLET_OLD
#define DECLARE_TASKLET_OLD(arg1, arg2) DECLARE_TASKLET(arg1, arg2, 0L)
#endif

static void tasklet_fn(unsigned long data)
{
    pr_info("Example    tasklet    starts\n");
    mdelay(5000);
    pr_info("Example    tasklet    ends\n");
}

static DECLARE_TASKLET_OLD(mytask, tasklet_fn); //initialize tasklet

static int example_tasklet_init(void)
{
    pr_info("tasklet    example    init\n");
    tasklet_schedule(&mytask); //start tasklet
    mdelay(200);
    pr_info("Example    tasklet    init    continues...\n");
    return 0;
}
static void example_tasklet_exit(void)
{
    pr_info("tasklet    example    exit\n");
    tasklet_kill(&mytask);
}

module_init(example_tasklet_init);
module_exit(example_tasklet_exit);

MODULE_DESCRIPTION("Tasklet    example");
MODULE_LICENSE("GPL");
/*
*    sched.c
To  add  a  task  to  the  scheduler  we  can  use  a  workqueue.    The  kernel  then  uses
the  Completely  Fair  Scheduler  (CFS)  to  execute  work  within  the  queue.
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

static struct workqueue_struct *queue = NULL;
static struct work_struct work;

static void work_handler(struct work_struct *data)
{
    pr_info("work    handler    function start.\n");
    mdelay(300000);
    pr_info("work    handler    function end.\n");
}

static int __init workqueue_init(void)
{
    pr_info("sched_init start.\n");
    queue = alloc_workqueue("HELLOWORLD", WQ_UNBOUND, 1);
    INIT_WORK(&work, work_handler);
    schedule_work(&work);
    pr_info("sched_init end.\n");
    return 0;
}

static void __exit workqueue_exit(void)
{
    destroy_workqueue(queue);
}

module_init(workqueue_init);
module_exit(workqueue_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Workqueue    example");
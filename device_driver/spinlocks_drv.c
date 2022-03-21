/*
 *    example_spinlock.c
As the name suggests, spinlocks lock up the CPU that the code is running on, 
taking 100% of its resources. Because of this you should only use the spinlock 
mechanism around code which is likely to take no more than a few milliseconds 
to run and so will not noticeably slow anything down from the user’s point of 
view.
The example here is "irq safe" in that if interrupts happen during the 
lock then they will not be forgotten and will activate when the unlock happens, 
using the flags variable to retain their state.
 */
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>

static DEFINE_SPINLOCK(sl_static);
static spinlock_t sl_dynamic;

static void example_spinlock_static(void)
{
    unsigned long flags;
    spin_lock_irqsave(&sl_static, flags);
    pr_info("Locked static spinlock\n");
    /*    Do    something    or    other    safely.    Because    this    uses    100%    CPU    time,    this
     *    code    should    take    no    more    than    a    few    milliseconds    to    run.
     */
    spin_unlock_irqrestore(&sl_static, flags);
    pr_info("Unlocked    static    spinlock\n");
}
static void example_spinlock_dynamic(void)
{
    unsigned long flags;
    spin_lock_init(&sl_dynamic);
    spin_lock_irqsave(&sl_dynamic, flags);
    pr_info("Locked dynamic spinlock\n");
    /*    Do    something    or    other    safely.    Because    this    uses    100%    CPU    time,    this
    *    code    should    take    no    more    than    a    few    milliseconds    to    run.

    */
    spin_unlock_irqrestore(&sl_dynamic, flags);
    pr_info("Unlocked    dynamic    spinlock\n");
}
static int example_spinlock_init(void)
{
    pr_info("example    spinlock    started\n");
    example_spinlock_static();
    example_spinlock_dynamic();
    return 0;
}
static void example_spinlock_exit(void)
{
    pr_info("example    spinlock    exit\n");
}
module_init(example_spinlock_init);
module_exit(example_spinlock_exit);
MODULE_DESCRIPTION("Spinlock    example");
MODULE_LICENSE("GPL");
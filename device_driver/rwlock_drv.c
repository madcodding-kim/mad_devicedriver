/*
 *    example_rwlock.c
 Read and write locks are specialised kinds of spinlocks so that you can exclu- 
sively read from something or write to something. Like the earlier spinlocks 
example, the one below shows an "irq safe" situation in which if other functions 
were triggered from irqs which might also read and write to whatever you are 
concerned with then they would not disrupt the logic. As before it is a good 
idea to keep anything done within the lock as short as possible so that it does 
not hang up the system and cause users to start revolting against the tyranny 
of your module.
 It is used multiple reader and one writer because spinlock can not differenciate read and write
*/
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>

static DEFINE_RWLOCK(myrwlock);

static void example_read_lock(void)
{
    unsigned long flags;
    read_lock_irqsave(&myrwlock, flags);
    pr_info("Read    Locked\n");
    /*    Read    from    something    */
    read_unlock_irqrestore(&myrwlock, flags);
    pr_info("Read    Unlocked\n");
}

static void example_write_lock(void)
{
    unsigned long flags;
    write_lock_irqsave(&myrwlock, flags);
    pr_info("Write    Locked\n");
    /*    Write    to    something    */
    write_unlock_irqrestore(&myrwlock, flags);
    pr_info("Write    Unlocked\n");
}
static int example_rwlock_init(void)
{
    pr_info("example_rwlock    started\n");
    example_read_lock();
    example_write_lock();
    return 0;
}
static void example_rwlock_exit(void)
{
    pr_info("example_rwlock    exit\n");
}
module_init(example_rwlock_init);
module_exit(example_rwlock_exit);

MODULE_DESCRIPTION("Read/Write    locks    example");
MODULE_LICENSE("GPL");
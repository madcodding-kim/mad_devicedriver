#include "blocking_drv.h"
static char message[MESSAGE_LENGTH];
static struct proc_dir_entry *our_proc_file;

static ssize_t module_output(struct file *file, char __user *buf, size_t    len,    loff_t *offset) //write to user
{
    pr_alert("%s    call.\n", __func__);
    static int finished = 0; //syncro
    int i;
    char output_msg[MESSAGE_LENGTH + 30];

    if (finished)
    {
        finished = 0;
        return 0;
    }
    sprintf(output_msg, "Last input:%s\n", message);
    for (i = 0; i < len && output_msg[i]; i++)
        put_user(output_msg[i], buf + i);
    finished = 1;
    return i; /*    Return    the    number    of    bytes    "read"    */
}

static ssize_t module_input(struct file *file, const char __user *buf, size_t length, loff_t *offset) //read from user
{
    pr_alert("%s    call.\n", __func__);
    int i;
    for (i = 0; i < MESSAGE_LENGTH - 1 && i < length; i++)
        get_user(message[i], buf + i);
    /*    we    want    a    standard,    zero    terminated    string    */
    message[i] = '\0';
}

static atomic_t already_open = ATOMIC_INIT(0);
static DECLARE_WAIT_QUEUE_HEAD(waitq); //waiting process queue

/*    Called    when    the    /proc    file    is    opened    */
static int module_open(struct inode *inode, struct file *file)
{
    pr_alert("%s    call.\n", __func__);
    if ((file->f_flags & O_NONBLOCK) && atomic_read(&already_open)) // Check if already_open value is locked
    {
        printk("module_open : file->f_flags & O_NONBLOCK) && atomic_read(&already_open");
        return -EAGAIN;
    }
        

    try_module_get(THIS_MODULE);
    while (atomic_cmpxchg(&already_open, 0, 1)) //change 0 -> 1 but if not succeed return
    {
        int i, is_sig = 0;
        /*    This    function    puts    the    current    process,    including    any    system
         *    calls,    such    as    us,    to    sleep.        Execution    will    be    resumed    right
         *    after    the    function    call,    either    because    somebody    called
         *    wake_up(&waitq)    (only    module_close    does    that,    when    the    file
         *    is    closed)    or    when    a    signal,    such    as    Ctrl-C,    is    sent
         *    to    the    process
         */
        wait_event_interruptible(waitq, !atomic_read(&already_open));  //sleep this process until changed already_open value to 0
        /*    If    we    woke    up    because    we    got    a    signal    we're    not    blocking,
         *    return    -EINTR    (fail    the    system    call).        This    allows    processes
         *    to    be    killed    or    stopped.
         */
        for (i = 0; i < _NSIG_WORDS && !is_sig; i++) // array of the signal in this task
            is_sig = current->pending.signal.sig[i] & ~current->blocked.sig[i];

            if (is_sig)
            {
                /*    It    is    important    to    put    module_put(THIS_MODULE)    here,    because
                 *    for    processes    where    the    open    is    interrupted    there    will    never
                 *    be    a    corresponding    close.    If    we    do    not    decrement    the    usage
                 *    count    here,    we    will    be    left    with    a    positive    usage    count
                 * which we will have no way to bring down to zero, giving us
                 * an immortal module, which can only be killed by rebooting
                 * the machine.
                 */
                module_put(THIS_MODULE);
                printk("module_open : current->pending.signal.sig[i] & ~current->blocked.sig[i]");
                return -EINTR;
            }
    }
    return 0;
}

static int module_close(struct inode *inode, struct file *file)
{
    pr_alert("%s    call.\n", __func__);
    atomic_set(&already_open, 0);
    wake_up(&waitq);
    module_put(THIS_MODULE);
    return 0; /* success */
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops file_ops_4_our_proc_file = {
    .proc_read = module_output,   /*    "read"    from    the    file    */
    .proc_write = module_input,   /*    "write"    to    the    file    */
    .proc_open = module_open,     /*    called    when    the    /proc    file    is    opened    */
    .proc_release = module_close, /*    called    when    it's    closed    */
};
#else
static const struct file_operations file_ops_4_our_proc_file = 
{
    .read = module_output,
    .write = module_input,
    .open = module_open,
    .release = module_close,
};
#endif

static int __init sleep_init(void)
{
    pr_alert("%s    call.\n", __func__);
    our_proc_file =proc_create(PROC_ENTRY_FILENAME, 0644, NULL, &file_ops_4_our_proc_file); //create /proc/sleep file
    if (our_proc_file == NULL)
    {
        remove_proc_entry(PROC_ENTRY_FILENAME, NULL);
        pr_debug("Error:    Could    not    initialize    /proc/%s\n", PROC_ENTRY_FILENAME);

        return -ENOMEM;
    }
    proc_set_size(our_proc_file, 80);
    proc_set_user(our_proc_file, GLOBAL_ROOT_UID, GLOBAL_ROOT_GID);
    pr_info("/proc/%s    created\n", PROC_ENTRY_FILENAME);
    return 0;
}
/*    Cleanup    -    unregister    our    file    from    /proc.        This    could    get    dangerous    if
 *    there    are    still    processes    waiting    in    waitq,    because    they    are    inside    our
 *    open    function,    which    will    get    unloaded.    I'll    explain    how    to    avoid    removal
 *    of    a    kernel    module    in    such    a    case    in    chapter    10.
 */
static void __exit sleep_exit(void)
{
    pr_alert("%s    call.\n", __func__);
    remove_proc_entry(PROC_ENTRY_FILENAME, NULL);
    pr_debug("/proc/%s    removed\n", PROC_ENTRY_FILENAME);
}

module_init(sleep_init);
module_exit(sleep_exit);
MODULE_LICENSE("GPL");
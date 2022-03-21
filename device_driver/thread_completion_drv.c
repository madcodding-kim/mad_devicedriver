#include <linux/completion.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>

static struct
{
    struct completion crank_comp;
    struct completion flywheel_comp;
} machine; // thread completion state

static int machine_crank_thread(void *arg)
{
    pr_info("Turn the crank\n");
    complete_all(&machine.crank_comp);  //Wake all waiting thread in this case it is flywheel.
    complete_and_exit(&machine.crank_comp, 0); //notify its completion.
}

static int machine_flywheel_spinup_thread(void *arg)
{
    wait_for_completion(&machine.crank_comp);
    pr_info("Flywheel    spins    up\n");
    complete_all(&machine.flywheel_comp);
    complete_and_exit(&machine.flywheel_comp, 0);
}

static int completions_init(void)
{
    struct task_struct *crank_thread;
    struct task_struct *flywheel_thread;
    pr_info("completions    example\n");

    init_completion(&machine.crank_comp); //completion initialize
    init_completion(&machine.flywheel_comp);

    crank_thread = kthread_create(machine_crank_thread, NULL, "KThreadCrank");
    if (IS_ERR(crank_thread))
    {
        return -1;
    }  
    flywheel_thread = kthread_create(machine_flywheel_spinup_thread, NULL,
                                     "KThread    Flywheel");
    if (IS_ERR(flywheel_thread))
    {
        kthread_stop(crank_thread);
        return -1;
    }    

    wake_up_process(flywheel_thread); //first start flywheel
    wake_up_process(crank_thread); //next start crank
    return 0;
}

static void completions_exit(void)
{
    wait_for_completion(&machine.crank_comp);  //Interrup is not acceptable in this waiting
    wait_for_completion(&machine.flywheel_comp);

    /*
wait_for_completion_timeout    
wait_for_completion_interruptible
wait_for_completion_interruptible_timeout
wait_for_completion_killable
try_wait_for_completions
    */
    pr_info("completions    exit\n");
}

module_init(completions_init);
module_exit(completions_exit);
MODULE_DESCRIPTION("Completions    example");
MODULE_LICENSE("GPL");
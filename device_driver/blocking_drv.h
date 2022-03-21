#include    <linux/kernel.h>    
#include    <linux/module.h>    
#include    <linux/proc_fs.h>    
#include    <linux/sched.h>    

#include    <linux/uaccess.h>    
#include    <linux/version.h>
#include    <linux/printk.h>

#if    LINUX_VERSION_CODE    >=    KERNEL_VERSION(5,    6,    0) 
#define    HAVE_PROC_OPS
#endif

#define    MESSAGE_LENGTH    80
#define    PROC_ENTRY_FILENAME    "sleep"
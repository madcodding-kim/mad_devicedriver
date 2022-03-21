#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/uaccess.h>
#include <linux/usb.h>
#include <linux/mutex.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/mm.h>
//#include <linux/hardware.h>
#include <asm/uaccess.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>        
#include <linux/proc_fs.h>    
#include <linux/sched.h>      
#include <linux/version.h>
#include <linux/printk.h>


#if    LINUX_VERSION_CODE    >=    KERNEL_VERSION(5,    6,    0) 
#define    HAVE_PROC_OPS
#endif

#define    MESSAGE_LENGTH    80
#define    PROC_ENTRY_FILENAME    "intr"
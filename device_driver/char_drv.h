#ifndef _CHAR_DRV_
#define _CHAR_DRV_

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
#define MAJOR_NUM 201
#define IOCTL_SET_MSG   _IOW(MAJOR_NUM, 0, char *)
#define IOCTL_GET_MSG   _IOR(MAJOR_NUM, 1, char *)
#define IOCTL_GET_NTH_BYTE  _IOWR(MAJOR_NUM, 2, int)


#define    BUF_LEN    80
enum    {
CDEV_NOT_USED    =    0, 
CDEV_EXCLUSIVE_OPEN    =    1,
};

DECLARE_WAIT_QUEUE_HEAD(key_queue);
#endif
/*
 *    chardev.h    -    the    header    file    with    the    ioctl    definitions.
 *
 * The declarations here have to be in a header file, because they need
 * to be known both to the kernel module (in chardev2.c) and the process
 * calling ioctl() (in userspace_ioctl.c).
 */
#ifndef CHARDEV_H
#define CHARDEV_H
#include <linux/ioctl.h>

#define MAJOR_NUM 201
#define IOCTL_SET_MSG   _IOW(MAJOR_NUM, 0, char *)
#define IOCTL_GET_MSG   _IOR(MAJOR_NUM, 1, char *)
#define IOCTL_GET_NTH_BYTE  _IOWR(MAJOR_NUM, 2, int)


#define DEVICE_PATH "/dev/test"
#endif
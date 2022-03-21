#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h> 
#include <linux/uaccess.h> //copy from/to user

#define MIN(a,b) (((a) <= (b)) ? (a) : (b))
#define BULK_EP_OUT 0x01
#define BULK_EP_IN 0x82
#define MAX_PKT_SIZE 512 
#define MAX_TRANSFER (PAGE_SIZE -512)
#define USB_VENDOR_ID 0x0781
#define USB_PRODUCT_ID 0x5567
#define USB_MINOR_BASE 192
#define WRITES_IN_FLIGHT 8

static struct usb_device *device;  //usb device
static struct usb_interface *interface;  //usb device interface
static struct semaphore limit_sem; //limit write count
static struct usb_anchor submitted; //in case we need to retract our submission
static struct urb* bulk_in_urb; //urb to read data
static unsigned char *bulk_in_buffer;  //receiving data buffer
static size_t bulk_in_filled; //buffer's data bytes
static size_t bult_in_copied; //copied data size to user space
static __u8 bulk_in_endpointAddr; //bulk address
static int errors;
static bool ongoing_read;
static bool processed_urb;
static struct usb_class_driver class; 
static unsigned char bulk_buf[MAX_PKT_SIZE]; 
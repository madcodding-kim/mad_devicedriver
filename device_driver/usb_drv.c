#include "usb_drv.h"

static int pen_open(struct inode *i, struct file *f)
{ 
    return 0; 
}  

static int pen_close(struct inode *i,struct file *f)
{
    return 0; 
}  

static ssize_t pen_read(struct file *f,char __user *buf , size_t cnt, loff_t *ppos)
{ 
    int retval; int read_cnt;
    retval = usb_bulk_msg(device,usb_rcvbulkpipe(device,BULK_EP_IN),bulk_buf, MAX_PKT_SIZE, &read_cnt,5000); //data transfer without URB, usb_rcvbulkpipe (bulk endpoint)
    
    if(retval)
    {
        printk(KERN_ERR "Bulk message returned %d \n", retval);
        return retval;
    }
    if (copy_to_user(buf, bulk_buf, MIN(cnt, read_cnt)))  
    {
        return -EFAULT;
    }
    return MIN(cnt, read_cnt);
}

static ssize_t pen_write(struct file *f, const char __user *buf, size_t cnt,loff_t *ppos)
{ 
    int retval;
    int wrote_cnt =MIN(cnt,MAX_PKT_SIZE);
    if(copy_from_user(bulk_buf, buf, MIN(cnt,MAX_PKT_SIZE)))
    {
        return -EFAULT;
    }
    /*write data in to bulk endpoint */
    retval =usb_bulk_msg(device,usb_sndbulkpipe(device, BULK_EP_OUT),bulk_buf,MIN(cnt,MAX_PKT_SIZE), &wrote_cnt ,5000);
    printk(KERN_ERR "Bulk message returned %d\n", retval);

    return wrote_cnt;
} 

static struct file_operations fops = {
.open = pen_open,
.release =pen_close,
.read =pen_read,
.write =pen_write,
};

static int pen_probe(struct usb_interface *interface, const struct usb_device_id *id)
{ 
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;  //information of endpoint,   
    int retval;
    int i;
    iface_desc = interface->cur_altsetting;  
    printk (KERN_INFO "Pen i/f %d now probed:(%04X:%04X)\n",iface_desc->desc.bInterfaceNumber, id->idVendor, id->idProduct); 
    printk(KERN_INFO "ID->bNumEndpoints: %02X\n",iface_desc->desc.bNumEndpoints);
    printk(KERN_INFO "ID>bInterfaceclass: %02X\n", iface_desc->desc.bInterfaceClass);
    for(i=0; i < iface_desc->desc.bNumEndpoints; i++)
    {
        endpoint = &iface_desc->endpoint[i].desc;
        printk(KERN_INFO "ED[%d]>bEndpointAddress: 0x%02X\n",i, endpoint->bEndpointAddress);  //Determine whether input or output
        printk(KERN_INFO "ED[%d]>bmAttributes: 0x%02X\n", i, endpoint->bmAttributes);         //endpoint type (control or interrupt or bulk, isochronous  )    
        printk(KERN_INFO "ED[%d]>wMaxPacketSize: 0x%04X (%d)\n",i, endpoint->wMaxPacketSize, endpoint-> wMaxPacketSize); //maximum package size
        printk(KERN_INFO "ED[%d]>wMaxPacketSize: %d\n",i, endpoint->bInterval); //interrupt intermval
    }
    device = interface_to_usbdev(interface); //change interface to usb device

    class.name = "usb/pen%d";
    class.fops = &fops;
    if((retval = usb_register_dev(interface, &class)) <0)  //Registe minor number used by sysfs
    {
        printk(KERN_ERR, "Not able to get a minor for this device");
    }
    else
    {
        printk(KERN_INFO, "Obtained minor[%d]\n", interface->minor);
    }

    return retval;
}  

static void pen_disconnect(struct usb_interface *interface)
{ 
    printk(KERN_INFO "Pen i/f %d now disconnected \n",interface->cur_altsetting->desc.bInterfaceNumber); 
    usb_deregister_dev(interface, &class);
} 

static struct usb_device_id pen_table[]= {{ USB_DEVICE(USB_VENDOR_ID , USB_PRODUCT_ID)}, {}}; //usb device indicator
MODULE_DEVICE_TABLE (usb, pen_table); 

static struct usb_driver pen_driver = 
{
    .name = "my_usb_driver",
    .id_table = pen_table,
    .probe = pen_probe,
    .disconnect = pen_disconnect, 
}; 



static int __init pen_init(void)
{
    int result;
    if(( result = usb_register(&pen_driver)))  //Register usb device to usb core
    {
        printk(KERN_ERR, "usb_register failed .err no %d", result);  
    }
    return result;
} 

static void __exit pen_exit(void)
{
    usb_deregister(&pen_driver);
}

module_init(pen_init);
module_exit(pen_exit);

MODULE_LICENSE( "GPL");

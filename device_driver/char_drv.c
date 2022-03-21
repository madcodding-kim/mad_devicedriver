// #include <linux/module.h>
// #include <linux/kernel.h>
// #include <linux/init.h>
// #include <linux/types.h>
// #include <linux/ioport.h>
// #include <linux/slab.h>
// #include <linux/mm.h>

//#include <asm/hardware.h>
// #include <asm/uaccess.h>
// #include <asm/irq.h>

// #include <unistd.h>

#include "char_drv.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WJKIM");
MODULE_DESCRIPTION("OUR FIRST charactor DRIVER:automatic");
// mknod /dev/test c 66 1

int paramArray[3];
module_param_array(paramArray, int, NULL, S_IRUSR | S_IWUSR);
int paramTest;
module_param(paramTest, int, S_IRUSR | S_IWUSR);

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;
static int share_data = 0;
static int ioctl_num = 0;
static unsigned int num_of_dev = 1;
static int irq_led_blue_number;
static int gpio_led_blue_number;
static int irq_led_red_number;
static int gpio_led_red_number;

static struct kobject *shared_data_module;
static    atomic_t    already_open    =    ATOMIC_INIT(CDEV_NOT_USED); //Initilize with 0
static    char    message[BUF_LEN] = {0,};


static ssize_t test_share_data_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", share_data);
}

static ssize_t test_share_data_store(struct kobject *kobj, struct kobj_attribute *attr, char *buf, size_t count)
{
    sscanf(buf, "%du", &share_data);
    return count;
}

static struct kobj_attribute share_date_attribute =
    __ATTR(share_data, 0660, (void *)test_share_data_show, (void *)test_share_data_store);


static irqreturn_t led_blue_interrupt(int irq, void *data) //irq number and gpio number is not equal sometimes
{
    share_data = 100;
    printk("led_blue_interrupt Service : %d \n", share_data);
    wake_up_interruptible(&key_queue);
    return 0;
}

static irqreturn_t led_red_interrupt(int irq, void *data)
{
    share_data = 200;
    printk("led_red_interrupt Service : %d \n", share_data);
    wake_up_interruptible(&key_queue);
    return 0;
}

static int test_open(struct inode *inode, struct file *fp)
{
    pr_alert("%s    call.\n", __func__);
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;

    try_module_get(THIS_MODULE); //equal to MOD_INC_USE_COUNT
    return 0;
}

static int test_release(struct inode *inode, struct file *fp)
{
    pr_alert("%s    call.\n", __func__);
    atomic_set(&already_open, CDEV_NOT_USED);
    module_put(THIS_MODULE); // equal to MOD_DEC_USE_COUNT
    return 0;
}

static ssize_t test_write(struct file *fp, const char *buffer, size_t bufsize, loff_t *len)
{
    pr_alert("%s    call.\n", __func__);

    int i;
    pr_info("test_write(%p,%p,%ld)", fp, buffer, bufsize);

    copy_from_user(message, buffer, bufsize);
    // for (i = 0; i < len && i < BUF_LEN; i++)
    //     get_user(message[i], buffer + i);
    printk("test_write message : %s", message);
    
    return i;
}

static ssize_t test_read(struct file *file, char __user *buffer, size_t length, loff_t *offset)
{
    pr_alert("%s    call.\n", __func__);
    int bytes_read = 0;
    const char *message_ptr = message;
    if (!*(message_ptr + *offset))
    {                
        pr_info("test_read : message is null\n");
        *offset = 0; 
        return 0;   
    }
    message_ptr += *offset;
    while (length && *message_ptr)
    {
        put_user(*(message_ptr++), buffer++);
        length--;
        bytes_read++;
    }
    
    pr_info("Read %d bytes, %ld left\n", bytes_read, length);
    *offset += bytes_read;
    return bytes_read;
}


static long test_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
    pr_alert("%s    call.\n", __func__);
    int i;

    switch (ioctl_num)
    {
    case IOCTL_SET_MSG:
    {
        char __user *tmp = (char __user *)ioctl_param;
        char ch;
        get_user(ch, tmp);
        for (i = 0; ch && i < BUF_LEN; i++, tmp++) //check size of data
        {
            get_user(ch, tmp);
            printk("IOCTL_SET_MSG ch[%c]", ch);
        }

        test_write(file, (char __user *)ioctl_param, i, NULL);
        break;
    }
    case IOCTL_GET_MSG:
    {
        loff_t offset = 0;
        i = test_read(file, (char __user *)ioctl_param, 99, &offset);
        put_user('\0', (char __user *)ioctl_param + i);
        break;
    }
    case IOCTL_GET_NTH_BYTE:
        return (long)message[ioctl_param];
        break;
    }
    return 0;
}


static struct file_operations test_fops = {
    .owner = THIS_MODULE,
    .write = test_write,
    .read = test_read,
    .open = test_open,
    .release = test_release,
    .unlocked_ioctl = test_ioctl,
};

void setGpioInterrupt(void)
{
    gpio_led_blue_number = 299;
    if (gpio_request(gpio_led_blue_number, "led_blue_gpio") < 0)
    {
        printk(KERN_ERR "Fail to alloc led_blue_gpio");
    }
    else
    {
        printk("led_blue_gpio_number[%d]", gpio_led_blue_number);
        gpio_direction_output(gpio_led_blue_number, 1);
        gpio_export(gpio_led_blue_number, true);
    }

    if ((irq_led_blue_number = gpio_to_irq(gpio_led_blue_number)) < 0)
    {
        printk(KERN_ERR "Fail to get gpio_led_blue irq number");
    }
    else
    {
        printk("Irq number gpio_led_blue[%d]", irq_led_blue_number);
    }

    if (request_irq(irq_led_blue_number, led_blue_interrupt, /*SA_INTERRUPT*/ IRQF_TRIGGER_FALLING, "led_blue", NULL) < 0)
        printk(KERN_ERR "request_irq() FAIL !\n");

    gpio_led_red_number = 301;
    if (gpio_request(gpio_led_red_number, "led_red_gpio") < 0)
    {
        printk(KERN_ERR "Fail to alloc led_red_gpio");
    }
    else
    {
        printk("led_red_gpio_number[%d]", gpio_led_red_number);
        gpio_direction_output(gpio_led_red_number, 1);
        gpio_export(gpio_led_red_number, true);
    }

    if ((irq_led_red_number = gpio_to_irq(gpio_led_red_number)) < 0)
    {
        printk(KERN_ERR "Fail to get irq irq_led_red_number");
    }
    else
    {
        printk("Irq number led_red[%d]", irq_led_red_number);
    }

    if (request_irq(irq_led_red_number, led_red_interrupt, IRQF_TRIGGER_FALLING, "led_red", NULL) < 0)
        printk(KERN_ERR "request_irq() FAIL !\n");
}

int param_init(void)
{

    printk(KERN_ALERT "TEST_DRV init");
    // int result;

    // if((result = register_chrdev(MAJOR_NUM, "Test device", &test_fops)) <0)
    // {
    // printk("HELLO-init_module: can't get major %d.\n", MAJOR_NUM);
    // return result;
    // }
    dev = MKDEV(MAJOR_NUM, 0);
    if (register_chrdev_region(dev, num_of_dev, "TEST DRV") < 0)
    {
        printk("Init_module: can't get dev number\n");
        return -1;
    }

    printk("Init_module major[%d], minor[%d]\n", MAJOR(dev), MINOR(dev));

    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
    {
        unregister_chrdev_region(dev, 1);
        return -1;
    }

    if (device_create(cl, NULL, dev, NULL, "test") == NULL)
    {
        class_destroy(cl);
        unregister_chrdev_region(dev, 1);
    }

    cdev_init(&c_dev, &test_fops);
    if (cdev_add(&c_dev, dev, num_of_dev) == -1)
    {
        device_destroy(cl, dev);
        class_destroy(cl);
        unregister_chrdev_region(dev, 1);
        return -1;
    }

    // struct task_struct *task;
    // for_each_process(task)
    // {
    //     printk("process Name :%s\t PID:%d\t process State:%ld\n", task->comm, task->pid, task->state);
    // }

    // printk("parameter init\n");
    // printk("parameter value : %d\n", paramTest);
    // printk("Array parameter value : %d\t%d\t%d\n", paramArray[0], paramArray[1], paramArray[2]);

    setGpioInterrupt();

    shared_data_module = kobject_create_and_add("share_data", kernel_kobj);
    if (!shared_data_module)
        return -ENOMEM;

    int error = sysfs_create_file(shared_data_module, &share_date_attribute.attr);
    if (error)
    {
        pr_info("failed to create the myvariable file in    /sys/kernel/mymodule\n");
    }

    return 0;
}

void param_exit(void)
{
    kobject_put(shared_data_module);

    disable_irq(irq_led_blue_number);
    disable_irq(irq_led_red_number);

    free_irq(irq_led_blue_number, NULL);
    free_irq(irq_led_red_number, NULL);

    gpio_free(gpio_led_blue_number);
    gpio_free(gpio_led_red_number);

    printk(KERN_ALERT "CHAR_DRV exit");
    device_destroy(cl, dev);
    class_destroy(cl);
    unregister_chrdev_region(dev, num_of_dev);
    // unregister_chrdev(MAJOR_NUM, "Test device");
}

module_init(param_init);
module_exit(param_exit);
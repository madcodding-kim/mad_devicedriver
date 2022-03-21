#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <asm/uaccess.h>

/***Information about this device***/
MODULE_LICENSE("Dual BSD/GPL");
#define DRIVER_NAME "MyDevice"				/* /proc/Device name displayed on devices etc.*/
static const unsigned int MINOR_BASE = 0;	/*Starting number and number of minor numbers used in this device driver(=Number of devices) */
static const unsigned int MINOR_NUM  = 1;	/*Minor number is 0*/

/***I2C device management information***/
/*I2C devices handled by this device driver*/
enum mydevice_i2c_model {
	MYDEVICE_MODEL_A = 0,
	MYDEVICE_MODEL_NUM,
};

/*Register the table that identifies the device handled by this device driver*/
/*The important thing is the first name field. This determines the device name. The back is the data that can be used freely with this driver. Insert pointers and identification numbers*/
static struct i2c_device_id mydevice_i2c_idtable[] = {
	{"MyI2CDevice", MYDEVICE_MODEL_A},
	{ }
};
MODULE_DEVICE_TABLE(i2c, mydevice_i2c_idtable);

/*Each I2C device(client)Information associated with. Set at probe time i2c_set_Keep in clientdata*/
struct mydevice_device_info {
	struct cdev        cdev;			/*probed I2C device(client)Required to associate cdev with. container at open_Search by of*/
	unsigned int       mydevice_major;	/*Major number of this device driver(Dynamically decide) */
	struct class      *mydevice_class;	/*Device driver class object*/
	struct i2c_client *client;
	/*Add other if necessary. mutex etc.*/
};


/*Function called at open*/
static int mydevice_open(struct inode *inode, struct file *file)
{
	printk("mydevice_open");

	/*Cdev with this open(inode->i_cdev)With mydevice_device_Find info*/
	struct mydevice_device_info *dev_info;
	dev_info = container_of(inode->i_cdev /*address*/, struct mydevice_device_info , cdev);
	if (dev_info  == NULL || dev_info->client  == NULL) {
		printk(KERN_ERR "container_of\n");
		return -EFAULT;
	}
	file->private_data = dev_info;
	printk("i2c address = %02X\n",dev_info->client->addr);

	return 0;
}

/*Function called at close*/
static int mydevice_close(struct inode *inode, struct file *file)
{
	printk("mydevice_close");
	return 0;
}

/*Function called when reading*/
static ssize_t mydevice_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	printk("mydevice_read");

	struct mydevice_device_info *dev_info = filp->private_data;
	struct i2c_client * client = dev_info->client;

	int version;
	version = i2c_smbus_read_byte_data(client, 0x0f);
	return sprintf(buf, "id = 0x%02X\n", version);
}

/*Function called when writing*/
static ssize_t mydevice_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	printk("mydevice_write");
	return count;
}

/*Handler table for various system calls*/
struct file_operations s_mydevice_fops = {
	.open    = mydevice_open,
	.release = mydevice_close,
	.read    = mydevice_read,
	.write   = mydevice_write,
};


static int mydevice_i2c_create_cdev(struct mydevice_device_info *dev_info)
{
	int alloc_ret = 0;
	int cdev_err = 0;
	dev_t dev;

	/*Secure a free major number*/
	alloc_ret = alloc_chrdev_region(&dev, MINOR_BASE, MINOR_NUM, DRIVER_NAME);
	if (alloc_ret != 0) {
		printk(KERN_ERR  "alloc_chrdev_region = %d\n", alloc_ret);
		return -1;
	}

	/*Obtained dev( =Major number+Minor number)Get the major number from and keep it*/
	dev_info->mydevice_major = MAJOR(dev);
	dev = MKDEV(dev_info->mydevice_major, MINOR_BASE);	/*Unnecessary? */

	/*Initialization of cdev structure and registration of system call handler table*/
	cdev_init(&dev_info->cdev, &s_mydevice_fops);
	dev_info->cdev.owner = THIS_MODULE;

	/*This device driver(cdev)In the kernel*/
	cdev_err = cdev_add(&dev_info->cdev, dev, MINOR_NUM);
	if (cdev_err != 0) {
		printk(KERN_ERR  "cdev_add = %d\n", alloc_ret);
		unregister_chrdev_region(dev, MINOR_NUM);
		return -1;
	}

	/*Register the class of this device(/sys/class/mydevice/make) */
	dev_info->mydevice_class = class_create(THIS_MODULE, "mydevice");
	if (IS_ERR(dev_info->mydevice_class)) {
		printk(KERN_ERR  "class_create\n");
		cdev_del(&dev_info->cdev);
		unregister_chrdev_region(dev, MINOR_NUM);
		return -1;
	}

	/* /sys/class/mydevice/mydevice*make*/
  int minor = MINOR_BASE;
	for (; minor < MINOR_BASE + MINOR_NUM; minor++) {
		device_create(dev_info->mydevice_class, NULL, MKDEV(dev_info->mydevice_major, minor), NULL, "mydevice%d", minor);
	}

	return 0;
}

static void mydevice_i2c_delete_cdev(struct mydevice_device_info *dev_info)
{
	dev_t dev = MKDEV(dev_info->mydevice_major, MINOR_BASE);
	
	/* /sys/class/mydevice/mydevice*To delete*/
  int minor = MINOR_BASE;
	for (; minor < MINOR_BASE + MINOR_NUM; minor++) {
		device_destroy(dev_info->mydevice_class, MKDEV(dev_info->mydevice_major, minor));
	}

	/*Remove class registration for this device(/sys/class/mydevice/To delete) */
	class_destroy(dev_info->mydevice_class);

	/*This device driver(cdev)From the kernel*/
	cdev_del(&dev_info->cdev);

	/*Remove the major number registration used in this device driver*/
	unregister_chrdev_region(dev, MINOR_NUM);
}

static int mydevice_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	printk("mydevice_i2c_probe\n");
	printk("id.name = %s, id.driver_data = %d", id->name, (int)(id->driver_data));
	printk("slave address = 0x%02X\n", client->addr);

	/*Usually here to check if the device is supported by this driver*/

	/* open/close/read/i2c even with write_The client is used, so keep it*/
	struct mydevice_device_info *dev_info;
	dev_info =  (struct mydevice_device_info*)devm_kzalloc(&client->dev, sizeof(struct mydevice_device_info), GFP_KERNEL);
	dev_info->client = client;
	i2c_set_clientdata(client, dev_info);
	

	/*Register this device driver as a character type in the kernel.(/sys/class/mydevice/mydevice*make) */
	if(mydevice_i2c_create_cdev(dev_info)) return -ENOMEM;

	return 0;
}

static int mydevice_i2c_remove(struct i2c_client *client)
{
	printk("mydevice_i2c_remove\n");
	struct mydevice_device_info *dev_info;
	dev_info = i2c_get_clientdata(client);
	mydevice_i2c_delete_cdev(dev_info);

	return 0;
}

static struct i2c_driver mydevice_driver = {
	.driver = {
		.name	= DRIVER_NAME,
		.owner = THIS_MODULE,
	},
	.id_table		= mydevice_i2c_idtable,		//I2C devices supported by this device driver
	.probe			= mydevice_i2c_probe,		//Process called when the target I2C device is recognized
	.remove			= mydevice_i2c_remove,		//Process called when the target I2C device is removed
};


/*Register this device driver as a device driver that uses the I2C bus.*/
module_i2c_driver(mydevice_driver);
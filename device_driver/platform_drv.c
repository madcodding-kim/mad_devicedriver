/*
 *    devicemodel.c
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
struct devicemodel_data
{
    char *greeting;
    int number;
};

static int devicemodel_probe(struct platform_device *dev) //device instance 
{
    struct devicemodel_data *pd =
        (struct devicemodel_data *)(dev->dev.platform_data);
    pr_info("devicemodel    probe\n");
    pr_info("devicemodel    greeting:    %s;    %d\n", pd->greeting, pd->number);
    /*    Your    device    initialization    code    */
    return 0;
}

static int devicemodel_remove(struct platform_device *dev)
{
    pr_info("devicemodel    example    removed\n");
    /*    Your    device    removal    code    */
    return 0;
}

static int devicemodel_suspend(struct device *dev)
{
    pr_info("devicemodel    example    suspend\n");
    /*    Your    device    suspend    code    */
    return 0;
}

static int devicemodel_resume(struct device *dev)
{
    pr_info("devicemodel    example    resume\n");
    /*    Your    device    resume    code    */
    return 0;
}

static const struct dev_pm_ops devicemodel_pm_ops = {
    .suspend = devicemodel_suspend,
    .resume = devicemodel_resume,
    .poweroff = devicemodel_suspend,
    .freeze = devicemodel_suspend,
    .thaw = devicemodel_resume,
    .restore = devicemodel_resume,
};

static struct platform_driver devicemodel_driver = 
{
    .driver =
    {
        .name = "devicemodel_example", //driver name
        .owner = THIS_MODULE,
        .pm = &devicemodel_pm_ops,
    },
    .probe = devicemodel_probe,
    .remove = devicemodel_remove,
};

int devicenumber = 1;

static int devicemodel_init(void)
{
    int ret;
    pr_info("devicemodel    init : %d\n", devicenumber);
    ret = platform_driver_register(&devicemodel_driver); //Register into platform bus
    if (ret)
    {
        pr_err("Unable    to    register    driver\n");

        return ret;
    }
    return 0;
}

static void devicemodel_exit(void)
{
    pr_info("devicemodel    exit\n");
    platform_driver_unregister(&devicemodel_driver);
}

module_init(devicemodel_init);
module_exit(devicemodel_exit);

MODULE_ALIAS("platform-driver"); //Module name and information for user space
MODULE_AUTHOR("WJKIM");
MODULE_VERSION("1.0.0"); 
MODULE_LICENSE("GPL"); 
MODULE_DESCRIPTION("Linux    Device    Model    example");
module_param(devicenumber, int, 0);

//modinfo platform_drv.ko
//insmod platform_drv.ko devicenumber=33
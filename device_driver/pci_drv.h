#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pci.h>

#define MY_DRIVER "my_pci_driver"

/* This sample driver supports device with VID = 0x010F, and PID = 0x0F0E*/
static struct pci_device_id my_driver_id_table[] = {
    { PCI_DEVICE(0x15AD, 0x1976) },
    {0,}
};

MODULE_DEVICE_TABLE(pci, my_driver_id_table);

static int my_driver_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static void my_driver_remove(struct pci_dev *pdev);

/* Driver registration structure */
static struct pci_driver my_driver = {
    .name = MY_DRIVER,
    .id_table = my_driver_id_table,
    .probe = my_driver_probe,
    .remove = my_driver_remove
};

/* This is a "private" data structure */
/* You can store there any data that should be passed between driver's functions */
struct my_driver_priv {
    u8 __iomem *hwmem;
};

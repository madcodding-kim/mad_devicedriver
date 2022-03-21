ARCH=arm64
CROSS_COMPILE=aarch64-linux-gnu-
KERN_DIR=/home/nvidia/bsp_l4t/Linux_for_Tegra/out/modules/lib/modules/4.9.140-tegra-eimx290/build
HOST_KERN_DIR=/lib/modules/$(shell uname -r)/build/
PWD=$(shell pwd)
PROJECT_NAME=i2c_eeprom_drv

obj-m+=$(PROJECT_NAME).o#char_drv.o usb_skel_drv.o usb_drv.o pci_drv.o 
objs+=$(PROJECT_NAME).o#char_drv.o usb_skel_drv.o usb_drv.o pci_drv.o 

all: 
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) clean
	rm -rf *.ko
	rm -rf *.o
help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) help
install:
	scp $(PROJECT_NAME).ko sgrt@10.11.231.169:/home/sgrt
host:
	make -C $(HOST_KERN_DIR) M=$(PWD) modules
hostClean:
	make -C $(HOST_KERN_DIR) M=$(PWD) clean
	rm -rf *.ko
	rm -rf *.o
hostHelp:
	make -C $(HOST_KERN_DIR) M=$(PWD) help


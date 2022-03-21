KERN_DIR=/home/nvidia/bsp_l4t/Linux_for_Tegra/out/modules/lib/modules/4.9.140-tegra-eimx290/build
ARCH=arm64
CROSS_COMPILE=aarch64-linux-gnu-gcc
CC=gcc
FILE_NAME=intrpt_drv_app#ioctl_drv_app char_drv_app

all: 
	$(CROSS_COMPILE) $(INCLUDES) $(LIBS) $(LDFLAGS) -o $(FILE_NAME).out $(FILE_NAME).cpp
host:
	$(CC) $(INCLUDES) $(LIBS) $(LDFLAGS) -o $(FILE_NAME).out $(FILE_NAME).cpp
clean:
	rm -rf *.out
	rm -rf *.o
install:
	scp $(FILE_NAME).out sgrt@10.11.231.169:/home/sgrt
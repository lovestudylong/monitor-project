KERNELDIR :=/home/shrek/Linux_4412/kernel/linux-3.14
PWD :=$(shell pwd)

ARM_DRVDIR=/opt/4412/rootfs/drv/

obj-m += leddrv.o adcdrv.o

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

install:
	sudo cp *.ko  $(ARM_DRVDIR)

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
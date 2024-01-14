KERNELDIR :=/home/shrek/Linux_4412/kernel/linux-3.14
PWD :=$(shell pwd)

CROSS_COMPILE=arm-linux-
CC=$(CROSS_COMPILE)gcc

ARM_DRVDIR=/opt/4412/rootfs/drv/
NFS_BOA_WWWDIR=/opt/4412/rootfs/boa/www/
NFS_BOA_CGIDIR=/opt/4412/rootfs/boa/cgi-bin/

obj-m +=leddrv.o 

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	$(CC) -o cgi_led.cgi -static cgi_led.c

install:
	sudo rm $(ARM_DRVDIR)leddrv.ko
	sudo rm $(NFS_BOA_WWWDIR)led.html
	sudo rm $(NFS_BOA_CGIDIR)cgi_led.cgi

	sudo cp leddrv.ko  $(ARM_DRVDIR)
	sudo cp led.html $(NFS_BOA_WWWDIR)
	sudo cp cgi_led.cgi $(NFS_BOA_CGIDIR)

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -rf cgi_led.cgi
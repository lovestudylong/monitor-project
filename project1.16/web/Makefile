KERNELDIR :=/home/shrek/Linux_4412/kernel/linux-3.14
PWD :=$(shell pwd)

CROSS_COMPILE=arm-linux-
CC=$(CROSS_COMPILE)gcc

AFLAGS=-Wall -g
LDFLAGS= -L ./lib

ARM_DRVDIR=/opt/4412/rootfs/drv/
NFS_BOA_WWWDIR=/opt/4412/rootfs/boa/www/
NFS_BOA_CGIDIR=/opt/4412/rootfs/boa/cgi-bin/

obj-m +=leddrv.o

OBJS= cgi_refresh.cgi

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	$(CC) -o cgi_led.cgi -static cgi_led.c
	$(CC) -o cgi_refresh.cgi -static cgi_refresh.c

install:
	sudo rm $(NFS_BOA_WWWDIR)* -rf
	sudo rm $(NFS_BOA_CGIDIR)* -rf

	sudo cp leddrv.ko  $(ARM_DRVDIR)
	sudo cp monitor.html $(NFS_BOA_WWWDIR)
	sudo cp cgi_led.cgi $(NFS_BOA_CGIDIR)
	sudo cp cgi_refresh.cgi $(NFS_BOA_CGIDIR)

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm *.cgi
CROSS_COMPILE=arm-linux-
CC=$(CROSS_COMPILE)gcc

AFLAGS=-Wall -g
LDFLAGS= -L ./lib

NFS_BOA_WWWDIR=/opt/4412/rootfs/boa/www/
NFS_BOA_CGIDIR=/opt/4412/rootfs/boa/cgi-bin/

OBJS= cgi_refresh.cgi

all:
	$(CC) -o cgi_led.cgi -static cgi_led.c
	$(CC) -o cgi_refresh.cgi -static cgi_refresh.c

install:
	sudo rm $(NFS_BOA_WWWDIR)* -rf
	sudo rm $(NFS_BOA_CGIDIR)* -rf

	sudo cp monitor.html $(NFS_BOA_WWWDIR)
	sudo cp cgi_led.cgi $(NFS_BOA_CGIDIR)
	sudo cp cgi_refresh.cgi $(NFS_BOA_CGIDIR)

clean:
	rm *.cgi
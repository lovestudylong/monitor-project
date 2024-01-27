CROSS_COMPILE = arm-linux-
CC=$(CROSS_COMPILE)gcc

AFLAGS = -Wall -g
LDFLAGS = -static

NFS_BOA_WWWDIR = /opt/4412/rootfs/boa/www/
NFS_BOA_CGIDIR = /opt/4412/rootfs/boa/cgi-bin/

OBJS = cgi_refresh.cgi cgi_led.cgi cgi_buzzer.cgi

all: $(OBJS)

%.cgi: %.c
	$(CC) $(LDFLAGS) -o $@ $<

install:
	sudo rm $(NFS_BOA_WWWDIR)* -rf
	sudo rm $(NFS_BOA_CGIDIR)* -rf

	sudo cp *.html $(NFS_BOA_WWWDIR)
	sudo cp *.css $(NFS_BOA_WWWDIR)
	sudo cp images $(NFS_BOA_WWWDIR) -rf
	sudo cp *.cgi $(NFS_BOA_CGIDIR)
	sudo cp favicon.ico $(NFS_BOA_WWWDIR)

clean:
	rm *.cgi

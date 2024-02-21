CROSS_COMPILE = arm-linux-
#CC=$(CROSS_COMPILE)gcc
CC=gcc

AFLAGS = -Wall -g
LDFLAGS = -lsqlite3

NFS_BOA_WWWDIR = /boa/www/
NFS_BOA_CGIDIR = /boa/cgi-bin/

OBJS = cgi_refresh.cgi cgi_led.cgi cgi_buzzer.cgi login.cgi

all: $(OBJS)

%.cgi: %.c
	$(CC) -o $@ $< $(LDFLAGS)

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

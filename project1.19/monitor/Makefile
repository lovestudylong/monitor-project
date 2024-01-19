CROSS_COMPILE=arm-linux-
CC=$(CROSS_COMPILE)gcc
CFLAGS= -c -g
LDFLAGS= -lpthread

OBJS=main.o common.o pthread_led.o pthread_refresh.o pthread_client_request.o

mymonitor_obj: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	mv *.o ./obj
$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

install:
	sudo cp mymonitor_obj /opt/4412/rootfs

.PHONY:clean
clean:
	rm ./obj/*.o mymonitor_obj
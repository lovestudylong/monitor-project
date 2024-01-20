#ifndef __COMMON_H_
#define __COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <termios.h>
#include <syscall.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>

#define LED_DEV "/dev/leddev"
#define ADC_DEV "/dev/adcdev"

unsigned char cmd_led;

struct monitor_info {
    unsigned int adc;
    short gyrox;
    short gyroy;
    short gyroz;
    short aacx;
    short aacy;
    short aacz;
};

extern void *pthread_led(void *arg);
extern void *pthread_refresh(void *arg);
extern void *pthread_client_request(void *arg);
extern void *pthread_transfer(void *arg);

struct msg {
    long msgtype;
    unsigned char text[1];
};

#endif
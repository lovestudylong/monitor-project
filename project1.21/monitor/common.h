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
#define PWM_DEV "/dev/pwmdev"

extern int msgid;
extern int shmid;
extern int semid;

extern key_t msg_key;
extern key_t shm_key;
extern key_t sem_key;

extern unsigned char cmd_led;
extern unsigned char cmd_pwm;

extern pthread_mutex_t mutex_led,
                       mutex_pwm;

extern pthread_cond_t cond_led,
                      cond_pwm;

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
extern void *pthread_buzzer(void *arg);

struct msg {
    long msgtype;
    unsigned char text[1];
};

#endif
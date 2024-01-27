/*
 *  monitor/pthread_buzzer.c
 *
 *  (C) 2024  Chen Zichao
 */

#include "common.h"
#include "../drv/pwmdrv.h"

extern pthread_mutex_t mutex_pwm;

extern pthread_cond_t cond_pwm;

void *pthread_buzzer(void *arg)
{
    int fd = -1;
    int i;

    fd = open(PWM_DEV, O_RDWR);
    if (fd < 0) {
        printf("open %s faipwm\n", PWM_DEV);
        return (void *)-1;
    }

    printf("pthread_pwm is ready\n");

    while (1) {
        pthread_mutex_lock(&mutex_pwm);
        pthread_cond_wait(&cond_pwm, &mutex_pwm);
        if ((cmd_pwm & 1) == 1) { // 打开蜂鸣器
            ioctl(fd, MY_PWM_ON, 1);
        } else if ((cmd_pwm & 1) == 0) { // 关闭蜂鸣器
            ioctl(fd, MY_PWM_OFF, 1);
        }
        pthread_mutex_unlock(&mutex_pwm);
    }

    close(fd);
}
/*
 *  monitor/common.c
 *
 *  (C) 2024  Chen Zichao
 */

#include "common.h"

pthread_mutex_t mutex_led,
                mutex_pwm;

pthread_cond_t cond_led,
               cond_pwm;

int msgid;
int shmid;
int semid;

key_t msg_key;
key_t shm_key;
key_t sem_key;

unsigned char cmd_led;
unsigned char cmd_pwm;

struct monitor_info *env_data_buf;
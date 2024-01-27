/*
 *  monitor/pthread_refresh.c
 *
 *  (C) 2024  Chen Zichao
 */

#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include "common.h"
#include "sem.h"
#include "../drv/mpu6050.h"

#define SEM_NUM 1
#define MEMORY_SIZE 1024

extern int shmid;
extern int semid;

extern key_t shm_key;
extern key_t sem_key;

extern struct monitor_info *env_data_buf;

union semun {
    int val;
};

void *pthread_refresh(void *arg)
{
    int adc_fd = -1;
    int mpu_fd = -1;
    unsigned int adcValue;
    union mpu6050_data data;

    adc_fd = open(ADC_DEV, O_RDONLY);
    if (adc_fd < 0) {
        printf("open %s failed\n", ADC_DEV);
        return (void *)-1;
    }

    mpu_fd = open(MPU_DEV, O_RDONLY);
    if (mpu_fd < 0) {
        printf("open %s failed\n", MPU_DEV);
        return (void *)-1;
    }

    sem_key = ftok("/tmp", 100);
    if (sem_key < 0) {
        perror("ftok");
        exit(-1);
    }

    semid = semget(sem_key, SEM_NUM, IPC_CREAT | 0666);
    if (semid < 0) {
        perror("semget");
        exit(-1);
    }

    union semun mysem;
    mysem.val = 1;
    if (semctl(semid, 0, SETVAL, mysem) < 0) {
        perror("semctl");
        exit(-1);
    }

    shm_key = ftok("/tmp", 100);
    if (shm_key < 0) {
        perror("ftok");
        exit(-1);
    }

    shmid = shmget(shm_key, MEMORY_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(-1);
    }

    env_data_buf = (struct monitor_info *)shmat(shmid, NULL, 0);
    if (env_data_buf < 0) {
        perror("shmat");
        exit(-1);
    }

    printf("pthread_refresh is ready\n");

    bzero(env_data_buf, sizeof(struct monitor_info));
    while (1) {
        sem_p(semid, 0);
        read(adc_fd, &adcValue, 2);
        adcValue = adcValue * 0.4396;
        printf("adcValue = %d\n", adcValue);
        env_data_buf->adc = adcValue;

        int temp;

		ioctl(mpu_fd, GET_GYRO, &data);
		temp = data.gyro.x;
		temp = temp / 16.4;
        env_data_buf->gyrox = temp;
        printf("gyrox = %d\n", temp);

		temp = data.gyro.y;
		temp = temp / 16.4;
		env_data_buf->gyroy = temp;
        printf("gyroy = %d\n", temp);

		temp = data.gyro.z;
		temp = temp / 16.4;
		env_data_buf->gyroz = temp;
        printf("gyroz = %d\n", temp);

        ioctl(mpu_fd, GET_ACCEL, &data);
		temp = data.accel.x;
		temp = temp / 16384;
        printf("accelx = %d\n", temp);

		env_data_buf->aacx = temp;
		temp = data.accel.y;
		temp = temp / 16384;
        printf("accely = %d\n", temp);

		env_data_buf->aacy = temp;
		temp = data.accel.z;
		temp = temp / 16384;
		env_data_buf->aacz = temp;
        printf("accelz = %d\n", temp);

		ioctl(mpu_fd, GET_TEMP, &data);
		temp = data.temp;
		temp = temp / 340 + 36.53;
        env_data_buf->temp = temp;
        printf("temp = %d°C\n", temp);
        sleep(1);
        sem_v(semid, 0);
    }
}
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include "common.h"
#include "sem.h"

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
        env_data_buf->adc = 0;
        env_data_buf->gyrox = 1;
        env_data_buf->gyroy = 2;
        env_data_buf->gyroz = 3;
        env_data_buf->aacx = 4;
        env_data_buf->aacy = 5;
        env_data_buf->aacz = 6;
        sleep(1);
        sem_v(semid, 0);
    }
}
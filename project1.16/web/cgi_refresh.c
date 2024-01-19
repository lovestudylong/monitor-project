#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include "sem.h"

#define SEM_NUM 1
#define MEMORY_SIZE 1024

struct monitor_info {
    float adc;
    short gyrox;
    short gyroy;
    short gyroz;
    short aacx;
    short aacy;
    short aacz;
};

int main(int argc, const char *argv[])
{
    key_t shm_key, sem_key;
    int shmid, semid;
    struct monitor_info *env_data_buf;

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

    sem_p(semid, 0);

    printf("Content-type: text/html;charset=utf-8\n\n");
    
	printf("<head><meta http-equiv=\"refresh\" content=\"1\"></style> </head>");
	printf("<html>\n");
    printf("<script>function show(){var date =new Date(); var now = \"\"; now = date.getFullYear()+\"年\"; now = now + (date.getMonth()+1)+\"月\"; \ now = now + date.getDate()+\"日\"; now = now + date.getHours()+\"时\"; now = now + date.getMinutes()+\"分\";now = now + date.getSeconds()+\"秒\"; document.getElementById(\"nowDiv\").innerHTML = now; setTimeout(\"show()\",1000);} </script>\n");
    printf("<p align=center><body onload=\"show()\"> <div id=\"nowDiv\"></div></p>\n");

    printf("<p>adc:%f</p>\n", env_data_buf->adc);
    printf("<p>gyrox:%d</p>\n", env_data_buf->gyrox);
    printf("<p>gyroy:%d</p>\n", env_data_buf->gyroy);
    printf("<p>gyroz:%d</p>\n", env_data_buf->gyroz);
    printf("<p>aacx:%d</p>\n", env_data_buf->aacx);
    printf("<p>aacy:%d</p>\n", env_data_buf->aacy);
    printf("<p>aacz:%d</p>\n", env_data_buf->aacz);

	printf("</html>\n");

    sem_v(semid, 0);

    return 0;
}
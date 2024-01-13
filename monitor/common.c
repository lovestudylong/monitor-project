#include "common.h"

pthread_mutex_t mutex_led;

pthread_cond_t cond_led;

int msgid;
int shmid;
int semid;

key_t msg_key;
key_t shm_key;
key_t sem_key;


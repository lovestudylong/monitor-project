#include "common.h"

void release_pthread_resource(int signo);

extern pthread_mutex_t mutex_led;

extern pthread_cond_t cond_led;

extern int msgid;
extern int shmid;
extern int semid;

pthread_t id_led,
          id_refresh,
          id_client_request;

int main(int argc, const char *argv[])
{
    signal(SIGINT, release_pthread_resource);

    pthread_mutex_init(&mutex_led, NULL);

    pthread_cond_init(&cond_led, NULL);

    pthread_create(&id_led, NULL, pthread_led, NULL);
    pthread_create(&id_refresh, NULL, pthread_refresh, NULL);
    pthread_create(&id_client_request, NULL, pthread_client_request, NULL);

    pthread_join(id_led, NULL);
    pthread_join(id_refresh, NULL);
    pthread_join(id_client_request, NULL);

    return 0;
}

void release_pthread_resource(int signo)
{
    pthread_mutex_destroy(&mutex_led);

    pthread_cond_destroy(&cond_led);

    pthread_detach(id_led);
    pthread_detach(id_refresh);
    pthread_detach(id_client_request);

    printf("all pthread is detached\n");

    msgctl(msgid, IPC_RMID, NULL);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 1, IPC_RMID, NULL);

    exit(0);
}
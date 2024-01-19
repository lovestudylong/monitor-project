#include "common.h"

extern int msgid;
extern key_t msg_key;

extern pthread_mutex_t mutex_led;

extern pthread_cond_t cond_led;

extern unsigned char cmd_led;

struct msg msgbuf;

#define MSGLEN (sizeof(struct msg) - sizeof(long))

void *pthread_client_request(void *arg)
{
    msg_key = ftok("/tmp", 100);
    if (msg_key < 0) {
        perror("ftok");
        exit(-1);
    }

    msgid = msgget(msg_key, IPC_CREAT | 0666);
    if (msgid < 0) {
        perror("msgget");
        exit(-1);
    }

    printf("pthread_client_request is ready\n");

    while (1) {
        bzero(&msgbuf, sizeof(msgbuf));
        msgrcv(msgid, &msgbuf, MSGLEN, 0, 0);
        usleep(5);
        printf("receive msgtype = %ld, text = %#x\n", msgbuf.msgtype, msgbuf.text[0]);

        switch (msgbuf.msgtype) {
            case 1L: // LED
                pthread_mutex_lock(&mutex_led);
                printf("led request\n");
                cmd_led = msgbuf.text[0];
                pthread_mutex_unlock(&mutex_led);
                pthread_cond_signal(&cond_led);
                break;
            case 2L:
                break;
            default:
                break;
        }
    }
}
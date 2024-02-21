/*
 *  web/cgi_buzzer.c
 *
 *  (C) 2024  Chen Zichao
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/limits.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <strings.h>

struct msg {
    long msgtype;
    unsigned char text[1];
};

#define MSGLEN (sizeof(struct msg) - sizeof(long))

int main(int argc, const char *argv[])
{
    key_t msg_key;
    int msgid;
    struct msg msgbuf;
    int buzzer_state;
    char *data;

    printf("Content-type: text/html;charset=utf-8\n\n");
    printf("<html>\n");
    printf("<head><title>request</title></head>\n"); 

    data = getenv("QUERY_STRING");

    if(sscanf(data, "buzzer_state=%d", &buzzer_state) != 1) {
        printf("<h1>Error!</h1>");
        printf("<p>please input correct<p>");
        return -1;
    }

    printf("<h1>Success!</h1>");
    printf("<p>buzzer_state = %d</p>", buzzer_state);

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

    bzero(&msgbuf, sizeof(msgbuf));

    msgbuf.msgtype = 2L; // BUZZER
    msgbuf.text[0] = buzzer_state;
    msgsnd(msgid, &msgbuf, MSGLEN, 0);

    printf("</html>\n");

    return 0;
}
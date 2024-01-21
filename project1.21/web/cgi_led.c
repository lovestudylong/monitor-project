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
    int led_num, led_state;
    char *data;

    printf("Content-type: text/html;charset=utf-8\n\n");
    printf("<html>\n");
    printf("<head><title>request</title></head>\n"); 

    data = getenv("QUERY_STRING");

    if(sscanf(data, "led_num=%d&led_state=%d", &led_num, &led_state) != 2) {
        printf("<h1>Error!</h1>");
        printf("<p>please input correct<p>");
        return -1;
    }

    if (led_num < 2 || led_num > 5) {
        printf("<h1>Error!</h1>");
        printf("<p>Please input 2 <= led_num <= 5!</p>");
        return -1;
    }

    if (led_state < -1 || led_state > 2) {
        printf("<h1>Error!</h1>");
        printf("<p>Please input 0 <= led_state <= 1!</p>");
        return -1;
    }

    printf("<h1>Success!</h1>");
    printf("<p>led_num = %d, led_state = %d</p>", led_num, led_state);

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

    msgbuf.msgtype = 1L; // LED
    msgbuf.text[0] = (1 << led_num) | (led_state << 0);
    msgsnd(msgid, &msgbuf, MSGLEN, 0);

    printf("</html>\n");

    return 0;
}
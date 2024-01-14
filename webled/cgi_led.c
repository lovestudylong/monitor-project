#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/limits.h>
#include <errno.h>

#include "leddrv.h"

#define LED_DEVICE "/dev/leddev"

int main(int argc, const char *argv[])
{
    int led_num, led_state;
    int fd = -1;
    char *data;
    int i;

    fd = open(LED_DEVICE, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\n", argv[1]);
        return -1;
    }

    printf("Content-type: text/html;charset=utf-8\n\n");
    printf("<html>\n");
    printf("<head><title>webled</title></head>\n");

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

    if (led_state == 0) {
        ioctl(fd, MY_LED_OFF, led_num);
    } else if (led_state == 1) {
        ioctl(fd, MY_LED_ON, led_num);
    } else if (led_state == 2) {
        while (1) {
            for (i = 2; i <= 5; i++) {
                led_num = i;
                led_state = 0;
                ioctl(fd, MY_LED_OFF, led_num);
                usleep(500000);
                led_state = 1;
                ioctl(fd, MY_LED_ON, led_num);
                usleep(500000);
            }
        }
    }

    close(fd);
    printf("</html>\n");

    return 0;
}
#include "common.h"
#include "../drv/leddrv.h"

extern pthread_mutex_t mutex_led;

extern pthread_cond_t cond_led;

#define LED_DEVICE "/dev/leddev"

void *pthread_led(void *arg)
{
    int fd = -1;
    int i;

    fd = open(LED_DEVICE, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\n", LED_DEVICE);
        return (void *)-1;
    }

    printf("pthread_led is ready\n");

    while (1) {
        pthread_mutex_lock(&mutex_led);
        pthread_cond_wait(&cond_led, &mutex_led);
        if ((cmd_led & 1) == 1) { // 开灯操作
            for (i = 2; i <= 5; i++) {
                if (((cmd_led >> i) & 1) == 1) {
                    ioctl(fd, MY_LED_ON, i);
                }
            }
        } else if ((cmd_led & 1) == 0) { // 关灯操作
            for (i = 2; i <= 5; i++) {
                if (((cmd_led >> i) & 1) == 1) {
                    ioctl(fd, MY_LED_OFF, i);
                }
            }
        }
        pthread_mutex_unlock(&mutex_led);
    }

    close(fd);
}
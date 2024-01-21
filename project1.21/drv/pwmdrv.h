#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H

#define PWM_DEV_MAGIC 'g'

#define MY_PWM_OFF _IO(PWM_DEV_MAGIC, 0)
#define MY_PWM_ON  _IO(PWM_DEV_MAGIC, 1)

#endif
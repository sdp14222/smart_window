#ifndef __SERVO_H__
#define __SERVO_H__

#include <wiringPi.h>
#include <stdio.h>
#include <softPwm.h>
#include <stdlib.h>

#define SERVO 12

void servo_init(void);
void servo(void);

#endif

#ifndef __SERVO_H__
#define __SERVO_H__

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

#define SERVO_PIN 12 // bcm pin num

void servo_init(void);
void servo(void);

#endif

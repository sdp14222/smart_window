#ifndef __DCMOTOR_H__
#define __DCMOTOR_H__

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

#define DCMOTOR_PIN 13 // BCM pin num

void dcmotor_init(void);
void dcmotor();

#endif

#ifndef __DOOR_H__
#define __DOOR_H__

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

#define DR_PIN 12 // bcm pin num
/*************************************************************
 * PWM = 19.2MHz(Raspberry pi default) / (divisor * range)
 *
 * ex)
 * 50Hz = 19.2MHz / (384 * 1000)
 *************************************************************/

#define DR_CLOSE	25
#define DR_OPEN		125

#define DR_DEFAULT	DR_CLOSE 

void dr_init(void);

inline void dr_init_run(void)
{
	pwmWrite(DR_PIN, DR_DEFAULT);
}

int dr(int open);

#endif

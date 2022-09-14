#ifndef __DCMOTOR_H__
#define __DCMOTOR_H__

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

#define DCMOTOR_PIN 	13	// BCM pin num

/*************************************************
 * default range : 0 ~ 1023
 * current range : 0 ~ 1000 -> servo pwm settings
 *************************************************/
#define MAX_RANGE	1000
#define DEFAULT_RANGE	500

void dcmotor_init(void);
inline void dcmotor_init_run(void)
{
	pwmWrite(DCMOTOR_PIN, DEFAULT_RANGE);
}

int dcmotor(int range);

#endif

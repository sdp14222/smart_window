#ifndef __FANMOTOR_H__
#define __FANMOTOR_H__

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

#define FM_PIN 	13	// BCM pin num

/*************************************************
 * default range : 0 ~ 1023
 * current range : 0 ~ 1000 -> servo pwm settings
 *************************************************/
#define MAX_RANGE	1000
#define DEFAULT_RANGE	500

void fm_init(void);
inline void fm_init_run(void)
{
	pwmWrite(FM_PIN, DEFAULT_RANGE);
}

int fm(int range);

#endif

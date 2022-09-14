#ifndef __SERVO_H__
#define __SERVO_H__

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

#define SERVO_PIN 12 // bcm pin num
/*************************************************************
 * PWM = 19.2MHz(Raspberry pi default) / (divisor * range)
 *
 * ex)
 * 50Hz = 19.2MHz / (384 * 1000)
 *************************************************************/

#define DR_CLOSE	25
#define DR_OPEN		125

#define DR_DEFAULT	DR_CLOSE 

void servo_init(void);

inline void servo_init_run(void)
{
	pwmWrite(SERVO_PIN, DR_DEFAULT);
}

int servo(int open);

#endif

#include "dcmotor.h"

void dcmotor_init(void)
{
	pinMode(DCMOTOR_PIN, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS); // Mark-Space mode
}

void dcmotor(void)
{
	/*
	 * default range : 0 ~ 1023
   	 */
	pwmWrite(DCMOTOR_PIN, 500);
	delay(1000);	
	pwmWrite(DCMOTOR_PIN, 0);
	delay(1000);	
}

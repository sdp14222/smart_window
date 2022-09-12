#include "servo.h"

void servo_init(void)
{
	pinMode(SERVO_PIN, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS); // Mark-Space mode

	/*
	 * PWM = 19.2MHz(Raspberry pi default) / (divisor * range)
	 * 
	 * ex)
	 * 50Hz = 19.2MHz / (384 * 1000)
	 */
	pwmSetClock(384);
	pwmSetRange(1000);
}

void servo(void)
{
	pwmWrite(SERVO_PIN, 25);
	delay(1000);	
	pwmWrite(SERVO_PIN, 125);
	delay(1000);	
}

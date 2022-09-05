#include "servo.h"

void servo_init(void)
{
	pinMode(SERVO, OUTPUT);
	softPwmCreate(SERVO, 0, 200);
}

void servo(void)
{
	softPwmWrite(SERVO, 5);
	delay(1000);	
	softPwmWrite(SERVO, 25);
	delay(1000);	
}

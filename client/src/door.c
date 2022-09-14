#include "door.h"

void dr_init(void)
{
	pinMode(DR_PIN, PWM_OUTPUT);
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

int dr(int open)
{
	static int isOpened = DR_DEFAULT;

	switch(open)
	{
		case -1 :
			return isOpened;
		case DR_CLOSE :
			isOpened = DR_CLOSE;
		case DR_OPEN  :
			isOpened = DR_OPEN;
			pwmWrite(DR_PIN, isOpened);
			return isOpened;
		default :
			return -1; // error
	}
}

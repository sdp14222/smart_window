#include "dcmotor.h"

void dcmotor_init(void)
{
	pinMode(DCMOTOR_PIN, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS); // Mark-Space mode
}

#if 0
inline void dcmotor_init_run(void)
{
	pwmWrite(DCMOTOR_PIN, DEFAULT_RANGE);
}
#endif

int dcmotor(int range)
{
	/*************************************************
 	 * default range : 0 ~ 1023
 	 * current range : 0 ~ 1000 -> servo pwm settings
 	 *************************************************/
	static int cur_range = DEFAULT_RANGE;

	if(range < -1 || range > MAX_RANGE)
	{
		return -1; // error
	}

	switch(range)
	{
		case -1 :
			return cur_range;
		default :
			cur_range = range;
			pwmWrite(DCMOTOR_PIN, cur_range);
			return cur_range;
	}
}

 /**************************************************
  2  * Created  : 2022.08.31
  3  * Modified : 2022.09.21
  4  * Author   : SangDon Park
  5  **************************************************/
#include "fanmotor.h"

void fm_init(void)
{
	pinMode(FM_PIN, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS); // Mark-Space mode
}

int fm(int range)
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
			pwmWrite(FM_PIN, cur_range);
			return cur_range;
	}
}

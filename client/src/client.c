#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <stdlib.h>
#include "dcmotor.h"
#include "servo.h"

int main(void)
{
    wiringPiSetupGpio();
#if 0
    dcmotor_init();
    while(1)
    {
        dcmotor();
    }
#endif
    servo_init();
    while(1)
    {
	servo();
    }
    
    return 0;
}

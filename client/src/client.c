#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <stdlib.h>
#include "dcmotor.h"
#include "servo.h"
#include "dht11.h"

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

#if 0
    servo_init();
    while(1)
    {
	servo();
    }
#endif

#if 1
    while(1)
    {
	    read_dht11_dat();
	    delay(2000);
    }
#endif
    
    return 0;
}

#include <wiringPi.h>
#include <stdio.h>
#include <softPwm.h>
#include <stdlib.h>

#define SERVO 12

int main(void)
{
	wiringPiSetupGpio();
	pinMode(SERVO, OUTPUT);
	softPwmCreate(SERVO, 0, 200);

	while(1) {

		softPwmWrite(SERVO, 5);
		delay(1000);	

		softPwmWrite(SERVO, 25);
		delay(1000);	
	}
	return 0;
}

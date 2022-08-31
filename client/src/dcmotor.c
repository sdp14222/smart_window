#include <wiringPi.h>
#include <stdio.h>
#include <softPwm.h>
#include <stdlib.h>

#define FOREWARD  13

int main(void)
{
	wiringPiSetupGpio();
	pinMode(FOREWARD, OUTPUT);
	softPwmCreate(FOREWARD, 0, 200);

	while(1) {
		softPwmWrite(FOREWARD, 100);
		printf("forward run \n");

		delay(1000);	
		softPwmWrite(FOREWARD, 0);
		printf("---------------\n");
		delay(1000);	
	}
	return 0;
}

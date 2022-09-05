#include "dcmotor.h"

#define FOREWARD  13

void dcmotor_init(void)
{
	pinMode(FOREWARD, OUTPUT);
	softPwmCreate(FOREWARD, 0, 200);
}

void dcmotor(void)
{
    softPwmWrite(FOREWARD, 100);
    printf("forward run \n");

    delay(1000);	
    softPwmWrite(FOREWARD, 0);
    printf("---------------\n");
    delay(1000);	
}

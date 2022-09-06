#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <stdlib.h>
#include "dcmotor.h"
#include "servo.h"
#include "dht11.h"
#include "Character_LCD.h"

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

#if 0
    while(1)
    {
	    read_dht11_dat();
	    delay(2000);
    }
#endif

#if 1
    /*
     * Character_LCD test
     */
    char str[] = "Hello World!!";
    CLCD_Init();

    CLCD_Write(CLCD_ADDR_SET, 0, 0, str);
    CLCD_Write(CLCD_ADDR_SET, 1, 0, str);

    printf("delay...\n");
    delay(3000);

    printf("Hoee\n");
    CLCD_Write(CLCD_ADDR_SET, 0, 0, "Hoee..");

    printf("delay...\n");
    delay(3000);

    unsigned int cnt = 0;
    char str1[16];

    printf("CLCD_Clear_Display()\n");
    CLCD_Clear_Display();

    while(1)
    {
	    printf("delay...\n");
	    delay(1000);

	    printf("cnt = %d\n", cnt);
	    sprintf(str1, "cnt = %d", cnt);
	    CLCD_Write(CLCD_ADDR_SET, 1, 0, str1);
	    cnt++;

	    printf("delay...\n");
	    delay(1000);

	    printf("CLCD_Return_Home()\n");
	    CLCD_Return_Home();
    }

#endif
    
    return 0;
}

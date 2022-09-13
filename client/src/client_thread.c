#include "client.h"

void * dcmotor_thread(void *arg)
{
	//dcmotor_init();
	while(1)
	{
		dcmotor();
	}
}

void * servo_thread(void *arg)
{
	//servo_init();
	while(1)
	{
		servo();
	}
}


void * dht11_thread(void *arg)
{
	while(1)
	{
		read_dht11_dat();
		delay(2000);
	}
}

void * Character_LCD_thread(void *arg)
{
	//char str1[] = "Hello";
	//CLCD_Init();

	CLCD_Write(CLCD_ADDR_SET, 0, 0, "Hello");
	CLCD_Write(CLCD_ADDR_SET, 1, 0, "Smart Window");

	printf("delay...\n");
	delay(3000);

#if 0
	printf("Hoee\n");
	CLCD_Write(CLCD_ADDR_SET, 0, 0, "Hoee..");

	printf("delay...\n");
	delay(3000);

	unsigned int cnt = 0;
	char str1[16];
#endif

	printf("CLCD_Clear_Display()\n");
	CLCD_Clear_Display();

	char str0[16];

	while(1)
	{
#if 0
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
#endif
		//pthread_mutex_lock(&mutex);
		sprintf(str0, "H:%2d.%1d\% T:%2d.%1dC",
			 dht11_sdata[0], dht11_sdata[1], dht11_sdata[2], dht11_sdata[3]);
		//pthread_mutex_unlock(&mutex);
		CLCD_Write(CLCD_ADDR_SET, 0, 0, str0);
	}
}

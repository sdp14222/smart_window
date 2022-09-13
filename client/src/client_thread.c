#include "client.h"
#if 0
extern int dht11_dat[5];
int dht11_sdata[4];
#endif

void * dcmotor_thread(void *arg)
{
	//dcmotor_init();
	while(1)
	{
		dcmotor();
	}
	return NULL;
}

void * servo_thread(void *arg)
{
	//servo_init();
	while(1)
	{
		servo();
	}
	return NULL;
}


void * dht11_thread(void *arg)
{
	int ret_data[5] = {0};
	char str0[16];
	time_t t;
	struct tm s_tm;

	delay(2000);

	while(1)
	{
		if(read_dht11_dat(ret_data))
		{
			pthread_mutex_lock(&mutex);
			sprintf(str0, "H:%2d.%1d%c T:%2d.%1dC",
				 ret_data[0], ret_data[1], '%', ret_data[2], ret_data[3]);
			pthread_mutex_unlock(&mutex);
			CLCD_Write(CLCD_ADDR_SET, 0, 0, str0);
		}
		delay(2000);

		t = time(NULL);
		s_tm = *localtime(&t);
		printf("%d %d %d %d %d %d ",
				s_tm.tm_year + 1900,
				s_tm.tm_mon + 1,
				s_tm.tm_mday,
				s_tm.tm_hour,
				s_tm.tm_min,
				s_tm.tm_sec);
	}
	return NULL;
}

void * Character_LCD_init_thread(void *arg)
{
	//char str1[] = "Hello";
	//CLCD_Init();

	CLCD_Write(CLCD_ADDR_SET, 0, 0, "Hello");
	CLCD_Write(CLCD_ADDR_SET, 1, 0, "Smart Window");

	printf("delay...\n");
	delay(1000);

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

#if 0
	char str0[16];

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
		pthread_mutex_lock(&mutex);
		sprintf(str0, "H:%2d.%1d%c T:%2d.%1dC",
			 dht11_sdata[0], dht11_sdata[1], '%',dht11_sdata[2], dht11_sdata[3]);
		pthread_mutex_unlock(&mutex);
		CLCD_Write(CLCD_ADDR_SET, 0, 0, str0);
	}
#endif
	return NULL;
}

#if 0
void * peek_data_thread(void *arg)
{
	/*
	 * dht11 data check
	 */
	while(1)
	{
		if(dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF))
		{
			pthread_mutex_lock(&mutex);
			dht11_sdata[0] = dht11_dat[0];
			dht11_sdata[1] = dht11_dat[1];
			dht11_sdata[2] = dht11_dat[2];
			dht11_sdata[3] = dht11_dat[3];
			pthread_mutex_unlock(&mutex);
		}
		else
		{

		}
		delay(10);
	}
	return NULL;
}
#endif

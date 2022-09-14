#include "client.h"

extern struct smart_window_send_data	s_data;
static struct tm* current_time(void);

void * dcmotor_thread(void *arg)
{
	struct tm* s_tm;
	int fan_speed;

	int fm_cnt = s_data.fmv.fm_cnt = 0;
	s_data.fmv.did = DID_FM;

	dcmotor_init_run();

	while(1)
	{
#if 0
		if(cnt <= 5)
		{
			ret = dcmotor(-1);
			printf("dc ret : %d\n", ret);
		}
		else if(cnt > 5 && cnt <= 10)
		{
			ret = dcmotor(200);
			printf("dc ret : %d\n", ret);
		}
		else if(cnt > 10 && cnt <= 15)
		{
			ret = dcmotor(800);
			printf("dc ret : %d\n", ret);
		}
		else
		{
			ret = dcmotor(0);
			printf("dc ret : %d\n", ret);
		}
		cnt++;
#endif

		if(fm_cnt == 5)
		{
			fan_speed = dcmotor(0);
		}
		else
		{
			fan_speed = dcmotor(-1);
		}
		s_data.fmv.fm_dat[fm_cnt].speed  = fan_speed;

		s_tm = current_time();
		s_data.fmv.fm_dat[fm_cnt].t.year = s_tm->tm_year + 1900;
		s_data.fmv.fm_dat[fm_cnt].t.mon  = s_tm->tm_mon  + 1;
		s_data.fmv.fm_dat[fm_cnt].t.day  = s_tm->tm_mday;
		s_data.fmv.fm_dat[fm_cnt].t.hour = s_tm->tm_hour;
		s_data.fmv.fm_dat[fm_cnt].t.min  = s_tm->tm_min;
		s_data.fmv.fm_dat[fm_cnt].t.sec  = s_tm->tm_sec;

#if 0
		printf("dc did   : %d\n", s_data.fmv.did);
		printf("dc cnt   : %d\n", s_data.fmv.fm_cnt);
		printf("dc speed : %d\n", s_data.fmv.fm_dat[fm_cnt].speed);
		printf("dc year  : %d\n", s_data.fmv.fm_dat[fm_cnt].t.year);
		printf("dc mon   : %d\n", s_data.fmv.fm_dat[fm_cnt].t.mon);
		printf("dc mday  : %d\n", s_data.fmv.fm_dat[fm_cnt].t.day);
		printf("dc hour  : %d\n", s_data.fmv.fm_dat[fm_cnt].t.hour);
		printf("dc min   : %d\n", s_data.fmv.fm_dat[fm_cnt].t.min);
		printf("dc sec   : %d\n\n", s_data.fmv.fm_dat[fm_cnt].t.sec);
#endif
		fm_cnt = ++s_data.fmv.fm_cnt;

		delay(1000);
	}
	return NULL;
}

void * servo_thread(void *arg)
{
	struct tm* s_tm;

	int dr_cnt = s_data.drv.dr_cnt = 0;
	//servo_init();
	while(1)
	{
		servo();

		s_tm = current_time();
		s_data.drv.dr_dat[dr_cnt].t.year = s_tm->tm_year + 1900;
		s_data.drv.dr_dat[dr_cnt].t.mon  = s_tm->tm_mon + 1;
		s_data.drv.dr_dat[dr_cnt].t.day  = s_tm->tm_mday;
		s_data.drv.dr_dat[dr_cnt].t.hour = s_tm->tm_hour;
		s_data.drv.dr_dat[dr_cnt].t.min  = s_tm->tm_min;
		s_data.drv.dr_dat[dr_cnt].t.sec  = s_tm->tm_sec;
#if 0
		printf("servo year : %d\n", s_tm->tm_year + 1900);
		printf("servo mon  : %d\n", s_tm->tm_mon + 1);
		printf("servo mday : %d\n", s_tm->tm_mday);
		printf("servo hour : %d\n", s_tm->tm_hour);
		printf("servo min  : %d\n", s_tm->tm_min);
		printf("servo sec  : %d\n\n", s_tm->tm_sec);
#endif
		dr_cnt = ++s_data.drv.dr_cnt;
		delay(1000);
	}
	return NULL;
}


void * dht11_thread(void *arg)
{
	int ret_data[5] = {0};
	char str0[16];
	struct tm* s_tm;

	int ht_cnt = s_data.htv.ht_cnt = 0;
	s_data.htv.did = DID_HT;

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

			s_tm = current_time();
#if 0
			printf("year : %d\n", s_tm->tm_year + 1900);
			printf("mon  : %d\n", s_tm->tm_mon + 1);
			printf("mday : %d\n", s_tm->tm_mday);
			printf("hour : %d\n", s_tm->tm_hour);
			printf("min  : %d\n", s_tm->tm_min);
			printf("sec  : %d\n\n", s_tm->tm_sec);
#endif
			s_data.htv.ht_dat[ht_cnt].h_int = ret_data[0];
			s_data.htv.ht_dat[ht_cnt].h_flt = ret_data[1];
			s_data.htv.ht_dat[ht_cnt].t_int = ret_data[2];
			s_data.htv.ht_dat[ht_cnt].t_flt = ret_data[3];
			s_data.htv.ht_dat[ht_cnt].t.year = s_tm->tm_year + 1900;
			s_data.htv.ht_dat[ht_cnt].t.mon  = s_tm->tm_mon + 1;
			s_data.htv.ht_dat[ht_cnt].t.day  = s_tm->tm_mday;
			s_data.htv.ht_dat[ht_cnt].t.hour = s_tm->tm_hour;
			s_data.htv.ht_dat[ht_cnt].t.min  = s_tm->tm_min;
			s_data.htv.ht_dat[ht_cnt].t.sec  = s_tm->tm_sec;

#if 0
			printf("cnt    : %d\n", s_data.htv.ht_cnt);
			printf("t int  : %d\n", s_data.htv.ht_dat[ht_cnt].t_int);
			printf("t hour : %d\n", s_data.htv.ht_dat[ht_cnt].t.hour);
			printf("t min  : %d\n", s_data.htv.ht_dat[ht_cnt].t.min);
			printf("t sec  : %d\n", s_data.htv.ht_dat[ht_cnt].t.sec);
#endif

			ht_cnt = ++s_data.htv.ht_cnt;
		}
		delay(2000);
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

	delay(10000);

	int i;
	
	printf("================================================\n");
	printf("================================================\n");

	printf("================================================\n");
	printf("s_data.htv.ht_cnt : %d\n", s_data.htv.ht_cnt);

	for(i = 0; i < s_data.htv.ht_cnt; i++)
	{
		printf("s_data.htv.ht_dat[%d].h_int : %d\n", i, s_data.htv.ht_dat[i].h_int);
		printf("s_data.htv.ht_dat[%d].h_flt : %d\n", i, s_data.htv.ht_dat[i].h_flt);
		printf("s_data.htv.ht_dat[%d].t_int : %d\n", i, s_data.htv.ht_dat[i].t_int);
		printf("s_data.htv.ht_dat[%d].t_flt : %d\n", i, s_data.htv.ht_dat[i].t_flt);

		printf("s_data.htv.ht_dat[%d].t.year : %d\n", i, s_data.htv.ht_dat[i].t.year);
		printf("s_data.htv.ht_dat[%d].t.mon  : %d\n", i, s_data.htv.ht_dat[i].t.mon);
		printf("s_data.htv.ht_dat[%d].t.day  : %d\n", i, s_data.htv.ht_dat[i].t.day);
		printf("s_data.htv.ht_dat[%d].t.hour : %d\n", i, s_data.htv.ht_dat[i].t.hour);
		printf("s_data.htv.ht_dat[%d].t.min  : %d\n", i, s_data.htv.ht_dat[i].t.min);
		printf("s_data.htv.ht_dat[%d].t.sec  : %d\n", i, s_data.htv.ht_dat[i].t.sec);
		printf("\n");
	}

	printf("================================================\n");

	printf("================================================\n");
	printf("s_data.drv.dr_cnt : %d\n", s_data.drv.dr_cnt);

	for(i = 0; i < s_data.drv.dr_cnt; i++)
	{
		printf("s_data.drv.dr_dat[%d].t.year : %d\n", i, s_data.drv.dr_dat[i].t.year);
		printf("s_data.drv.dr_dat[%d].t.mon  : %d\n", i, s_data.drv.dr_dat[i].t.mon);
		printf("s_data.drv.dr_dat[%d].t.day  : %d\n", i, s_data.drv.dr_dat[i].t.day);
		printf("s_data.drv.dr_dat[%d].t.hour : %d\n", i, s_data.drv.dr_dat[i].t.hour);
		printf("s_data.drv.dr_dat[%d].t.min  : %d\n", i, s_data.drv.dr_dat[i].t.min);
		printf("s_data.drv.dr_dat[%d].t.sec  : %d\n", i, s_data.drv.dr_dat[i].t.sec);
		printf("\n");
	}

	printf("================================================\n");

	printf("================================================\n");
	printf("s_data.fmv.fm_cnt : %d\n", s_data.fmv.fm_cnt);

	for(i = 0; i < s_data.fmv.fm_cnt; i++)
	{
		printf("s_data.fmv.fm_dat[%d].speed : %d\n", i, s_data.fmv.fm_dat[i].speed);

		printf("s_data.fmv.fm_dat[%d].t.year : %d\n", i, s_data.fmv.fm_dat[i].t.year);
		printf("s_data.fmv.fm_dat[%d].t.mon  : %d\n", i, s_data.fmv.fm_dat[i].t.mon);
		printf("s_data.fmv.fm_dat[%d].t.day  : %d\n", i, s_data.fmv.fm_dat[i].t.day);
		printf("s_data.fmv.fm_dat[%d].t.hour : %d\n", i, s_data.fmv.fm_dat[i].t.hour);
		printf("s_data.fmv.fm_dat[%d].t.min  : %d\n", i, s_data.fmv.fm_dat[i].t.min);
		printf("s_data.fmv.fm_dat[%d].t.sec  : %d\n", i, s_data.fmv.fm_dat[i].t.sec);
		printf("\n");
	}

	printf("================================================\n");

	printf("================================================\n");
	printf("================================================\n");


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

static struct tm* current_time(void)
{
	time_t t;
	static struct tm s_tm;

	t = time(NULL);
	s_tm = *localtime(&t);
#if 0
	printf("%d %d %d %d %d %d ",
			s_tm.tm_year + 1900,
			s_tm.tm_mon + 1,
			s_tm.tm_mday,
			s_tm.tm_hour,
			s_tm.tm_min,
			s_tm.tm_sec);
#endif
	return &s_tm;
}

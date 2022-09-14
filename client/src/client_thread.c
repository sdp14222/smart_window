/**********************************************************
 * Created : 2022.09.13 
 * Author  : SangDon Park
 * 
 * Desc    : Client.c threads & functions
 **********************************************************/
#include "client.h"

extern struct smart_window_send_data	s_data;

static struct tm* current_time(void);
static int data_store(did_t did, void *arg);
static int ht_dat_proc(void *arg);
static int dd_dat_proc(void *arg);
static int rw_dat_proc(void *arg);
static int dr_dat_proc(void *arg);
static int fm_dat_proc(void *arg);


static int ht_dat_proc(void *arg)
{
	unsigned int *ht_cnt = &s_data.htv.ht_cnt;
	struct ht_data *htdp = &s_data.htv.ht_dat[*ht_cnt];
	struct tm* s_tm;

	htdp->h_int = ((int *)arg)[0];
	htdp->h_flt = ((int *)arg)[1];
	htdp->t_int = ((int *)arg)[2];
	htdp->t_flt = ((int *)arg)[3];

	s_tm = current_time();
	htdp->t.year = s_tm->tm_year;
	htdp->t.mon  = s_tm->tm_mon;
	htdp->t.day  = s_tm->tm_mday;
	htdp->t.hour = s_tm->tm_hour;
	htdp->t.min  = s_tm->tm_min;
	htdp->t.sec  = s_tm->tm_sec;

	(*ht_cnt)++;

	return 1;
}

static int dd_dat_proc(void *arg)
{
	return 0;
}

static int rw_dat_proc(void *arg)
{
	return 0;
}

static int dr_dat_proc(void *arg)
{
	unsigned int *dr_cnt = &s_data.drv.dr_cnt;
	struct dr_data *drdp = &s_data.drv.dr_dat[*dr_cnt];
	struct tm* s_tm;

	drdp->open = *(int *)arg;

	s_tm = current_time();

	drdp->t.year = s_tm->tm_year;
	drdp->t.mon  = s_tm->tm_mon;
	drdp->t.day  = s_tm->tm_mday;
	drdp->t.hour = s_tm->tm_hour;
	drdp->t.min  = s_tm->tm_min;
	drdp->t.sec  = s_tm->tm_sec;

	(*dr_cnt)++;

	return 1;
}

static int fm_dat_proc(void *arg)
{
	unsigned int *fm_cnt = &s_data.fmv.fm_cnt;
	struct fm_data *fmdp = &s_data.fmv.fm_dat[*fm_cnt];
	struct tm* s_tm;

	fmdp->speed = *(int *)arg;

	s_tm = current_time();

	fmdp->t.year = s_tm->tm_year;
	fmdp->t.mon  = s_tm->tm_mon;
	fmdp->t.day  = s_tm->tm_mday;
	fmdp->t.hour = s_tm->tm_hour;
	fmdp->t.min  = s_tm->tm_min;
	fmdp->t.sec  = s_tm->tm_sec;

	(*fm_cnt)++;

	return 1;
}

static int data_store(did_t did, void *arg)
{
	int (*dat_proc)(void *arg);

	switch(did)
	{
		case DID_HT :
			dat_proc = ht_dat_proc;
			break;
		case DID_DD :
			dat_proc = dd_dat_proc;
			break;
		case DID_RW :
			dat_proc = rw_dat_proc;
			break;
		case DID_DR :
			dat_proc = dr_dat_proc;
			break;
		case DID_FM :
			dat_proc = fm_dat_proc;
			break;
		default :
			return 0;
	}

	return dat_proc(arg);
}

void * fm_thread(void *arg)
{
	int fan_speed;
	int cnt = 0;
	s_data.fmv.did = DID_FM;

	fm_init_run();

	while(1)
	{
		if(cnt == 5)
		{
			fan_speed = fm(0);
		}
		else
		{
			fan_speed = fm(-1);
		}

		data_store(DID_FM, (void *)&fan_speed);

		cnt++;
		delay(1000);
	}
	return NULL;
}

void * dr_thread(void *arg)
{
	int isOpened;
	int cnt = 0;
	s_data.drv.did = DID_DR;

	dr_init_run();

	while(1)
	{
		if(cnt == 5)
		{
			isOpened = dr(DR_OPEN);
		}
		else
		{
			isOpened = dr(-1);
		}

		data_store(DID_DR, (void *)&isOpened);

		cnt++;
		delay(1000);
	}
	return NULL;
}


void * dht11_thread(void *arg)
{
	int ret_data[5] = {0};
	char str0[16];

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

			data_store(DID_HT, (void *)ret_data);
		}
		delay(2000);
	}
	return NULL;
}

void * Character_LCD_init_thread(void *arg)
{
	CLCD_Write(CLCD_ADDR_SET, 0, 0, "Hello");
	CLCD_Write(CLCD_ADDR_SET, 1, 0, "Smart Window");

	printf("delay...\n");
	delay(1000);

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
		printf("s_data.drv.dr_dat[%d].open : %d\n", i, s_data.drv.dr_dat[i].open);

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

	return NULL;
}

static struct tm* current_time(void)
{
	time_t t;
	static struct tm s_tm;

	t    = time(NULL);
	s_tm = *localtime(&t);

	s_tm.tm_year += 1900;
	s_tm.tm_mon  += 1;

	return &s_tm;
}

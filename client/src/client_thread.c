/**********************************************************
 * Created : 2022.09.13 
 * Author  : SangDon Park
 * 
 * Desc    : Client.c threads & functions
 **********************************************************/
#include "client.h"

extern struct smart_window_send_data	s_data;
extern pthread_mutex_t			mutex_arr[DID_TOTAL_CNT];	

static struct tm* current_time(void);
static int data_store(did_t did, void *arg);
static int ht_dat_proc(void *arg);
static int dd_dat_proc(void *arg);
static int rw_dat_proc(void *arg);
static int dr_dat_proc(void *arg);
static int fm_dat_proc(void *arg);

static int ht_dat_proc(void *arg)
{
	pthread_mutex_lock(&mutex_arr[DID_HT]);
#if 0
	uint8_t *ht_cnt = &s_data.htv.ht_cnt;
#endif
	uint8_t *ht_cnt = &s_data.smv.sdi[DID_HT].cnt;
#if 0
	struct ht_data *htdp = &s_data.htv.ht_dat[*ht_cnt];
#endif
	struct ht_data *htdp = &s_data.smv.sd.ht[*ht_cnt];
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
	pthread_mutex_unlock(&mutex_arr[DID_HT]);

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
	pthread_mutex_lock(&mutex_arr[DID_DR]);
#if 0
	uint8_t *dr_cnt = &s_data.drv.dr_cnt;
#endif
	uint8_t *dr_cnt = &s_data.smv.sdi[DID_DR].cnt;
#if 0
	struct dr_data *drdp = &s_data.drv.dr_dat[*dr_cnt];
#endif
	struct dr_data *drdp = &s_data.smv.sd.dr[*dr_cnt];
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
	pthread_mutex_unlock(&mutex_arr[DID_DR]);

	return 1;
}

static int fm_dat_proc(void *arg)
{
	pthread_mutex_lock(&mutex_arr[DID_FM]);
#if 0
	uint8_t *fm_cnt = &s_data.fmv.fm_cnt;
#endif
	uint8_t *fm_cnt = &s_data.smv.sdi[DID_FM].cnt;

	struct fm_data *fmdp = &s_data.smv.sd.fm[*fm_cnt];
	struct tm* s_tm;

	fmdp->speed = *(uint16_t *)arg;

	s_tm = current_time();

	fmdp->t.year = s_tm->tm_year;
	fmdp->t.mon  = s_tm->tm_mon;
	fmdp->t.day  = s_tm->tm_mday;
	fmdp->t.hour = s_tm->tm_hour;
	fmdp->t.min  = s_tm->tm_min;
	fmdp->t.sec  = s_tm->tm_sec;

	(*fm_cnt)++;
	pthread_mutex_unlock(&mutex_arr[DID_FM]);

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
	static int fan_speed;
	int cnt = 0;
	int tmp_speed;
#if 0
	s_data.fmv.did = DID_FM;
#endif
	s_data.smv.sdi[DID_FM].did = DID_FM;

	fm_init_run();
	fan_speed = fm(-1);
	data_store(DID_FM, (void *)&fan_speed);

	while(1)
	{
		if((tmp_speed = fm(-1)) != fan_speed)
		{
			fan_speed = tmp_speed;
			data_store(DID_FM, (void *)&fan_speed);
		}
		else if(cnt == 5)
		{
			fan_speed = fm(0);
			data_store(DID_FM, (void *)&fan_speed);
		}

		cnt++;
		delay(1000);
	}
	return NULL;
}

void * dr_thread(void *arg)
{
	static int isOpened;
	int cnt = 0;
	int tmp_isOpened;
#if 0
	s_data.drv.did = DID_DR;
#endif
	s_data.smv.sdi[DID_DR].did = DID_DR;

	dr_init_run();
	isOpened = dr(-1);
	data_store(DID_DR, (void *)&isOpened);

	while(1)
	{
		if((tmp_isOpened = dr(-1)) != isOpened)
		{
			isOpened = tmp_isOpened;
			data_store(DID_DR, (void *)&isOpened);
		}
		else if(cnt == 5)
		{
			isOpened = dr(DR_OPEN);
			data_store(DID_DR, (void *)&isOpened);
		}

		cnt++;
		delay(1000);
	}
	return NULL;
}


void * dht11_thread(void *arg)
{
	int ret_data[5] = {0};
	char str0[16];

#if 0
	s_data.htv.did = DID_HT;
#endif
	s_data.smv.sdi[DID_HT].did = DID_HT;

	delay(2000);

	while(1)
	{
		if(read_dht11_dat(ret_data))
		{
			sprintf(str0, "H:%2d.%1d%c T:%2d.%1dC",
				 ret_data[0], ret_data[1], '%', ret_data[2], ret_data[3]);

			CLCD_Write(CLCD_ADDR_SET, 0, 0, str0);

			data_store(DID_HT, (void *)ret_data);
		}
		delay(2000);
	}
	return NULL;
}

void * Character_LCD_init_thread(void *arg)
{
	s_data.uid = 1;

	CLCD_Write(CLCD_ADDR_SET, 0, 0, "Hello");
	CLCD_Write(CLCD_ADDR_SET, 1, 0, "Smart Window");

	printf("delay...\n");
	delay(1000);

	printf("CLCD_Clear_Display()\n");
	CLCD_Clear_Display();

	return NULL;
}

void * dd_thread(void *arg)
{
#if 0
	s_data.ddv.did = DID_DD;
#endif
	s_data.smv.sdi[DID_DD].did = DID_DD;
	
	return NULL;
}

void * rw_thread(void *arg)
{
#if 0
	s_data.rwv.did = DID_RW;
#endif
	s_data.smv.sdi[DID_RW].did = DID_RW;

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

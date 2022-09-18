/*
 * Create date : 2022.08.31
 * Author : SangDon Park
 */
#include "client.h"

#define THREAD_CNT ( sizeof(fthread_p) / sizeof(fthread_p[0]) )

static void get_send_data_size(uint16_t *size);
static void send_data_mem_cpy(char **msg_np, uint16_t *size);
static void send_data_mem_cpy_prv(char **msg_np, uint16_t *size);
static void send_data_mem_cpy_htv(char **msg_np);
static void send_data_mem_cpy_ddv(char **msg_np);
static void send_data_mem_cpy_rwv(char **msg_np);
static void send_data_mem_cpy_drv(char **msg_np);
static void send_data_mem_cpy_fmv(char **msg_np);

struct smart_window_send_data	s_data;
pthread_mutex_t 		mutex_arr[DID_TOTAL_CNT];

int main(void)
{
	int sock;
	int i;
	struct sockaddr_in serv_addr;
	void * (*fthread_p[])(void *) = {
		dht11_thread,
		dd_thread,
		rw_thread,
		dr_thread,
		fm_thread,
		Character_LCD_init_thread,
		send_msg,
		recv_msg,
	};
	pthread_t pth_arr[THREAD_CNT]; 
	void * thread_return;

	sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family 		= AF_INET;
	serv_addr.sin_addr.s_addr 	= inet_addr(SERVER_IP);
	serv_addr.sin_port 		= htons(atoi(SERVER_PORT));

	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");

	wiringPiSetupGpio(); // bcm pin num

	fm_init();
    	dr_init();
	CLCD_Init();

	for(i = 0; i < DID_TOTAL_CNT; i++)
	{
		pthread_mutex_init(&mutex_arr[i], NULL);
	}

	for(i = 0; i < THREAD_CNT; i++)
	{
		pthread_create(&pth_arr[i], NULL, fthread_p[i], (void*)&sock);
	}

	for(i = 0; i < THREAD_CNT; i++)
	{
		pthread_join(pth_arr[i], &thread_return);
	}

	for(i = 0; i < DID_TOTAL_CNT; i++)
	{
		pthread_mutex_destroy(&mutex_arr[i]);
	}

	close(sock);

	return 0;
}


static void send_data_mem_cpy_prv(char **msg_np, uint16_t *size)
{
	uint8_t op = 0b0010;
	uint16_t __size = *size;
	uint32_t __uid = s_data.uid;

	// op
	memcpy(*msg_np, &op, sizeof(op));
	*msg_np += sizeof(op);

	// total data size
#if 0
	memcpy(*msg_np, size, sizeof(*size));
	*msg_np += sizeof(*size);
#endif
	__size = htons(__size);
	memcpy(*msg_np, &__size, sizeof(*size));
	*msg_np += sizeof(*size);

	// uid
#if 0
	memcpy(*msg_np, &s_data.uid, sizeof(s_data.uid));
	*msg_np += sizeof(s_data.uid);
#endif
	__uid = htonl(s_data.uid);
	memcpy(*msg_np, &__uid, sizeof(s_data.uid));
	*msg_np += sizeof(s_data.uid);

}

static void send_data_mem_cpy_htv(char **msg_np)
{
	// htv
	pthread_mutex_lock(&mutex_arr[DID_HT]);
#if 0
	memcpy(*msg_np, &s_data.htv.did, sizeof(s_data.htv.did));
	*msg_np += sizeof(s_data.htv.did);

	memcpy(*msg_np, &s_data.htv.ht_cnt, sizeof(s_data.htv.ht_cnt));
	*msg_np += sizeof(s_data.htv.ht_cnt);

	memcpy(*msg_np, s_data.htv.ht_dat, sizeof(struct ht_data) * s_data.htv.ht_cnt);
	*msg_np += sizeof(struct ht_data) * s_data.htv.ht_cnt;
	s_data.htv.ht_cnt = 0;
#endif
	uint8_t cnt = s_data.smv.sdi[DID_HT].cnt;
	uint8_t did = s_data.smv.sdi[DID_HT].did;

	memcpy(*msg_np, &s_data.smv.sdi[DID_HT].did, sizeof(s_data.smv.sdi[DID_HT].did));
	*msg_np += sizeof(s_data.smv.sdi[DID_HT].did);

	memcpy(*msg_np, &s_data.smv.sdi[DID_HT].cnt, sizeof(s_data.smv.sdi[DID_HT].cnt));
	*msg_np += sizeof(s_data.smv.sdi[DID_HT].cnt);

	memcpy(*msg_np, s_data.smv.sd.ht, sizeof(struct ht_data) * cnt);
	*msg_np += sizeof(struct ht_data) * cnt;
	
	s_data.smv.sdi[DID_HT].cnt = 0;
	pthread_mutex_unlock(&mutex_arr[DID_HT]);
}

static void send_data_mem_cpy_ddv(char **msg_np)
{
	// ddv
#if 0
	pthread_mutex_lock(&mutex_arr[DID_DD]);
	memcpy(*msg_np, &s_data.ddv.did, sizeof(s_data.ddv.did));
	*msg_np += sizeof(s_data.ddv.did);

	memcpy(*msg_np, &s_data.ddv.dd_cnt, sizeof(s_data.ddv.dd_cnt));
	*msg_np += sizeof(s_data.ddv.dd_cnt);

	memcpy(*msg_np, s_data.ddv.dd_dat, sizeof(struct dd_data) * s_data.ddv.dd_cnt);
	*msg_np += sizeof(struct dd_data) * s_data.ddv.dd_cnt;
	s_data.ddv.dd_cnt = 0;
#endif
	uint8_t cnt = s_data.smv.sdi[DID_DD].cnt;
	uint8_t did = s_data.smv.sdi[DID_DD].did;

	memcpy(*msg_np, &s_data.smv.sdi[DID_DD].did, sizeof(s_data.smv.sdi[DID_DD].did));
	*msg_np += sizeof(s_data.smv.sdi[DID_DD].did);

	memcpy(*msg_np, &s_data.smv.sdi[DID_DD].cnt, sizeof(s_data.smv.sdi[DID_DD].cnt));
	*msg_np += sizeof(s_data.smv.sdi[DID_DD].did);

	memcpy(*msg_np, s_data.smv.sd.dd, sizeof(struct dd_data) * cnt);
	*msg_np += sizeof(struct dd_data) * cnt;
	
	s_data.smv.sdi[DID_DD].cnt = 0;
	pthread_mutex_unlock(&mutex_arr[DID_DD]);
}

static void send_data_mem_cpy_rwv(char **msg_np)
{
	// rwv
#if 0
	pthread_mutex_lock(&mutex_arr[DID_RW]);
	memcpy(*msg_np, &s_data.rwv.did, sizeof(s_data.rwv.did));
	*msg_np += sizeof(s_data.rwv.did);

	memcpy(*msg_np, &s_data.rwv.rw_cnt, sizeof(s_data.rwv.rw_cnt));
	*msg_np += sizeof(s_data.rwv.rw_cnt);

	memcpy(*msg_np, s_data.rwv.rw_dat, sizeof(struct rw_data) * s_data.rwv.rw_cnt);
	*msg_np += sizeof(struct rw_data) * s_data.rwv.rw_cnt;
	s_data.rwv.rw_cnt = 0;
#endif
	uint8_t cnt = s_data.smv.sdi[DID_RW].cnt;
	uint8_t did = s_data.smv.sdi[DID_RW].did;

	memcpy(*msg_np, &s_data.smv.sdi[DID_RW].did, sizeof(s_data.smv.sdi[DID_RW].did));
	*msg_np += sizeof(s_data.smv.sdi[DID_RW].did);

	memcpy(*msg_np, &s_data.smv.sdi[DID_RW].cnt, sizeof(s_data.smv.sdi[DID_RW].cnt));
	*msg_np += sizeof(s_data.smv.sdi[DID_RW].did);

	memcpy(*msg_np, s_data.smv.sd.rw, sizeof(struct rw_data) * cnt);
	*msg_np += sizeof(struct rw_data) * cnt;
	
	s_data.smv.sdi[DID_RW].cnt = 0;
	pthread_mutex_unlock(&mutex_arr[DID_RW]);
}

static void send_data_mem_cpy_drv(char **msg_np)
{
	// drv
#if 0
	pthread_mutex_lock(&mutex_arr[DID_DR]);
	memcpy(*msg_np, &s_data.drv.did, sizeof(s_data.drv.did));
	*msg_np += sizeof(s_data.drv.did);

	memcpy(*msg_np, &s_data.drv.dr_cnt, sizeof(s_data.drv.dr_cnt));
	*msg_np += sizeof(s_data.drv.dr_cnt);

	memcpy(*msg_np, s_data.drv.dr_dat, sizeof(struct dr_data) * s_data.drv.dr_cnt);
	*msg_np += sizeof(struct dr_data) * s_data.drv.dr_cnt;
	s_data.drv.dr_cnt = 0;
#endif
	uint8_t cnt = s_data.smv.sdi[DID_DR].cnt;
	uint8_t did = s_data.smv.sdi[DID_DR].did;

	memcpy(*msg_np, &s_data.smv.sdi[DID_DR].did, sizeof(s_data.smv.sdi[DID_DR].did));
	*msg_np += sizeof(s_data.smv.sdi[DID_DR].did);

	memcpy(*msg_np, &s_data.smv.sdi[DID_DR].cnt, sizeof(s_data.smv.sdi[DID_DR].cnt));
	*msg_np += sizeof(s_data.smv.sdi[DID_DR].did);

	memcpy(*msg_np, s_data.smv.sd.dr, sizeof(struct dr_data) * cnt);
	*msg_np += sizeof(struct dr_data) * cnt;
	
	s_data.smv.sdi[DID_DR].cnt = 0;
	pthread_mutex_unlock(&mutex_arr[DID_DR]);
}

static void send_data_mem_cpy_fmv(char **msg_np)
{
	// fmv
#if 0
	pthread_mutex_lock(&mutex_arr[DID_FM]);
	memcpy(*msg_np, &s_data.fmv.did, sizeof(s_data.fmv.did));
	*msg_np += sizeof(s_data.fmv.did);

	memcpy(*msg_np, &s_data.fmv.fm_cnt, sizeof(s_data.fmv.fm_cnt));
	*msg_np += sizeof(s_data.fmv.fm_cnt);

	memcpy(*msg_np, s_data.fmv.fm_dat, sizeof(struct fm_data) * s_data.fmv.fm_cnt);
	*msg_np += sizeof(struct fm_data) * s_data.fmv.fm_cnt;
	s_data.fmv.fm_cnt = 0;
#endif
	uint8_t cnt = s_data.smv.sdi[DID_FM].cnt;
	uint8_t did = s_data.smv.sdi[DID_FM].did;

	memcpy(*msg_np, &s_data.smv.sdi[DID_FM].did, sizeof(s_data.smv.sdi[DID_FM].did));
	*msg_np += sizeof(s_data.smv.sdi[DID_FM].did);

	memcpy(*msg_np, &s_data.smv.sdi[DID_FM].cnt, sizeof(s_data.smv.sdi[DID_FM].cnt));
	*msg_np += sizeof(s_data.smv.sdi[DID_FM].did);

	memcpy(*msg_np, s_data.smv.sd.fm, sizeof(struct fm_data) * cnt);
	*msg_np += sizeof(struct fm_data) * cnt;
	
	s_data.smv.sdi[DID_FM].cnt = 0;
	pthread_mutex_unlock(&mutex_arr[DID_FM]);
}

static void send_data_mem_cpy(char **msg_np, uint16_t *size)
{
	int i;
	void (*fp[])(char **) = {
		send_data_mem_cpy_htv,
		send_data_mem_cpy_ddv,
		send_data_mem_cpy_rwv,
		send_data_mem_cpy_drv,
		send_data_mem_cpy_fmv,
	};
	send_data_mem_cpy_prv(msg_np, size);

	for(i = 0; i < (sizeof(fp) / sizeof(fp[0])); i++)
	{
		fp[i](msg_np);
	}
}

void * send_msg(void *arg)
{
	int sock = *((int*)arg);
	int cnt = 0;
	uint16_t size = 0;
	char *msg = NULL;
	char *msg_n = NULL;

	while(1)
	{
		size = 0;

		for(cnt = 0; cnt < 10; cnt++)
		{
			delay(1000);
		}

		get_send_data_size(&size);
		printf("total size : %d\n", size); 

		msg_n = msg = (char *)malloc(size);
		if(msg == NULL)
		{
			printf("malloc failed\n");
			return NULL;
		}

		memset(msg, 0, size);

		send_data_mem_cpy(&msg_n, &size);

		for(int i = 0; i < size; i++)
		{
			printf("msg[%d] : %x\n", i, msg[i]);
		}

		write(sock , msg, size);

		free(msg);
	}

	return NULL;
}

void * recv_msg(void *arg)
{
#if 0
	int sock = *((int*)arg);
	char name_msg[NAME_SIZE + BUF_SIZE];
	int str_len;
	while(1)
	{
		str_len = read(sock, name_msg, NAME_SIZE + BUF_SIZE - 1);
		if(str_len == -1)
		{
			return (void*)-1;
		}
		name_msg[str_len] = 0;
		fputs(name_msg, stdout);
	}
#endif
	return NULL;
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

static void get_send_data_size(uint16_t *size)
{
	int __size = 0;
	int i;
	__size += 1; // op (0000 0010)

	__size += 4; // s_data.uid

	for(i = 0; i < DID_TOTAL_CNT; i++)
	{
		__size += 1; // s_data.smv.sdi[i].did size;
		__size += 1; // s_data.smv.sdi[i].cnt size;

		switch(i)
		{
		case DID_HT:
			__size += s_data.smv.sdi[i].cnt * sizeof(struct ht_data);
			break;

		case DID_DD:
			__size += s_data.smv.sdi[i].cnt * sizeof(struct dd_data);
			break;
		case DID_RW:
			__size += s_data.smv.sdi[i].cnt * sizeof(struct rw_data);
			break;
		case DID_DR:
			__size += s_data.smv.sdi[i].cnt * sizeof(struct dr_data);
			break;
		case DID_FM:
			__size += s_data.smv.sdi[i].cnt * sizeof(struct fm_data);
			break;
		}
	}

#if 0	
	__size += 1; // s_data.htv.did 
	__size += 1; // s_data.htv.ht_cnt 
	__size += s_data.htv.ht_cnt * sizeof(struct ht_data); //s_data.htv.ht_dat[i];

	__size += 1; // s_data.ddv.did 
	__size += 1; // s_data.ddv.dd_cnt 
	__size += s_data.ddv.dd_cnt * sizeof(struct dd_data); //s_data.ddv.dd_dat[i];

	__size += 1; // s_data.rwv.did 
	__size += 1; // s_data.rwv.dd_cnt 
	__size += s_data.rwv.rw_cnt * sizeof(struct rw_data);//s_data.rwv.rw_dat[i];

	
	__size += 1; // s_data.drv.did 
	__size += 1; // s_data.drv.dr_cnt;
	__size += s_data.drv.dr_cnt * sizeof(struct dr_data); //s_data.drv.dr_dat[i];

	
	__size += 1; // s_data.fmv.did
	__size += 1; // s_data.fmv.fm_cnt 
	__size += s_data.fmv.fm_cnt * sizeof(struct fm_data); //s_data.fmv.fm_dat[i];
#endif
	*size = __size;
}

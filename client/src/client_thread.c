/**********************************************************
 * Created  : 2022.09.13 
 * Modified : 2022.09.19
 * Author   : SangDon Park
 * 
 * Desc     : Client.c threads & functions
 **********************************************************/
#include "client.h"

extern struct smart_window_send_data	s_data;
extern pthread_mutex_t			mutex_arr[DID_TOTAL_CNT];	

static void get_send_data_size(uint16_t *size);
static void send_data_mem_cpy(char **msg_np, uint16_t *size);
static void send_data_mem_cpy_prv(char **msg_np, uint16_t *size);
static void send_data_mem_cpy_all(char **msg_np);

static struct tm* current_time(void);
static int data_store(did_t did, void *arg);
static int time_store(struct td *tdp);
static int ht_dat_proc(void *arg);
static int dd_dat_proc(void *arg);
static int rw_dat_proc(void *arg);
static int dr_dat_proc(void *arg);
static int fm_dat_proc(void *arg);
#if 0
static int init_signal_send();
#endif

static int ht_dat_proc(void *arg)
{
	pthread_mutex_lock(&mutex_arr[DID_HT]);
	uint8_t *ht_cnt = &s_data.smv.sdi[DID_HT].cnt;
	struct ht_data *htdp = &s_data.smv.sd.ht[*ht_cnt];

	htdp->h_int = ((int *)arg)[0];
	htdp->h_flt = ((int *)arg)[1];
	htdp->t_int = ((int *)arg)[2];
	htdp->t_flt = ((int *)arg)[3];

	time_store(&htdp->t);
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
	uint8_t *dr_cnt = &s_data.smv.sdi[DID_DR].cnt;
	struct dr_data *drdp = &s_data.smv.sd.dr[*dr_cnt];

	drdp->open = *(int *)arg;

	time_store(&drdp->t);
	(*dr_cnt)++;
	pthread_mutex_unlock(&mutex_arr[DID_DR]);

	return 1;
}

static int fm_dat_proc(void *arg)
{
	pthread_mutex_lock(&mutex_arr[DID_FM]);
	uint8_t *fm_cnt = &s_data.smv.sdi[DID_FM].cnt;

	struct fm_data *fmdp = &s_data.smv.sd.fm[*fm_cnt];

	fmdp->speed = htons(*(uint16_t *)arg);

	time_store(&fmdp->t);
	(*fm_cnt)++;
	pthread_mutex_unlock(&mutex_arr[DID_FM]);

	return 1;
}

static int time_store(struct td *tdp)
{
	struct tm* s_tm;

	s_tm = current_time();

	tdp->year = s_tm->tm_year;
	tdp->mon  = s_tm->tm_mon;
	tdp->day  = s_tm->tm_mday;
	tdp->hour = s_tm->tm_hour;
	tdp->min  = s_tm->tm_min;
	tdp->sec  = s_tm->tm_sec;
	
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
	s_data.smv.sdi[DID_DD].did = DID_DD;
	
	return NULL;
}

void * rw_thread(void *arg)
{
	s_data.smv.sdi[DID_RW].did = DID_RW;

	return NULL;
}

static struct tm* current_time(void)
{
	time_t t;
	static struct tm s_tm;

	t    = time(NULL);
	s_tm = *localtime(&t);

	s_tm.tm_year = htons(s_tm.tm_year + 1900); // store as big endian
	s_tm.tm_mon  += 1;

	return &s_tm;
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
	__size = htons(__size);
	memcpy(*msg_np, &__size, sizeof(*size));
	*msg_np += sizeof(*size);

	// uid
	__uid = htonl(s_data.uid);
	memcpy(*msg_np, &__uid, sizeof(s_data.uid));
	*msg_np += sizeof(s_data.uid);
}

static void send_data_mem_cpy_all(char **msg_np)
{
	struct sm       *smp = &s_data.smv;
	struct sm_data  *sdp = &smp->sd;
	int i;
	uint8_t did, cnt;

	for(i = 0; i < DID_TOTAL_CNT; i++)
	{
		pthread_mutex_lock(&mutex_arr[i]);
		did = smp->sdi[i].did;
		memcpy(*msg_np, &did, sizeof(did));
		*msg_np += sizeof(did);

		cnt = smp->sdi[i].cnt;
		memcpy(*msg_np, &cnt, sizeof(cnt));
		*msg_np += sizeof(cnt);

		switch(i)
		{
		case DID_HT:
			memcpy(*msg_np, sdp->ht, sizeof(struct ht_data) * cnt);
			*msg_np += sizeof(struct ht_data) * cnt;
			break;
		case DID_DD:
			memcpy(*msg_np, sdp->dd, sizeof(struct dd_data) * cnt);
			*msg_np += sizeof(struct dd_data) * cnt;
			break;
		case DID_RW:
			memcpy(*msg_np, sdp->rw, sizeof(struct rw_data) * cnt);
			*msg_np += sizeof(struct rw_data) * cnt;
			break;
		case DID_DR:
			memcpy(*msg_np, sdp->dr, sizeof(struct dr_data) * cnt);
			*msg_np += sizeof(struct dr_data) * cnt;
			break;
		case DID_FM:
			memcpy(*msg_np, sdp->fm, sizeof(struct fm_data) * cnt);
			*msg_np += sizeof(struct fm_data) * cnt;
			break;
		default:
			break;
		}
		smp->sdi[i].cnt = 0;
		pthread_mutex_unlock(&mutex_arr[i]);
	}
}

static void send_data_mem_cpy(char **msg_np, uint16_t *size)
{
	send_data_mem_cpy_prv(msg_np, size);
	send_data_mem_cpy_all(msg_np);
}

void * send_msg(void *arg)
{
	int sock;
	struct sockaddr_in serv_addr;
	int cnt = 0;
	uint16_t size = 0;
	char *msg = NULL;
	char *msg_n = NULL;
	int read_len;

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family            = AF_INET;
	serv_addr.sin_addr.s_addr       = inet_addr(SERVER_IP);
	serv_addr.sin_port              = htons(atoi(SERVER_PORT));

#if 0
	for(int i = 0; i < 10; i++)
		init_signal_send();
#endif

	while(1)
	{
		size = 0;
		sock = socket(PF_INET, SOCK_STREAM, 0);

		printf("sock : %d\n", sock);

		if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
			error_handling("connect() error");

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
		shutdown(sock, SHUT_WR);
		read_len = read(sock, msg, size);
		printf("read_len : %d\n", read_len);
		printf("read_len : %s\n", msg);

		free(msg);
		close(sock);
	}

	return NULL;
}

#if 0
static int init_signal_send()
{
	static int serv_sock;
	struct sockaddr_in serv_adr;
	const int INIT_SIZE = 7;
	char send_msg[INIT_SIZE];
	char *msgp = send_msg;

	uint8_t send_op = 0b0000;
	uint16_t send_size = INIT_SIZE;
	uint32_t uid = 1;

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serv_adr.sin_port = htons(atoi("60004"));

	printf("serv_sock : %d\n", serv_sock);

	*msgp++ = send_op;

	*(uint16_t*)msgp = htons(send_size);
	msgp += sizeof(send_size);

	*(uint32_t*)msgp = htonl(uid);

	for(int i = 0; i < INIT_SIZE; i++)
	{
		printf("%x\n", send_msg[i]);
	}

	if(connect(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1 )
	{
		error_handling("connect() error~~~");
	}

	write(serv_sock, send_msg, INIT_SIZE);

	puts("write~~~~~");

	char message[100] = "hawawa...";
	int read_cnt, cnt; 

	sleep(1);
	close(serv_sock);

#if 0
	while(1)
#endif
	{
		puts("hmmmm");
		printf("serv_sock : %d\n", serv_sock);
		read_cnt = read(serv_sock, message, 3);
		printf("read_cnt : %d\n", read_cnt);
		if(read_cnt == -1)
		{
			puts("read_cnt == -1");
		}
		else if(read_cnt == 0)
		{
			printf("a");
		}
	}
	printf("message : %s\n", message);
	
#if 0
	close(serv_sock);
#endif

	return 1;
}
#endif

void * recv_msg(void *arg)
{
	int serv_sock;
	struct sockaddr_in serv_adr;
	const int INIT_SIZE = 7;
	char send_msg[INIT_SIZE];
	char msg[100];
	char *msgp = send_msg;

	uint8_t send_op = 0b0000;
	uint16_t send_size = INIT_SIZE;
	uint32_t uid = 1;

	int read_cnt, cnt; 

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serv_adr.sin_port = htons(atoi(SERVER_PORT2));

	printf("serv_sock : %d\n", serv_sock);

	*msgp++ = send_op;

	*(uint16_t*)msgp = htons(send_size);
	msgp += sizeof(send_size);

	*(uint32_t*)msgp = htonl(uid);

	for(int i = 0; i < INIT_SIZE; i++)
	{
		printf("%x\n", send_msg[i]);
	}

	if(connect(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1 )
	{
		error_handling("connect() error~~~");
	}

	write(serv_sock, send_msg, INIT_SIZE);

	while(1)
	{
		read_cnt = read(serv_sock, msg, sizeof(msg));
		cnt += read_cnt;

		if(read_cnt == -1)
		{
			puts("read_cnt == -1");
		}
		else if(read_cnt == 0)
		{
			printf("a");
		}
	}
	
	close(serv_sock);

	return NULL;

#if 0
	int my_sock, serv_sock;
	struct sockaddr_in my_adr, serv_adr;
	unsigned int serv_adr_sz;
	const int INIT_SIZE = 7;
	const int PORT_NUM = 58972;
	char init_read_msg[INIT_SIZE];
	int read_cnt, cnt;
	int my_sock2;

	init_signal_send();
		
	while(1);

	my_sock = socket(PF_INET, SOCK_STREAM, 0);
	
	memset(&my_adr, 0, sizeof(my_adr));
	my_adr.sin_family = AF_INET;
	my_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_adr.sin_port = htons(PORT_NUM);
	printf("my_sock : %d\n", my_sock);


	my_sock2 = socket(PF_INET, SOCK_STREAM, 0);
	my_adr.sin_family = AF_INET;
	my_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_adr.sin_port = htons(20000);
	printf("my_sock2 : %d\n", my_sock2);

	if(bind(my_sock, (struct sockaddr*)&my_adr, sizeof(my_adr)) == -1)
	{
		error_handling("bind() error");
	}

	if(listen(my_sock, 5) == -1)
	{
		error_handling("listen() error");
	}

	while(1)
	{
		cnt = 0;

		serv_sock = accept(my_sock, (struct sockaddr*)&serv_adr, &serv_adr_sz);

		printf("Connected client IP   : %s \n", inet_ntoa(serv_adr.sin_addr));
		printf("Connected client Port : %d \n", ntohs(serv_adr.sin_port)); 

		while(cnt < INIT_SIZE)
		{
			read_cnt = read(serv_sock, init_read_msg, INIT_SIZE);
			if(read_cnt == -1)
			{

				break;
			}
			else if(read_cnt == 0)
			{
				puts("read_cnt 0");
				break;
			}
			
			cnt += read_cnt;
		}

		if(!cnt)
		{
			puts("cnt == 0");
			continue;
		}

		// 0000 10xx
		// 0000 1100
		switch(((*init_read_msg) >> 2) ^ 0x3)
		{
		case 1:
			// 0000 10xx
			// 0000 1011
			switch(*init_read_msg & 0x3)
			{
			case 1:
				// control of raspberry pi
				//read();
				break;
			case 2:
				// response of connection request
				break;
			case 3:
				// response of first connection signal
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
			
	}


	return NULL;
#endif
}

static void get_send_data_size(uint16_t *size)
{
	int __size = 0;
	int i;
	__size += 1; // op (0000 0010)
	__size += 2; // total bytes
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

	*size = __size;
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

/*
 * Created  : 2022.08.31
 * Modified : 2022.09.18
 * Author   : SangDon Park
 */
#include "client.h"

#define THREAD_CNT ( sizeof(fthread_p) / sizeof(fthread_p[0]) )

static void get_send_data_size(uint16_t *size);
static void send_data_mem_cpy(char **msg_np, uint16_t *size);
static void send_data_mem_cpy_prv(char **msg_np, uint16_t *size);
static void send_data_mem_cpy_all(char **msg_np);

struct smart_window_send_data	s_data;
pthread_mutex_t 		mutex_arr[DID_TOTAL_CNT];

int main(void)
{
	int i;
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
		pthread_create(&pth_arr[i], NULL, fthread_p[i], NULL);
	}

	for(i = 0; i < THREAD_CNT; i++)
	{
		pthread_join(pth_arr[i], &thread_return);
	}

	for(i = 0; i < DID_TOTAL_CNT; i++)
	{
		pthread_mutex_destroy(&mutex_arr[i]);
	}

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
	struct sm 	*smp = &s_data.smv;
	struct sm_data	*sdp = &smp->sd;
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
	serv_addr.sin_family 		= AF_INET;
	serv_addr.sin_addr.s_addr 	= inet_addr(SERVER_IP);
	serv_addr.sin_port 		= htons(atoi(SERVER_PORT));

	while(1)
	{
		size = 0;
		sock = socket(PF_INET, SOCK_STREAM, 0);
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

void * recv_msg(void *arg)
{
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

	*size = __size;
}

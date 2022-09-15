/*
 * Create date : 2022.08.31
 * Author : SangDon Park
 */
#include "client.h"

#define THREAD_CNT ( sizeof(fthread_p) / sizeof(fthread_p[0]) )

char *server_ip = "172.30.1.46";
char *server_port = "39202";

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
	serv_addr.sin_addr.s_addr 	= inet_addr(server_ip);
	serv_addr.sin_port 		= htons(atoi(server_port));

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

void * send_msg(void *arg)
{
	int sock = *((int*)arg);
	int cnt = 0;
	uint8_t op = 0;
	uint16_t size = 0;
	char *msg = NULL;
	char *msg_n = NULL;

	op |= 0b0010;

	while(1)
	{
		size = 0;

		for(cnt = 0; cnt < 60; cnt++)
		{
			delay(1000);
		}

		size += 1; // op (0000 0010)

		size += 4; // s_data.uid
		
		size += 1; // s_data.htv.did 
		size += 1; // s_data.htv.ht_cnt 
		size += s_data.htv.ht_cnt * sizeof(struct ht_data); //s_data.htv.ht_dat[i];

		size += 1; // s_data.ddv.did 
		size += 1; // s_data.ddv.dd_cnt 
		size += s_data.ddv.dd_cnt * sizeof(struct dd_data); //s_data.ddv.dd_dat[i];

		size += 1; // s_data.rwv.did 
		size += 1; // s_data.rwv.dd_cnt 
		size += s_data.rwv.rw_cnt * sizeof(struct rw_data);//s_data.rwv.rw_dat[i];

		
		size += 1; // s_data.drv.did 
		size += 1; // s_data.drv.dr_cnt;
		size += s_data.drv.dr_cnt * sizeof(struct dr_data); //s_data.drv.dr_dat[i];

		
		size += 1; // s_data.fmv.did
		size += 1; // s_data.fmv.fm_cnt 
		size += s_data.fmv.fm_cnt * sizeof(struct fm_data); //s_data.fmv.fm_dat[i];
		printf("total size : %d\n", size); 

		msg_n = msg = (char *)malloc(size);
		memset(msg, 0, size);
		
		// op
		memcpy(msg_n, &op, sizeof(op));
		msg_n += sizeof(op);

		// total data size
		memcpy(msg_n, &size, sizeof(size));
		msg_n += sizeof(size);

		// uid
		memcpy(msg_n, &s_data.uid, sizeof(s_data.uid));
		msg_n += sizeof(s_data.uid);
		

		pthread_mutex_lock(&mutex_arr[DID_HT]);
		// htv
		memcpy(msg_n, &s_data.htv.did, sizeof(s_data.htv.did));
		msg_n += sizeof(s_data.htv.did);

		memcpy(msg_n, &s_data.htv.ht_cnt, sizeof(s_data.htv.ht_cnt));
		msg_n += sizeof(s_data.htv.ht_cnt);

		memcpy(msg_n, s_data.htv.ht_dat, sizeof(struct ht_data) * s_data.htv.ht_cnt);
		msg_n += sizeof(struct ht_data) * s_data.htv.ht_cnt;
		s_data.htv.ht_cnt = 0;
		pthread_mutex_unlock(&mutex_arr[DID_HT]);

		// ddv
		pthread_mutex_lock(&mutex_arr[DID_DD]);
		memcpy(msg_n, &s_data.ddv.did, sizeof(s_data.ddv.did));
		msg_n += sizeof(s_data.ddv.did);

		memcpy(msg_n, &s_data.ddv.dd_cnt, sizeof(s_data.ddv.dd_cnt));
		msg_n += sizeof(s_data.ddv.dd_cnt);

		memcpy(msg_n, s_data.ddv.dd_dat, sizeof(struct dd_data) * s_data.ddv.dd_cnt);
		msg_n += sizeof(struct dd_data) * s_data.ddv.dd_cnt;
		s_data.ddv.dd_cnt = 0;
		pthread_mutex_unlock(&mutex_arr[DID_DD]);

		// rwv
		pthread_mutex_lock(&mutex_arr[DID_RW]);
		memcpy(msg_n, &s_data.rwv.did, sizeof(s_data.rwv.did));
		msg_n += sizeof(s_data.rwv.did);

		memcpy(msg_n, &s_data.rwv.rw_cnt, sizeof(s_data.rwv.rw_cnt));
		msg_n += sizeof(s_data.rwv.rw_cnt);

		memcpy(msg_n, s_data.rwv.rw_dat, sizeof(struct rw_data) * s_data.rwv.rw_cnt);
		msg_n += sizeof(struct rw_data) * s_data.rwv.rw_cnt;
		s_data.rwv.rw_cnt = 0;
		pthread_mutex_unlock(&mutex_arr[DID_RW]);

		// drv
		pthread_mutex_lock(&mutex_arr[DID_DR]);
		memcpy(msg_n, &s_data.drv.did, sizeof(s_data.drv.did));
		msg_n += sizeof(s_data.drv.did);

		memcpy(msg_n, &s_data.drv.dr_cnt, sizeof(s_data.drv.dr_cnt));
		msg_n += sizeof(s_data.drv.dr_cnt);

		memcpy(msg_n, s_data.drv.dr_dat, sizeof(struct dr_data) * s_data.drv.dr_cnt);
		msg_n += sizeof(struct dr_data) * s_data.drv.dr_cnt;
		s_data.drv.dr_cnt = 0;
		pthread_mutex_unlock(&mutex_arr[DID_DR]);

		// fmv
		pthread_mutex_lock(&mutex_arr[DID_FM]);
		memcpy(msg_n, &s_data.fmv.did, sizeof(s_data.fmv.did));
		msg_n += sizeof(s_data.fmv.did);

		memcpy(msg_n, &s_data.fmv.fm_cnt, sizeof(s_data.fmv.fm_cnt));
		msg_n += sizeof(s_data.fmv.fm_cnt);

		memcpy(msg_n, s_data.fmv.fm_dat, sizeof(struct fm_data) * s_data.fmv.fm_cnt);
		msg_n += sizeof(struct fm_data) * s_data.fmv.fm_cnt;
		s_data.fmv.fm_cnt = 0;
		pthread_mutex_unlock(&mutex_arr[DID_FM]);

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

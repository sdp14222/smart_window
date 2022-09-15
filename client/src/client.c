/*
 * Create date : 2022.08.31
 * Author : SangDon Park
 */
#include "client.h"

#define THREAD_CNT ( sizeof(fthread_p) / sizeof(fthread_p[0]) )

char *server_ip = "172.30.1.46";
char *server_port = "39202";

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

pthread_mutex_t mutex;
struct smart_window_send_data	s_data;

int main(void)
{
	int sock;
	struct sockaddr_in serv_addr;
	void * (*fthread_p[])(void *) = {
		send_msg,
		recv_msg,
		Character_LCD_init_thread,
		fm_thread,
		dr_thread,
		dht11_thread,
	};
	pthread_t pth_arr[THREAD_CNT]; 
	int i;
	void * thread_return;

	sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(server_ip);
	serv_addr.sin_port = htons(atoi(server_port));

	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");

	wiringPiSetupGpio(); // bcm pin num

	fm_init();
    	dr_init();
	CLCD_Init();

	pthread_mutex_init(&mutex, NULL);

	s_data.uid = 1;
	s_data.ddv.did = DID_DD;
	s_data.rwv.did = DID_RW;


	for(i = 0; i < THREAD_CNT; i++)
	{
		pthread_create(&pth_arr[i], NULL, fthread_p[i], (void*)&sock);
	}

	for(i = 0; i < THREAD_CNT; i++)
	{
		pthread_join(pth_arr[i], &thread_return);
	}

	pthread_mutex_destroy(&mutex);

	close(sock);

	return 0;
}

void * send_msg(void *arg)
{
#if 0
	int sock = *((int*)arg);
	char name_msg[NAME_SIZE + BUF_SIZE];
	while(1)
	{
		fgets(msg, BUF_SIZE, stdin);
		if(!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			close(sock);
			exit(0);
		}
		sprintf(name_msg, "%s %s", name, msg);
		write(sock , name_msg, strlen(name_msg));
	}
#endif
	int cnt = 0;
	uint8_t op = 0;
	int size = 0;
	op |= 0b0010;
	char *msg = NULL;
	char *msg_n = NULL;

	while(1)
	{
		size = 0;

		for(cnt = 0; cnt < 5; cnt++)
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

		// uid
		memcpy(msg_n, &s_data.uid, sizeof(s_data.uid));
		msg_n += sizeof(s_data.uid);
		

		// htv
		memcpy(msg_n, &s_data.htv.did, sizeof(s_data.htv.did));
		msg_n += sizeof(s_data.htv.did);

		memcpy(msg_n, &s_data.htv.ht_cnt, sizeof(s_data.htv.ht_cnt));
		msg_n += sizeof(s_data.htv.ht_cnt);

		memcpy(msg_n, s_data.htv.ht_dat, sizeof(struct ht_data) * s_data.htv.ht_cnt);
		msg_n += sizeof(struct ht_data) * s_data.htv.ht_cnt;

		// ddv
		memcpy(msg_n, &s_data.ddv.did, sizeof(s_data.ddv.did));
		msg_n += sizeof(s_data.ddv.did);

		memcpy(msg_n, &s_data.ddv.dd_cnt, sizeof(s_data.ddv.dd_cnt));
		msg_n += sizeof(s_data.ddv.dd_cnt);

		memcpy(msg_n, s_data.ddv.dd_dat, sizeof(struct dd_data) * s_data.ddv.dd_cnt);
		msg_n += sizeof(struct dd_data) * s_data.ddv.dd_cnt;

		// rwv
		memcpy(msg_n, &s_data.rwv.did, sizeof(s_data.rwv.did));
		msg_n += sizeof(s_data.rwv.did);

		memcpy(msg_n, &s_data.rwv.rw_cnt, sizeof(s_data.rwv.rw_cnt));
		msg_n += sizeof(s_data.rwv.rw_cnt);

		memcpy(msg_n, s_data.rwv.rw_dat, sizeof(struct rw_data) * s_data.rwv.rw_cnt);
		msg_n += sizeof(struct rw_data) * s_data.rwv.rw_cnt;

		// drv
		memcpy(msg_n, &s_data.drv.did, sizeof(s_data.drv.did));
		msg_n += sizeof(s_data.drv.did);

		memcpy(msg_n, &s_data.drv.dr_cnt, sizeof(s_data.drv.dr_cnt));
		msg_n += sizeof(s_data.drv.dr_cnt);

		memcpy(msg_n, s_data.drv.dr_dat, sizeof(struct dr_data) * s_data.drv.dr_cnt);
		msg_n += sizeof(struct dr_data) * s_data.drv.dr_cnt;

		// fmv
		memcpy(msg_n, &s_data.fmv.did, sizeof(s_data.fmv.did));
		msg_n += sizeof(s_data.fmv.did);

		memcpy(msg_n, &s_data.fmv.fm_cnt, sizeof(s_data.fmv.fm_cnt));
		msg_n += sizeof(s_data.fmv.fm_cnt);

		memcpy(msg_n, s_data.fmv.fm_dat, sizeof(struct fm_data) * s_data.fmv.fm_cnt);
		msg_n += sizeof(struct fm_data) * s_data.fmv.fm_cnt;

		for(int i = 0; i < size; i++)
		{
			printf("msg[%d] : %x\n", i, msg[i]);
		}

		free(msg);
	}

	return NULL;
}

void * recv_msg(void *arg)
{
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
	return NULL;
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

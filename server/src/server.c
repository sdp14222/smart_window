/**************************************************
 * Created  : 2022.09.15
 * Modified : 2022.09.17
 * Author   : SangDon Park
 **************************************************/
#include "server.h"

void send_msg(char * msg, int len);
void error_handling(char * msg);
static int rptos(struct init_msg_info *info);

pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
	int serv_sock;
	struct sockaddr_in serv_adr;
	pthread_t read_thr;

	pthread_mutex_init(&mutx, NULL);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family 		= AF_INET; 
	serv_adr.sin_addr.s_addr 	= htonl(INADDR_ANY);
	serv_adr.sin_port 		= htons(atoi(SERVER_PORT));
	
	puts("server start...");

	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");

	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	pthread_create(&read_thr, NULL, read_msg, (void*)&serv_sock);

	pthread_join(read_thr, NULL);
	
	close(serv_sock);

	return 0;
}

void * read_msg(void *arg)
{
	int serv_sock = *(int*)arg;
	struct init_msg_info init_info;
	int read_cnt, cnt;
	unsigned int clnt_adr_sz = sizeof(init_info.clnt_adr);

	while(1)
	{
		puts("client wait...");
		init_info.clnt_sock = accept(serv_sock, (struct sockaddr*)&init_info.clnt_adr, &clnt_adr_sz);
		cnt = read_cnt = 0;
		memset(init_info.init_msg, 0, INIT_READ_BYTES);
		
		printf("Connected client IP  : %s \n", inet_ntoa(init_info.clnt_adr.sin_addr));
		printf("Connected client Port: %d \n", ntohs(init_info.clnt_adr.sin_port));

		while(read_cnt < INIT_READ_BYTES)
		{
			read_cnt = read(init_info.clnt_sock, init_info.init_msg, INIT_READ_BYTES);
			cnt += read_cnt;
			if(read_cnt == -1)
			{
				close(init_info.clnt_sock);
				continue;
			}
		}
		
		init_info.cmd = *(uint8_t*)(init_info.init_msg);
		printf("cmd : %d\n", init_info.cmd);
		init_info.total_size = *(uint16_t*)(init_info.init_msg + 1);
		printf("total_size : %d\n", init_info.total_size);
		init_info.uid = *(uint32_t*)(init_info.init_msg + 3);
		printf("uid : %d\n", init_info.uid);
			
		// 0000 0xxx
		// 0000 0100
		switch((init_info.cmd >> 2) & (uint8_t)0x1)
		{
			case 0:
			// raspberry pi to server
				puts("r.pi to server...");
				if(!rptos(&init_info))
				{
					puts("rptos failed...");
				}
				break;
			case 1:
			// mobile to server
#if 0
				mtos();
#endif
				break;
			default:
				printf("init_info.cmd ^ (uint8_t)0x4 : %d\n", init_info.cmd ^ (uint8_t)0x4);
				puts("abnormal connection...");
		}
		puts("client close...");
	}
}

static int rptos(struct init_msg_info *info)
{
	pthread_t t_id;

	// 0000 00xx
	// 0000 0011
	switch(info->cmd & ((uint8_t)0x3))
	{
		case 0:
		// 00 : first connect signal
			close(info->clnt_sock);
			return 0;
		case 1:
		// confirm connection response to raspberry pi
			close(info->clnt_sock);
			return 0;
		case 2:
		// data from raspberry pi
			puts("data from raspberry pi...");
			pthread_create(&t_id, NULL, handle_clnt, (void*)info);
			pthread_detach(t_id);
			return 1;
		default:
			close(info->clnt_sock);
			return 0;
	}
}
	
void * handle_clnt(void * arg)
{
	struct init_msg_info *info = (struct init_msg_info*)arg;
	int msg_len, msg_len_sum= 0;
	char *msg = NULL;
	int msg_size = info->total_size - INIT_READ_BYTES;
	int i;

	msg = (char*)malloc(msg_size);
	if(msg == NULL)
	{
		puts("handle_clnt malloc failed...");
		return NULL;

	}

	while(msg_len_sum < msg_size)
	{
		msg_len = read(info->clnt_sock, msg, info->total_size);
		if(msg_len == -1)
		{
			puts("while read...clnt disconnect...");
			close(info->clnt_sock);
			return NULL;
		}
		else if(msg_len == 0)
		{
			puts("while read end");
			break;
		}
		msg_len_sum += msg_len;
		printf("msg_len_sum : %d\n", msg_len_sum);
	}

	for(i = 0; i < msg_size; i++)
	{
		printf("msg[%d] : %x\n", i + INIT_READ_BYTES, msg[i]);	
	}

	free(msg);
	close(info->clnt_sock);

	return NULL;
}
void send_msg(char * msg, int len)   // send to all
{

}
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

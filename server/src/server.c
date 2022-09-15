/**************************************************
 * Created  : 2022.09.15
 * Modified : 2022.09.15
 * Author   : SangDon Park
 **************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "server.h"

#define MSG_BUF_SIZE 4000
#if 0
#define MAX_CLNT 256
#endif

void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);

#if 0
int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
#endif
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	unsigned int clnt_adr_sz;
	pthread_t t_id;

	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	pthread_mutex_init(&mutx, NULL);
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family 		= AF_INET; 
	serv_adr.sin_addr.s_addr 	= htonl(INADDR_ANY);
	serv_adr.sin_port 		= htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");

	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	while(1)
	{
		clnt_adr_sz = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		
		pthread_mutex_lock(&mutx);
#if 0
		clnt_socks[clnt_cnt++] = clnt_sock;
#endif
		pthread_mutex_unlock(&mutx);
	
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);

		printf("Connected client IP  : %s \n", inet_ntoa(clnt_adr.sin_addr));
		printf("Connected client Port: %d \n", ntohs(clnt_adr.sin_port));
	}
	close(serv_sock);

	return 0;
}
	
void * handle_clnt(void * arg)
{
	int clnt_sock = *((int*)arg);
	
	int msg_len= 0;
	int total_len = 0;
	char msg[MSG_BUF_SIZE] = {0};
	
	while( ( msg_len = read(clnt_sock, msg, sizeof(msg)) ) != 0)
		total_len += msg_len;

	printf("total_len : %d\n", total_len);
	for(int i = 0; i < total_len; i ++)
	{
		printf("msg[%d] : %x\n", i, msg[i]);	
	}
#if 0
	int str_len= 0, i;	

	while((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
		send_msg(msg, str_len);
	
	pthread_mutex_lock(&mutx);
	for(i = 0; i < clnt_cnt; i++)   // remove disconnected client
	{
		if(clnt_sock == clnt_socks[i])
		{
			while(i++ < clnt_cnt - 1)
				clnt_socks[i] = clnt_socks[i + 1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
#endif
	close(clnt_sock);

	return NULL;
}
void send_msg(char * msg, int len)   // send to all
{
#if 0
	int i;
	pthread_mutex_lock(&mutx);
	for(i=0; i < clnt_cnt; i++)
		write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutx);
#endif
}
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

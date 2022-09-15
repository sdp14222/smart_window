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

#define MSG_BUF_INIT_SIZE	10
#define INIT_READ_BYTES		3

#define SERVER_PORT		39202

void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);

pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	unsigned int clnt_adr_sz;
	pthread_t t_id;

	pthread_mutex_init(&mutx, NULL);
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family 		= AF_INET; 
	serv_adr.sin_addr.s_addr 	= htonl(INADDR_ANY);
	serv_adr.sin_port 		= htons(SERVER_PORT);
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");

	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	while(1)
	{
		clnt_adr_sz = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		
		pthread_mutex_lock(&mutx);

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
	int total_size = MSG_BUF_INIT_SIZE;
	char init_msg[MSG_BUF_INIT_SIZE] = {0};
	char *rmsg = NULL;
	int pos = INIT_READ_BYTES;
	int i;
	
	msg_len += read(clnt_sock, init_msg, INIT_READ_BYTES);
	printf("msg_len : %d\n", msg_len);
	total_size = *(uint16_t*)(init_msg + 1);

	rmsg = (char*)malloc(total_size - INIT_READ_BYTES);
	if(rmsg == NULL)
	{
		printf("malloc error\n");
		return NULL;
	}

	while(msg_len < total_size)
	{
		msg_len += read(clnt_sock, rmsg, total_size);
		printf("msg_len : %d\n", msg_len);
	}

	printf("total_size : %d\n", total_size);

	for(i = 0; i < total_size - INIT_READ_BYTES; i++)
	{
		printf("rmsg[%d] : %x\n", i + INIT_READ_BYTES, rmsg[i]);	
	}

	free(rmsg);

	close(clnt_sock);

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

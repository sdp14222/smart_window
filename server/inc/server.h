#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "common.h"

#define MSG_BUF_INIT_SIZE       10
#define INIT_READ_BYTES         7

struct init_msg_info
{
	int 			clnt_sock;
	struct sockaddr_in	clnt_adr;
	char			init_msg[MSG_BUF_INIT_SIZE];
	uint8_t			cmd;
	uint16_t		total_size;
	uint32_t		uid;
};

void * handle_clnt(void *arg);
void * read_msg(void *arg);
void * send_msg(void *arg);

#endif

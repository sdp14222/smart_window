/**************************************************
 * Created  : 2022.09.18
 * Modified : 2022.09.18
 * Author   : SangDon Park
 **************************************************/
#include "server.h"
static int rptos(struct init_msg_info *info);
static int mtos(struct init_msg_info *info);
static void * handle_clnt(void *arg);
static void * control_thread(void *arg);

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
		// 01 : confirm connection response to raspberry pi
		close(info->clnt_sock);
		return 0;
	case 2:
		// 10 : data from raspberry pi
		puts("data from raspberry pi...");
		pthread_create(&t_id, NULL, handle_clnt, (void*)info);
		pthread_detach(t_id);
		return 1;
	default:
		close(info->clnt_sock);
		return 0;
	}
}

static int mtos(struct init_msg_info *info)
{
	pthread_t t_id;

	// 0000 01xx
	// 0000 0011
	switch(info->cmd & ((uint8_t)0x3))
	{
	case 0:
		// 00 : current connection request of raspberry pi
		close(info->clnt_sock);
		return 0;
	case 1:
		// 01 : current status request of raspberry pi
		close(info->clnt_sock);
		return 0;
	case 2:
		// 01 : remote control request of raspberry pi
		puts("control request of raspberry pi...");
		pthread_create(&t_id, NULL, control_thread, (void*)info);
		pthread_detach(t_id);
		return 1;
	default:
		close(info->clnt_sock);
		return 0;
	}
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
			printf("read_cnt : %d\n", read_cnt);
			cnt += read_cnt;
			printf("cnt : %d\n", cnt);
			if(read_cnt == -1)
			{
				close(init_info.clnt_sock);
				break;
			}
			else if(read_cnt == 0)
			{
				puts("while read 0...");
				close(init_info.clnt_sock);
				break;
			}
		}
		for(int i = 0; i < read_cnt; i++)
		{
			printf("%x\n", init_info.init_msg[i]);
		}

		init_info.cmd = *(uint8_t*)(init_info.init_msg);
		printf("cmd : %d\n", init_info.cmd);
		init_info.total_size = ntohs(*(uint16_t*)(init_info.init_msg + 1));
		printf("total_size : %d\n", init_info.total_size);
		init_info.uid = ntohl(*(uint32_t*)(init_info.init_msg + 3));
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
			puts("mobile to server...");
			if(!mtos(&init_info))
			{
				puts("mtos failed...");
			}
			break;
		default:
			printf("init_info.cmd ^ (uint8_t)0x4 : %d\n", init_info.cmd ^ (uint8_t)0x4);
			puts("abnormal connection...");
		}
		puts("client close...");
	}
}

static void * handle_clnt(void * arg)
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

	write(info->clnt_sock, "close from server", 50);
	free(msg);
	close(info->clnt_sock);

	return NULL;
}

void * send_msg(void *arg)
{
	return NULL;
}

static void * control_thread(void *arg)
{
	struct init_msg_info *info = (struct init_msg_info*)arg;
	int msg_len, msg_len_sum= 0;
	char *msg = NULL;
	int msg_size = info->total_size - INIT_READ_BYTES;
	int i;

	msg = (char*)malloc(msg_size);
	if(msg == NULL)
	{
		puts("control_thread malloc failed...");
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

	puts("control_thread msgs...");
	for(i = 0; i < msg_size; i++)
	{
		printf("msg[%d] : %x\n", i + INIT_READ_BYTES, msg[i]);
	}

	write(info->clnt_sock, "close from server", 50);
	free(msg);
	close(info->clnt_sock);

	return NULL;
}


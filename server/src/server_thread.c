/**************************************************
 * Created  : 2022.09.18
 * Modified : 2022.09.19
 * Author   : SangDon Park
 **************************************************/
#include "server.h"
#include "server_sql.h"

static int rptos(struct init_msg_info *info);
static int mtos(struct init_msg_info *info);
static void * dfrp(void *arg);
static void * control_thread(void *arg);
static void * read_msg(void *arg);

static int rptos(struct init_msg_info *info)
{
	pthread_t t_id;

	// 0000 00xx
	// 0000 0011
	switch(info->cmd & ((uint8_t)0x3))
	{
	case 0:
		// 00 : first connect signal
		puts("first connect signal");
		close(info->clnt_sock);
		return 1;
	case 1:
		// 01 : confirm connection response to raspberry pi
		close(info->clnt_sock);
		return 0;
	case 2:
		// 10 : data from raspberry pi
		puts("data from raspberry pi...");
		pthread_create(&t_id, NULL, dfrp, (void*)info);
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
	struct init_msg_info *init_info = (struct init_msg_info*)arg;
 	int read_cnt = 0, cnt = 0;
	memset(init_info->init_msg, 0, INIT_READ_BYTES);

	printf("Connected client IP  : %s \n", inet_ntoa(init_info->clnt_adr.sin_addr));
	printf("Connected client Port: %d \n", ntohs(init_info->clnt_adr.sin_port));

	while(read_cnt < INIT_READ_BYTES)
	{
		read_cnt = read(init_info->clnt_sock, init_info->init_msg, INIT_READ_BYTES);
		printf("read_cnt : %d\n", read_cnt);
		cnt += read_cnt;
		printf("cnt : %d\n", cnt);
		if(read_cnt == -1)
		{
			close(init_info->clnt_sock);
			return NULL;
		}
		else if(read_cnt == 0)
		{
			puts("while read 0...");
			close(init_info->clnt_sock);
			break;
		}
	}

	if(!cnt)
	{
		puts("cnt == 0");	
		return NULL;
	}

	for(int i = 0; i < cnt; i++)
	{
		printf("%x\n", init_info->init_msg[i]);
	}

	init_info->cmd = *(uint8_t*)(init_info->init_msg);
	printf("cmd : %d\n", init_info->cmd);
	init_info->total_size = ntohs(*(uint16_t*)(init_info->init_msg + 1));
	printf("total_size : %d\n", init_info->total_size);
	init_info->uid = ntohl(*(uint32_t*)(init_info->init_msg + 3));
	printf("uid : %d\n", init_info->uid);

	// 0000 0xxx
	// 0000 0100
	switch((init_info->cmd >> 2) & (uint8_t)0x1)
	{
	case 0:
		// raspberry pi to server
		puts("r.pi to server...");
		if(!rptos(init_info))
		{
			puts("rptos failed...");
		}
		break;
	case 1:
	// mobile to server
		puts("mobile to server...");
		if(!mtos(init_info))
		{
			puts("mtos failed...");
		}
		break;
	default:
		printf("init_info->cmd ^ (uint8_t)0x4 : %d\n", init_info->cmd ^ (uint8_t)0x4);
		puts("abnormal connection...");
	}
	puts("client close...");

	return NULL;
}

void * client_accept(void *arg)
{
	int serv_sock = *(int*)arg;
	pthread_t read_msg_thr;

	while(1)
 	{
		struct init_msg_info init_info;
		unsigned int clnt_adr_sz = sizeof(init_info.clnt_adr);

		puts("client wait...");
		init_info.clnt_sock = accept(serv_sock, (struct sockaddr*)&init_info.clnt_adr, &clnt_adr_sz);
		pthread_create(&read_msg_thr, NULL, read_msg, (void*)&init_info);
		pthread_detach(read_msg_thr);
	}

	return NULL;
}

static void * dfrp(void * arg)
{
	struct init_msg_info *info = (struct init_msg_info*)arg;
	int msg_len, msg_len_sum= 0;
	char *msg = NULL;
	int msg_size = info->total_size - INIT_READ_BYTES;
	int i;

	msg = (char*)malloc(msg_size);
	if(msg == NULL)
	{
		puts("dfrp malloc failed...");
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
	
	if(!db_save_proc(msg, info->uid))
	{
		puts("db save failed...");
	}
	else
	{
		puts("db save success...");
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

static int rasp_control(uint32_t uid, char* msg)
{
	char *cmd_msgp, *cmd_msg = NULL;
	char ip_port[2][20];
	uint8_t op = 0b1010;
	uint16_t size = 1+2+4+1;

	if(!db_get_ip_port(uid, ip_port))
	{
		puts("uid, ip, port not exist");
		return 0;
	}
	printf("ip_port[0] : %s\n", ip_port[0]);
	printf("ip_port[1] : %s\n", ip_port[1]);

	switch(msg[7])
	{
	case 0b011:
		// dr
		size += 1;
		break;
	case 0b100:
		// fm
		size += 2;
		break;
	}

	cmd_msgp = cmd_msg = (char *)malloc(size);
	if(cmd_msg == NULL)
	{
		puts("cmd_msg malloc fail");
		return 0;
	}
	*cmd_msgp++ = op;

	*cmd_msgp = htons(size);
	cmd_msgp += size;

	*cmd_msgp = htonl(uid);
	cmd_msgp += sizeof(uint32_t);

	switch(msg[7])
	{
	case 0b011:
		// dr
		size += 1;
		uint8_t val1 = msg[8];
		*cmd_msgp = val1;
		cmd_msgp++; 
		break;
	case 0b100:
		// fm
		size += 2;
		uint16_t val2 = htons(*(uint16_t*)(msg + 8));
		*cmd_msgp = val2;
		cmd_msgp += sizeof(uint16_t);
		break;
	}

	int sock;
	struct sockaddr_in serv_addr;
	sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip_port[0]);
	serv_addr.sin_port = htons(atoi(ip_port[1]));

	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
	{
		error_handling("connect() error");
	}

	write(sock, cmd_msg, size);

	shutdown(sock, SHUT_WR);
	read(sock, cmd_msg, size);

	free(cmd_msg);
	close(sock);

	return 1;
}

static void * control_thread(void *arg)
{
	struct init_msg_info *info = (struct init_msg_info*)arg;
	int msg_len, msg_len_sum = 0;
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
		printf("control msg[%d] : %x\n", i + INIT_READ_BYTES, msg[i]);
	}

	rasp_control(info->uid, msg);

	write(info->clnt_sock, "close from server", 50);
	free(msg);
	close(info->clnt_sock);

	return NULL;
}

void * client_init_proc(void *arg)
{
	int serv_sock2 = *(int*)arg;
	unsigned int clnt_adr_sz;
	struct sockaddr_in clnt_adr;
	int clnt_sock;
	char msg[INIT_READ_BYTES];
	int read_cnt, cnt;

	clnt_adr_sz = sizeof(clnt_adr);


	while(1)
	{
		clnt_sock = accept(serv_sock2, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		printf("Connected IP : %s \n", inet_ntoa(clnt_adr.sin_addr));
		printf("Connected Port : %d \n", ntohs(clnt_adr.sin_port));

		cnt = 0;
		while(cnt < INIT_READ_BYTES)
		{
			read_cnt = read(clnt_sock, msg, INIT_READ_BYTES);
			cnt += read_cnt;
			if(read_cnt == -1)
			{
				puts("disconnect..");
				break;
			}
			else if(read_cnt == 0)
			{
				break;
			}
		}

		char *msgp = msg;

		uint8_t cmd = *msgp++;

		uint16_t size = ntohs(*(uint16_t*)msgp);
		msgp += sizeof(size);

		uint32_t uid = ntohl(*(uint32_t*)msgp);

		char ip_port[2][20];
		strcpy(ip_port[0], inet_ntoa(clnt_adr.sin_addr));
		sprintf(ip_port[1], "%d", ntohs(clnt_adr.sin_port));

		db_update_ip_port(uid, ip_port); 
	}
	return NULL;
}

void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}


/**************************************************
 * Created  : 2022.09.15
 * Modified : 2022.09.18
 * Author   : SangDon Park
 **************************************************/
#include "server.h"

static void error_handling(char * msg);

int main(int argc, char *argv[])
{
	int serv_sock;
	struct sockaddr_in serv_adr;
	pthread_t read_thr;

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
	
static void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

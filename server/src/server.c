/**************************************************
 * Created  : 2022.09.15
 * Modified : 2022.09.20
 * Author   : SangDon Park
 **************************************************/
#include "server.h"

int main(int argc, char *argv[])
{
	int serv_sock, serv_sock2;
	struct sockaddr_in serv_adr, serv_adr2;
	pthread_t client_accept_thr, client_init_proc_thr;

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family 		= AF_INET; 
	serv_adr.sin_addr.s_addr 	= htonl(INADDR_ANY);
	serv_adr.sin_port 		= htons(atoi(SERVER_PORT));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");

	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	puts("server start...");

	serv_sock2 = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr2, 0, sizeof(serv_adr2));
	serv_adr2.sin_family 		= AF_INET; 
	serv_adr2.sin_addr.s_addr 	= htonl(INADDR_ANY);
	serv_adr2.sin_port 		= htons(atoi(SERVER_PORT2));

	if(bind(serv_sock2, (struct sockaddr*) &serv_adr2, sizeof(serv_adr2))==-1)
		error_handling("bind2() error");

	if(listen(serv_sock2, 5)==-1)
		error_handling("listen2() error");

	puts("server2 start...");

	pthread_create(&client_accept_thr, NULL, client_accept, (void*)&serv_sock);
	pthread_create(&client_init_proc_thr, NULL, client_init_proc, (void*)&serv_sock2);

	pthread_join(client_accept_thr, NULL);
	pthread_join(client_init_proc_thr, NULL);
	
	close(serv_sock);
	close(serv_sock2);

	return 0;
}

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

#define MSG_BUF_INIT_SIZE       10
#define INIT_READ_BYTES         3

#define SERVER_PORT             39202

#endif

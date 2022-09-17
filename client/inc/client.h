#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include "fanmotor.h"
#include "door.h"
#include "dht11.h"
#include "Character_LCD.h"
#include "common.h"

void * dht11_thread(void *arg);
void * dd_thread(void *arg);
void * rw_thread(void *arg);
void * dr_thread(void *arg);
void * fm_thread(void *arg);
void * Character_LCD_init_thread(void *arg);
void * send_msg(void *arg);
void * recv_msg(void *arg);

void error_handling(char *msg);

#endif

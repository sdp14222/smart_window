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
#include "dcmotor.h"
#include "servo.h"
#include "dht11.h"
#include "Character_LCD.h"

#define BUF_SIZE 100
#define NAME_SIZE 20

void * send_msg(void *arg);
void * recv_msg(void *arg);
void * dcmotor_thread(void *arg);
void * servo_thread(void *arg);
void * dht11_thread(void *arg);
void * Character_LCD_init_thread(void *arg);
#if 0
void * peek_data_thread(void *arg);
#endif

void error_handling(char *msg);

#if 0
extern int dht11_sdata[4];
#endif
extern pthread_mutex_t mutex;

#endif

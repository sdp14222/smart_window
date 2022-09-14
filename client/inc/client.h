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

#define BUF_SIZE 100
#define NAME_SIZE 20

void * send_msg(void *arg);
void * recv_msg(void *arg);
void * fm_thread(void *arg);
void * dr_thread(void *arg);
void * dht11_thread(void *arg);
void * Character_LCD_init_thread(void *arg);

void error_handling(char *msg);

extern pthread_mutex_t mutex;

#define DATA_MAX_CNT 80
typedef enum did
{
	DID_HT = (uint8_t)0x0,
	DID_DD = (uint8_t)0x1,
	DID_RW = (uint8_t)0x2,
	DID_DR = (uint8_t)0x3,
	DID_FM = (uint8_t)0x4,
} did_t;

/********************************
 * time data
 ********************************/
struct td
{
	uint16_t	year;
	uint8_t		mon;
	uint8_t 	day;
	uint8_t 	hour;
	uint8_t 	min;
	uint8_t 	sec;
};

/********************************
 * humidity & temperature data
 ********************************/
struct ht_data
{
	uint8_t		h_int;	// humidity int
	uint8_t		h_flt;	// humidity float
	int8_t		t_int;	// temperature int
	uint8_t		t_flt;	// temperature float
	struct td 	t; 	// time
};

/********************************
 * humidity & temperature all data 
 ********************************/
struct ht
{
	did_t		did;		// data id
	unsigned int	ht_cnt; 	// ht_data count
	struct ht_data	ht_dat[DATA_MAX_CNT];
};

/********************************
 * dust data
 ********************************/
struct dd_data
{
	unsigned int	data; // Not yet
	struct td 	t;
};

/********************************
 * dust all data
 ********************************/
struct dd
{
	did_t		did;		// data id
	unsigned int 	dd_cnt;
	struct dd_data 	dd_dat[DATA_MAX_CNT];
};

/********************************
 * rain water data
 ********************************/
struct rw_data
{
	unsigned int	data; // Not yet
	struct td 	t;
};

/********************************
 * rain water all data
 ********************************/
struct rw
{
	did_t		did;		// data id
	unsigned int	rw_cnt; 
	struct rw_data 	rw_dat[DATA_MAX_CNT];
};

/********************************
 * door data
 ********************************/
struct dr_data
{
	unsigned int	open;		// door open 
	struct td 	t;
};

/********************************
 * door all data
 ********************************/
struct dr 
{
	did_t		did;		// data id
	unsigned int	dr_cnt;
	struct dr_data 	dr_dat[DATA_MAX_CNT];
};

/********************************
 * fan motor data
 ********************************/
struct fm_data
{
	unsigned int	speed; 		// current speed 
	struct td 	t;
};

/********************************
 * fan motor all data
 ********************************/
struct fm 
{
	did_t		did;		// data id
	unsigned int	fm_cnt;
	struct fm_data	fm_dat[DATA_MAX_CNT];
};

/********************************
 * To send data all
 ********************************/
struct smart_window_send_data
{
	uint32_t 	uid;
	struct ht 	htv;
	struct dd 	ddv;
	struct rw 	rwv;
	struct dr 	drv;
	struct fm 	fmv;
};

#endif

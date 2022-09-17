#ifndef __COMMON_H__
#define __COMMON_H__

#define SERVER_IP	"172.30.1.46"
#define SERVER_PORT	"39202"
#define DATA_MAX_CNT	70

typedef enum did
{
	DID_HT		= 0x0,
	DID_DD		= 0x1,
	DID_RW		= 0x2,
	DID_DR		= 0x3,
	DID_FM		= 0x4,
	DID_TOTAL_CNT, 
} did_t; 

/********************************
 * time data
 ********************************/
#pragma pack(push, 1)
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
	uint8_t		did;		// did_t data id
	uint8_t		ht_cnt; 	// ht_data count
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
	uint8_t		did;		// did_t data id
	uint8_t		dd_cnt;
	struct dd_data 	dd_dat[DATA_MAX_CNT];
};

/********************************
 * rain water data
 ********************************/
struct rw_data
{
	uint8_t		data; 		// Not yet
	struct td 	t;
};

/********************************
 * rain water all data
 ********************************/
struct rw
{
	uint8_t		did;		// did_t data id
	uint8_t		rw_cnt; 
	struct rw_data 	rw_dat[DATA_MAX_CNT];
};

/********************************
 * door data
 ********************************/
struct dr_data
{
	uint8_t		open;		// door open 
	struct td 	t;
};

/********************************
 * door all data
 ********************************/
struct dr 
{
	uint8_t		did;		// did_t data id
	uint8_t		dr_cnt;
	struct dr_data 	dr_dat[DATA_MAX_CNT];
};

/********************************
 * fan motor data
 ********************************/
struct fm_data
{
	uint16_t	speed; 		// current speed 
	struct td 	t;
};

/********************************
 * fan motor all data
 ********************************/
struct fm 
{
	uint8_t		did;		// did_t data id
	uint8_t		fm_cnt;
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
#pragma pack(pop)

#endif

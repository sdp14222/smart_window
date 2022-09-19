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
	uint16_t	year;	// store as big endian
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
 * dust data
 ********************************/
struct dd_data
{
	unsigned int	data; // Not yet
	struct td 	t;
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
 * door data
 ********************************/
struct dr_data
{
	uint8_t		open;		// door open 
	struct td 	t;
};

/********************************
 * fan motor data
 ********************************/
struct fm_data
{
	uint16_t	speed; 		// current speed, store as big endian
	struct td 	t;
};

struct sm_data
{
#if defined __CLIENT_DEFINED__
	struct ht_data ht[DATA_MAX_CNT];
	struct dd_data dd[DATA_MAX_CNT];
	struct rw_data rw[DATA_MAX_CNT];
	struct dr_data dr[DATA_MAX_CNT];
	struct fm_data fm[DATA_MAX_CNT];
#elif defined __SERVER_DEFINED__
	struct ht_data ht;
	struct dd_data dd;
	struct rw_data rw;
	struct dr_data dr;
	struct fm_data fm;
#endif
};

struct sm_data_info
{
	uint8_t		did;
	uint8_t		cnt; 
};

struct sm
{
	struct sm_data_info	sdi[DID_TOTAL_CNT];
	struct sm_data 		sd;
};

/********************************
 * To send data all
 ********************************/
struct smart_window_send_data
{
	uint32_t 	uid;
	struct sm	smv;
};
#pragma pack(pop)

#endif

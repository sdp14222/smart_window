 /**************************************************
  2  * Created  : 2022.09.19
  3  * Modified : 2022.09.20
  4  * Author   : SangDon Park
  5  **************************************************/
#include "server.h"
#include "server_sql.h"

static int db_ht_save(char **msg, uint32_t uid);
static int db_dd_save(char **msg, uint32_t uid);
static int db_rw_save(char **msg, uint32_t uid);
static int db_dr_save(char **msg, uint32_t uid);
static int db_fm_save(char **msg, uint32_t uid);
static ULL time_save(char **msg, uint32_t uid);

static MYSQL *connector = NULL;
static MYSQL_RES *result;
static MYSQL_ROW row = NULL;
static char query[2048];
static char uid_query[512];
static char tid_query[512];

static int db_init(void);

static int db_init(void)
{
	//MYSQL Connection
	connector = mysql_init(NULL);
	if (!mysql_real_connect(connector, DBHOST, DBUSER, DBPASS, DBNAME, 3306, NULL, 0))
	{
		fprintf(stderr, "%s\n", mysql_error(connector));
		return 0;
	}
	puts("MariaDB connected.\n");
	return 1;
}

int db_save_proc(char *msg, uint32_t uid)
{
	int query_ret;
	struct sm_data_info sdi;
	int i,j;
	int (*fp[])(char **, uint32_t) = {
		db_ht_save,
		db_dd_save,
		db_rw_save,
		db_dr_save,
		db_fm_save,
	};

	if(!db_init())
	{
		puts("db_init() error");
		return 0;
	}
	
	sprintf(query, "SELECT uid FROM swuser WHERE uid = %d", uid);
	query_ret = mysql_query(connector, query);

	QUERY_IS_FAILED(query_ret);

	result = mysql_store_result(connector);
	row = mysql_fetch_row(result);
	QUERY_RESULT_ROW_PROC(row);

	mysql_free_result(result);

	for(i = 0; i < DID_TOTAL_CNT; i++)
	{
		sdi.did = *(uint8_t*)msg;
		msg += sizeof(uint8_t);
		printf("sdi.did : %d\n", sdi.did);
		
		sdi.cnt = *(uint8_t*)msg;
		msg += sizeof(uint8_t);
		printf("sdi.cnt : %d\n", sdi.cnt);

		for(j = 0; j < sdi.cnt; j++)
		{
			fp[i](&msg, uid);
		}
	}

	mysql_close(connector);

	return 1;
}

static int db_ht_save(char **msg, uint32_t uid)
{
	ULL tid;
	struct ht_data ht;
	int query_ret;

	puts("db_ht_save start...");

	ht.h_int = *(uint8_t*)(*msg);
	*msg += sizeof(uint8_t);

	ht.h_flt = *(uint8_t*)(*msg);
	*msg += sizeof(uint8_t);

	ht.t_int = *(uint8_t*)(*msg);
	*msg += sizeof(uint8_t);

	ht.t_flt = *(uint8_t*)(*msg);
	*msg += sizeof(uint8_t);

	tid = time_save(msg, uid);

	QUERY_UID_TID_GENERATE(uid,tid);

	sprintf(query, "INSERT INTO dht(humidity_int, humidity_flt, temperature_int, temperature_flt, uid, tid) " 
		 	"VALUES ( %d, %d, %d, %d, (%s), (%s) )", 
			ht.h_int,
			ht.h_flt,
			ht.t_int,
			ht.t_flt,
			uid_query,
			tid_query
	);

	query_ret = mysql_query(connector, query);

	QUERY_IS_FAILED(query_ret);
	puts("db_ht_save end...");

	return QUERY_SUCCESS;
}

static int db_dd_save(char **msg, uint32_t uid)
{
	return QUERY_SUCCESS;
}

static int db_rw_save(char **msg, uint32_t uid)
{
	return QUERY_SUCCESS;
}

static int db_dr_save(char **msg, uint32_t uid)
{
	ULL tid;
	struct dr_data dr;
	int query_ret;

	puts("db_dr_save start...");

	dr.open = *(uint8_t*)(*msg);
	*msg += sizeof(uint8_t);

	tid = time_save(msg, uid);

	QUERY_UID_TID_GENERATE(uid,tid);

	sprintf(query, "INSERT INTO dr(isopen, uid, tid) " 
		 	"VALUES ( %d, (%s), (%s) )", 
			dr.open,
			uid_query,
			tid_query
	);
	query_ret = mysql_query(connector, query);

	QUERY_IS_FAILED(query_ret);
	puts("db_dr_save end...");

	return QUERY_SUCCESS;
}

static int db_fm_save(char **msg, uint32_t uid)
{
	ULL tid;
	struct fm_data fm;
	int query_ret;

	puts("db_fm_save start...");

	fm.speed = ntohs(*(uint16_t*)*msg);
	*msg += sizeof(uint16_t);

	tid = time_save(msg, uid);

	QUERY_UID_TID_GENERATE(uid,tid);

	sprintf(query, "INSERT INTO fm(speed, uid, tid) " 
		 	"VALUES ( %d, (%s), (%s) )", 
			fm.speed,
			uid_query,
			tid_query
	);
	query_ret = mysql_query(connector, query);

	QUERY_IS_FAILED(query_ret);
	puts("db_fm_save end...");
	return QUERY_SUCCESS;
}

static ULL time_save(char **msg, uint32_t uid)
{
	int query_ret;
	struct td t;
	row = NULL;

	puts("time_save start...");

	t.year = ntohs(*(uint16_t*)*msg);
	*msg += sizeof(uint16_t);

	t.mon = *(uint8_t*)*msg;
	*msg += sizeof(uint8_t);

	t.day = *(uint8_t*)*msg;
	*msg += sizeof(uint8_t);

	t.hour = *(uint8_t*)*msg;
	*msg += sizeof(uint8_t);

	t.min = *(uint8_t*)*msg;
	*msg += sizeof(uint8_t);

	t.sec = *(uint8_t*)*msg;
	*msg += sizeof(uint8_t);

	sprintf(query, "INSERT INTO time(year, month, day, hour, min, sec) VALUES ( %d, %d, %d, %d, %d, %d )", 
			t.year, t.mon, t.day, t.hour, t.min, t.sec);

	query_ret = mysql_query(connector, query);
	QUERY_IS_FAILED(query_ret);

	sprintf(query, "SELECT tid from time ORDER BY tid DESC LIMIT 1");
	if(mysql_query(connector, query))
	{
		puts("QUERY_FAIL");
		return QUERY_FAIL;
	}

	result = mysql_store_result(connector);
	row = mysql_fetch_row(result);
	QUERY_RESULT_ROW_PROC(row);

	mysql_free_result(result);

	puts("time_save end...");

	return (unsigned long long)(atoi(row[0]));
}

int db_get_ip_port(uint32_t uid, char ip_port[2][20])
{
	int query_ret;

	if(!db_init())
	{
		puts("db_init() error");
		return 0;
	}

	sprintf(query, "SELECT uid,ip,port FROM swuser WHERE uid = %d", uid);
	query_ret = mysql_query(connector, query);

	QUERY_IS_FAILED(query_ret);

	result = mysql_store_result(connector);
	while((row = mysql_fetch_row(result)) != NULL)
	{
		printf("row[0] : %s\n", row[0]);
		printf("row[1] : %s\n", row[1]);
		printf("row[2] : %s\n", row[2]);
		strcpy(ip_port[0], row[1]);
		printf("strcpy ip_port[0] : %s\n", ip_port[0]);
		strcpy(ip_port[1], row[2]);
		printf("strcpy ip_port[1] : %s\n", ip_port[1]);
	}

	mysql_free_result(result);

	mysql_close(connector);

	return 1;
}

int db_update_ip_port(uint32_t uid, char ip_port[2][20])
{
	int query_ret;

	if(!db_init())
	{
		puts("db_init() error");
		return 0;
	}

	sprintf(query, "UPDATE swuser SET ip=\"%s\", port=\"%s\" where uid=%d", 
			ip_port[0], ip_port[1], uid);
	query_ret = mysql_query(connector, query);

	QUERY_IS_FAILED(query_ret);

	mysql_close(connector);

	return 1;
}

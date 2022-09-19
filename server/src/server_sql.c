#include "server.h"
#include "server_sql.h"

static int db_ht_save(char **msg, uint32_t uid);
static unsigned long long int time_save(char **msg, uint32_t uid);

static MYSQL *connector = NULL;
static MYSQL_RES *result = NULL;
static MYSQL_ROW row = NULL;
static char query[1024];

int db_proc(char *msg, uint32_t uid)
{
	int query_ret;
	struct sm_data_info sdi;
	struct tm t;
	int i,j;

	//MYSQL Connection
	connector = mysql_init(NULL);
	if (!mysql_real_connect(connector, DBHOST, DBUSER, DBPASS, DBNAME, 3306, NULL, 0))
	{
		fprintf(stderr, "%s\n", mysql_error(connector));
		return 0;
	}
	puts("MariaDB connected.\n");
	
	sprintf(query, "SELECT uid FROM swuser WHERE uid = %d", uid);
	query_ret = mysql_query(connector, query);
	if(query_ret)
	{
		puts("QUERY_FAIL");
		return 0;
	}

	result = mysql_store_result(connector);
	row = mysql_fetch_row(result);
	if(!row)
	{
		puts("row is NULL");
	}
	else
	{
		printf("row : %s\n", row[0]);
	}

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
			switch(sdi.did)
			{
			case DID_HT:
				db_ht_save(&msg, uid);
				break;
			case DID_DD:
				break;
			case DID_RW:
				break;
			case DID_DR:
				break;
			case DID_FM:
				break;
			default:
				break;
			}
		}
	}

	mysql_close(connector);

	return 1;
}

static int db_ht_save(char **msg, uint32_t uid)
{
	unsigned long long int tid;
	struct sm_data sm;
	char q1[100];
	char q2[100];

	puts("db_ht_save start...");

	sm.ht.h_int = *(uint8_t*)(*msg);
	printf("sm.ht.h_int : %d\n", sm.ht.h_int);
	*msg += sizeof(uint8_t);

	sm.ht.h_flt = *(uint8_t*)(*msg);
	printf("sm.ht.h_flt : %d\n", sm.ht.h_flt);
	*msg += sizeof(uint8_t);

	sm.ht.t_int = *(uint8_t*)(*msg);
	printf("sm.ht.t_int : %d\n", sm.ht.t_int);
	*msg += sizeof(uint8_t);

	sm.ht.t_flt = *(uint8_t*)(*msg);
	printf("sm.ht.t_flt : %d\n", sm.ht.t_flt);
	*msg += sizeof(uint8_t);

	tid = time_save(msg, uid);
	printf("tid : %lld\n", tid);

	sprintf(q1, "select uid from swuser where uid = %d", uid);
	sprintf(q2, "select tid from time   where tid = %lld", tid);

	sprintf(query, "INSERT INTO dht(humidity_int, humidity_flt, temperature_int, temperature_flt, uid, tid) " 
		 	"VALUES ( %d, %d, %d, %d, (%s), (%s) )", 
			sm.ht.h_int,
			sm.ht.h_flt,
			sm.ht.t_int,
			sm.ht.t_flt,
			q1,
			q2
	);
	printf("%s\n", query);

	puts("db_ht_save middle...");
	if(mysql_query(connector, query))
	{
		puts("QUERY_FAIL");
		return QUERY_FAIL;
	}

	puts("db_ht_save end...");

	return QUERY_SUCCESS;
}

static void db_dd_save()
{
}

static void db_rw_save()
{
}

static void db_dr_save()
{
}

static void db_fm_save()
{
}

static unsigned long long int time_save(char **msg, uint32_t uid)
{
	struct td t;
	result = row = NULL;

	puts("time_save start...");

	t.year = ntohs(*(uint16_t*)*msg);
	*msg += sizeof(uint16_t);
	printf("t.year : %d\n", t.year);

	t.mon = *(uint8_t*)*msg;
	*msg += sizeof(uint8_t);
	printf("t.mon : %d\n", t.mon);

	t.day = *(uint8_t*)*msg;
	*msg += sizeof(uint8_t);
	printf("t.day : %d\n", t.day);

	t.hour = *(uint8_t*)*msg;
	*msg += sizeof(uint8_t);
	printf("t.hour : %d\n", t.hour);

	t.min = *(uint8_t*)*msg;
	*msg += sizeof(uint8_t);
	printf("t.min : %d\n", t.min);

	t.sec = *(uint8_t*)*msg;
	*msg += sizeof(uint8_t);
	printf("t.sec : %d\n", t.sec);

	sprintf(query, "INSERT INTO time(year, month, day, hour, min, sec) VALUES ( %d, %d, %d, %d, %d, %d )", 
			t.year, t.mon, t.day, t.hour, t.min, t.sec);

	if(mysql_query(connector, query))
	{
		puts("QUERY_FAIL");
		return QUERY_FAIL;
	}

	puts("time_save middle1...");

	sprintf(query, "SELECT tid from time ORDER BY tid DESC LIMIT 1");
	if(mysql_query(connector, query))
	{
		puts("QUERY_FAIL");
		return QUERY_FAIL;
	}

	puts("time_save middle2...");
	result = mysql_store_result(connector);
	row = mysql_fetch_row(result);

	if(!row)
	{
		puts("row is NULL");
	}
	else
	{
		printf("row : %s\n", row[0]);
	}
	mysql_free_result(result);

	puts("time_save end...");

	return (unsigned long long)(atoi(row[0]));
#if 0
	return row;
#endif
}


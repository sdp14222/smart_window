#ifndef __SERVER_SQL_H__
#define __SERVER_SQL_H__

#if 0
#include </usr/include/mysql/mysql.h>
#include </usr/include/mariadb/mysql.h>
#endif
#include <mysql.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DBHOST "localhost"
#define DBUSER "root"
#define DBPASS "raspberry"
#define DBNAME "smartwindow"

typedef unsigned long long int ULL;

#define QUERY_SUCCESS	 1
#define QUERY_FAIL	-1

#define QUERY_IS_FAILED(QUERY_RET) 	do {	\
	if(QUERY_RET) 				\
	{					\
		puts("QUERY_FAIL");		\
		return 0;			\
	}					\
} while(0);


#define QUERY_UID_TID_GENERATE(UID,TID) 	do {				\
	sprintf(uid_query, "select uid from swuser where uid = %d", uid);	\
	sprintf(tid_query, "select tid from time   where tid = %lld", tid);	\
} while(0);

#define QUERY_RESULT_ROW_PROC(ROW) 	do {	\
	if(!ROW)				\
	{					\
		puts("row is NULL");		\
		return 0;			\
	}					\
	printf("row : %s\n", row[0]);		\
} while(0);

int db_proc(char *msg, uint32_t uid);

#endif

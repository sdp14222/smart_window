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

#define QUERY_SUCCESS	 1
#define QUERY_FAIL	-1

int db_proc(char *msg, uint32_t uid);

#endif

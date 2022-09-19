# ===========================================
# Created       : 2022.09.15
# Last Modified : 2022.09.19
# Author        : SangDon Park
# ===========================================

CC = gcc

# Compiler Options
CFLAGS = -Wall -O2 -D_REENTRANT -lpthread 
CFLAGS += -D__SERVER_DEFINED__ 
#CFLSGS += -lmysqlclient
CFLAGS += -lmariadb

# Linker Options
LDFLAGS =

# Header File Path
MYSQL_INC = /usr/include/mariadb #mysqlconfig --cflags --libs
COMMON_INC = $(shell pwd)/../inc/
INCLUDE = -Iinc/
INCLUDE += -I$(COMMON_INC)
INCLUDE += -I$(MYSQL_INC)

# Source File Path
SRC_DIR = ./src

# Exec File Path
OUT_DIR = ./output

TARGET = server

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $^

OBJS += $(SRC_DIR)/server.o
OBJS += $(SRC_DIR)/server_thread.o
OBJS += $(SRC_DIR)/server_sql.o

all: TARGET

TARGET: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$(TARGET)

clean:
	rm -f $(OUT_DIR)/$(TARGET)
	rm -f $(SRC_DIR)/*.o

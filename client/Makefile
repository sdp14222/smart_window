# ===========================================
# Created	: 2022.08.31
# Last Modified	: 2022.09.17
# Author	: SangDon Park
# ===========================================

CC = gcc

# Compiler Options
CFLAGS = -Wall -O2 -lwiringPi -D_REENTRANT -lpthread

# Linker Options
LDFLAGS =

# Header File Path
COMMON_INC = $(shell pwd)/../inc/
INCLUDE += -Iinc/
INCLUDE += -I$(COMMON_INC)

# Source File Path 
SRC_DIR = ./src

# Exec File Path
OUT_DIR = ./output

TARGET = client

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $^

OBJS += $(SRC_DIR)/client.o 
OBJS += $(SRC_DIR)/fanmotor.o 
OBJS += $(SRC_DIR)/door.o 
OBJS += $(SRC_DIR)/dht11.o 
OBJS += $(SRC_DIR)/Character_LCD.o
OBJS += $(SRC_DIR)/client_thread.o


all: TARGET 

TARGET: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$(TARGET)

clean:
	rm -f $(OUT_DIR)/$(TARGET)
	rm -f $(SRC_DIR)/*.o
	

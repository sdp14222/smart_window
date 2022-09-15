# ===========================================
# Created       : 2022.09.15
# Last Modified : 2022.09.15
# Author        : SangDon Park
# ===========================================

CC = gcc

# Compiler Options
CFLAGS = -Wall -O2 -D_REENTRANT -lpthread

# Linker Options
LDFLAGS =

# Header File Path
INCLUDE = -Iinc/

# Source File Path
SRC_DIR = ./src

# Exec File Path
OUT_DIR = ./output

TARGET = server

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $^

OBJS += $(SRC_DIR)/server.o

#all: $(OBJS)
#       $(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$(TARGET)

#all: objs

#objs: client.o
#       $(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$(TARGET)

#$(TARGET): $(SRC_DIR)/$(TARGET).o
#       $(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$(TARGET)
all: TARGET
#       $(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$(TARGET)

TARGET: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$(TARGET)

clean:
	rm -f $(OUT_DIR)/$(TARGET)
	rm -f $(SRC_DIR)/*.o

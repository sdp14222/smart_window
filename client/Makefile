# ===========================================
# usage : make TARGET=xxx
# xxx   : dcmotor dht11 servo clcd pm2008 szh
# ===========================================

CC = gcc

# Compiler Options
CFLAGS = -Wall -O2 -lwiringPi

# Linker Options
LDFLAGS =

# Header File Path
INCLUDE = -Iinc

# Source File Path 
SRC_DIR = ./src

# Exec File Path
OUT_DIR = ./output

TARGET = 

all: $(TARGET)

$(TARGET): $(SRC_DIR)/$(TARGET).o
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$(TARGET)

clean:
	rm -f $(OUT_DIR)/*
	rm -fv $(SRC_DIR)/*.o
	

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

CLCD = clcd 
DCMOTOR = dcmotor 
SERVO = servo
DHT11 = dht11
PM2008 = pm2008
SZH = szh

$(CLCD): $(SRC_DIR)/$(CLCD).o
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$(CLCD)

$(DCMOTOR): $(SRC_DIR)/$(DCMOTOR).o
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$(DCMOTOR)

$(SERVO): $(SRC_DIR)/$(SERVO).o
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$(SERVO)

$(DHT11): $(SRC_DIR)/$(DHT11).o
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$(DHT11)

$(PM2008): $(SRC_DIR)/$(PM2008).o
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$(PM2008)

$(SZH): $(SRC_DIR)/$(SZH).o
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$(SZH)

ALL = $(CLCD) $(DCMOTOR) $(SERVO) $(DHT11) $(PM2008) $(SZH) 

all: $(ALL)

#$(ALL): $(SRC_DIR)/$(ALL).o
#	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$(ALL)

clean:
#	rm -f $(OUT_DIR)/$(ALL)
	rm -f $(SRC_DIR)/*.o
	

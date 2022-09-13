#ifndef __DHT11_H__
#define __DHT11_H__

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAXTIMINGS	85 
#define DHTPIN		19	

int dht11_sdata[4];

void read_dht11_dat();

#endif

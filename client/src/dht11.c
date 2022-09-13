#include "dht11.h"

static int dht11_dat[5] = { 0, 0, 0, 0, 0 };
 
int read_dht11_dat(int *ret_data)
{
	uint8_t laststate	= HIGH;
	uint8_t counter		= 0;
	uint8_t j		= 0, i;
	float	f; 

	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;
 

	/* phase 1 */
	pinMode( DHTPIN, OUTPUT );
	digitalWrite( DHTPIN, LOW );
	delay( 18 );

	/* phase 2 */
	digitalWrite( DHTPIN, HIGH );
	delayMicroseconds( 40 );
	pinMode( DHTPIN, INPUT );
 
 
	for ( i = 0; i < MAXTIMINGS; i++ )
	{
		counter = 0;
		while ( digitalRead( DHTPIN ) == laststate )	/* phase 3... : change ? */
		{
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
		laststate = digitalRead( DHTPIN );	/* phase 4... : update laststate changed */
 
		if ( counter == 255 )
			break;
 
		if ( (i >= 4) && (i % 2 == 0) )
		{
			dht11_dat[j / 8] <<= 1;	/* shift up one bit ... */
			if ( counter > 40 )	/* more than 40 counts(40 us) ?  it's long. means 1. income value is 1 */ /* short ? income value is 0 */
				dht11_dat[j / 8] |= 1; 	/* this time, income bit value is 1 */
			j++;	/* counter for sampling 5 bytes = 5x8 = 40 bits */
		}
	}
 
	if ( (j >= 40) &&
	     (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )  /* [4] is checksum sent by DHT11, let's validate it ! */
	{
		f = dht11_dat[2] * 9. / 5. + 32;
		printf( "Humidity = %d.%d %% Temperature = %d.%d C (%.1f F)\n",
			dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f );
		ret_data[0] = dht11_dat[0];	
		ret_data[1] = dht11_dat[1];	
		ret_data[2] = dht11_dat[2];	
		ret_data[3] = dht11_dat[3];	
		return 1;
	}else  {
		printf( "Data not good, skip\n" );
		return 0;
	}
}
 

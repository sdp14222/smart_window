/*
 * Created  : 2022.08.31
 * Modified : 2022.09.18
 * Author   : SangDon Park
 */
#include "client.h"

#define THREAD_CNT ( sizeof(fthread_p) / sizeof(fthread_p[0]) )

struct smart_window_send_data	s_data;
pthread_mutex_t 		mutex_arr[DID_TOTAL_CNT];

int main(void)
{
	int i;
	void * (*fthread_p[])(void *) = {
		dht11_thread,
		dd_thread,
		rw_thread,
		dr_thread,
		fm_thread,
		Character_LCD_init_thread,
		send_msg,
		recv_msg,
	};
	pthread_t pth_arr[THREAD_CNT]; 
	void * thread_return;

	wiringPiSetupGpio(); // bcm pin num

	fm_init();
    	dr_init();
	CLCD_Init();

	for(i = 0; i < DID_TOTAL_CNT; i++)
	{
		pthread_mutex_init(&mutex_arr[i], NULL);
	}

	for(i = 0; i < THREAD_CNT; i++)
	{
		pthread_create(&pth_arr[i], NULL, fthread_p[i], NULL);
	}

	for(i = 0; i < THREAD_CNT; i++)
	{
		pthread_join(pth_arr[i], &thread_return);
	}

	for(i = 0; i < DID_TOTAL_CNT; i++)
	{
		pthread_mutex_destroy(&mutex_arr[i]);
	}

	return 0;
}


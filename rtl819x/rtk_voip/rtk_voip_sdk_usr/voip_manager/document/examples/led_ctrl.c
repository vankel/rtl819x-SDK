#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "voip_manager.h"
#include "voip_params.h"

#define LED_TEST	1
#define TEST_TIME 3000000

/*
 *  LED control sample code.
 *  1. Please killall solar_monitor before running this sample code.
 *	2. led_ctrl chid &
 *
 */

int main(int argc, char *argv[])
{
	if(argc < 1)
	{
		printf("usage: %s chid \n", argv[0]);
		return 0;
	}

	while (1)
	{
#ifdef LED_TEST
#if 1
		printf("%s(%d) rtk_SetLedDisplay GPIO 35 LED_ON\n" , __FUNCTION__ , __LINE__);
		rtk_SetLedDisplay( 0, 0, LED_ON );
		usleep(TEST_TIME); 

		printf("%s(%d) rtk_SetLedDisplay GPIO 35 LED_OFF\n" , __FUNCTION__ , __LINE__);
		rtk_SetLedDisplay( 0, 0, LED_OFF );
		usleep(TEST_TIME); 
		
		printf("%s(%d) rtk_SetLedDisplay GPIO 36 LED_ON\n" , __FUNCTION__ , __LINE__);
		rtk_SetLedDisplay( 0, 1, LED_ON );
		usleep(TEST_TIME); 

		printf("%s(%d) rtk_SetLedDisplay GPIO 36 LED_OFF\n" , __FUNCTION__ , __LINE__);
		rtk_SetLedDisplay( 0, 1, LED_OFF );
		usleep(TEST_TIME); 
#endif
#if 1
		unsigned long ret = 0;

		printf("%s(%d) rtk_GpioControl GPIO 35 LED_ON\n" , __FUNCTION__ , __LINE__);
		rtk_GpioControl(2 , 35 , 0 , &ret);
		usleep(TEST_TIME);
		
		printf("%s(%d) rtk_GpioControl GPIO 35 LED_OFF\n" , __FUNCTION__ , __LINE__);
		rtk_GpioControl(2 , 35 , 1 , &ret);
		usleep(TEST_TIME);

		printf("%s(%d) rtk_GpioControl GPIO 36 LED_ON\n" , __FUNCTION__ , __LINE__);
		rtk_GpioControl(2 , 36 , 0 , &ret);
		usleep(TEST_TIME);
		
		printf("%s(%d) rtk_GpioControl GPIO 36 LED_OFF\n" , __FUNCTION__ , __LINE__);
		rtk_GpioControl(2 , 36 , 1 , &ret);
		usleep(TEST_TIME);
#endif
#endif

		usleep(100000); // 100ms
	}
	return 0;
}

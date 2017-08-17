#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <microhttpd.h>
#include "common.h"
#include "i2c_adaptor.h"
#include "json_data.h"
#include "heartbeat.h"

heardbeat_t MaHeardBeat={
.running_stat = START ,/*运行状态*/
.delay = 2 ,/*两秒一次心跳*/
.TimeCount = 0 ,
.err_count = 0 ,
.err_max = 5 ,/*判断stm32是否支持心跳*/
} ;

int mxj_mj_heartbeat_proc()
{
	char * rxbuff ;
	char * txbuff ;
	
	if(MaHeardBeat.running_stat == START)
	{
		if(MaHeardBeat.TimeCount >= MaHeardBeat.delay)
		{
			if(general_transport(rxbuff,0,MAJOR_HEART_BEAT,DEFAULT_SUB_TYPE,txbuff,0) )
			goto error ;
			MaHeardBeat.TimeCount = 0 ;
		}
		else
		{
			MaHeardBeat.TimeCount++ ;
		}
	
	}
	return MXJ_SUCCESS ;
error:
	printf("MA_HEARTBEAT_ERROR: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__) ;
	return -MXJ_FAIL;	
}




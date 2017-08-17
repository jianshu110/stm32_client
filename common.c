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
#include "file_process.h"
#include "heartbeat.h"


int is_hardware_support()
{
	int err_count = 0 ;
	int i=3 ;
	unsigned char rxbuff = 0 ;
	while(i)
	{
		if(general_transport((vol_log_t*)&vol_info.vol, sizeof(vol_t) ,
					                   GET_VOL, DEFAULT_SUB_TYPE ,
					                   &rxbuff , 0))
		{
			err_count++ ;
		}
		i-- ;
	}
	
	if(err_count == 3)
	{
		temp_info.running_stat = STOP ;
		vol_info.running_stat = STOP ;
		MaHeardBeat.running_stat = STOP ;
		strcpy(httpd.ret_str,"error") ;
		return MXJ_FAIL ;
	}
	return MXJ_SUCCESS ;
}




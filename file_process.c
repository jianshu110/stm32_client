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


temp_log_t temp_info = {

	.running_stat = START ,
	/*one minute*/
	.delay  = 59,
	.TimeCount = 0 ,
	.time   = 0 ,
	.temp ={
    	.EnvTemp = 0 ,
  } ,
	.count = 0,
	 /*three months*/ 
	.max_count = 23*60*24*90 ,
  
} ;

vol_log_t vol_info ={
	
	.running_stat = START ,
	/*Three seconds*/
	.delay	= 2, 
	.TimeCount = 0 ,
	.vol ={
		.s3_3v = 0 ,
		.m5v  = 0 ,
		.m12v = 0 ,
		.m48v = 0 ,
} ,
	.time = 0 ,
	.count = 0 ,
	/*One Month*/
	.max_count = 17*20*60*24*30 , 
} ;


int mxj_voltage_proc ()
{
  	if(vol_info.running_stat == START)
  	{
		char tmp_buf[128] ;
		FILE *vol_f =  NULL ; 
		FILE *vol_backup_f = NULL ;
		unsigned char rxbuff = 0 ;
		
		if(vol_info.TimeCount <= 0 )
		{
			vol_info.TimeCount = vol_info.delay ;
			memset(&vol_info.vol , 0 ,sizeof(vol_t));
			if(general_transport((vol_t*)&vol_info.vol, sizeof(vol_t) ,
				                   GET_VOL, DEFAULT_SUB_TYPE ,
				                   &rxbuff , 0))
				goto erro ; 	
			
			
			vol_f  = fopen(VOLTAGE_PATH,"a+") ;
			if(vol_f == NULL)
				goto erro ;
			
			fseek(vol_f, 0, SEEK_END);
			vol_info.count = ftell(vol_f) ;
		  	
		  	
			if(vol_info.count >= vol_info.max_count)
			{
				sprintf(tmp_buf,"%u,%4d,%4d\r\n",time(NULL),vol_info.vol.s3_3v,vol_info.vol.m5v) ;
				fwrite(tmp_buf , strlen(tmp_buf), 1 , vol_f) ;
				fclose(vol_f);
				system("sudo cat /home/pi/log/stm32_client/voltage.log > /home/pi/log/stm32_client/voltage.log.backup");
				system("sudo cat /dev/null > /home/pi/log/stm32_client/voltage.log ") ;
		    	vol_f  = fopen(VOLTAGE_PATH,"a+") ;
				if(vol_f == NULL)
					goto erro ;
			}
			else 
			{
				sprintf(tmp_buf,"%u,%4d,%4d\r\n",time(NULL),vol_info.vol.s3_3v,vol_info.vol.m5v) ;
				fwrite(tmp_buf , strlen(tmp_buf), 1 , vol_f) ;
			}
			fclose(vol_f);
			return MXJ_SUCCESS ;
		}
		else 	
			
			vol_info.TimeCount-- ;
			return MXJ_SUCCESS ; 
  	}
	return MXJ_SUCCESS ;
erro:
	printf("VOL_ERROR: %s %s %d %\n", __FILE__, __FUNCTION__, __LINE__) ;
	return -MXJ_FAIL ;

}


int mxj_temperature_proc()
{
	if(temp_info.running_stat == START)
	{
		unsigned char temp_tx = 0 ;
		char tmp_buf[128] ;
		FILE *temper_f = NULL ;
		FILE *temper_backup_f = NULL ;
		if(temp_info.TimeCount <= 0)
		{
			
			temp_info.TimeCount = temp_info.delay ;
			if(general_transport((temp_t*)&temp_info.temp, sizeof(temp_t) ,
				                   GTE_TEMP, DEFAULT_SUB_TYPE ,&temp_tx , 0) )

				 goto erro ;
		
			temper_f  = fopen(TEMP_PATH,"a+") ;
			if(temper_f == NULL)
				goto erro ;
			
			fseek(temper_f, 0, SEEK_END);
			temp_info.count = ftell(temper_f) ;
			
	    
			if(temp_info.count >= temp_info.max_count)
			{
	      
				sprintf(tmp_buf,"%u,%4d,%4d,%4d\r\n",time(NULL),temp_info.temp.EnvTemp,vol_info.vol.s3_3v,vol_info.vol.m5v) ;
				fwrite(tmp_buf , strlen(tmp_buf), 1 , temper_f) ;
				fclose(temper_f);
				system("sudo cat /home/pi/log/stm32_client/temperature.log > /home/pi/log/stm32_client/temperature.log.backup");
				system("sudo cat /dev/null > /home/pi/log/stm32_client/temperature.log ") ;
	      
				temper_f  = fopen(TEMP_PATH,"a+") ;
			if(temper_f == NULL)
				goto erro ;
			}
			else 
			{
				sprintf(tmp_buf,"%u,%4d,%4d,%4d\r\n",time(NULL),temp_info.temp.EnvTemp,vol_info.vol.s3_3v,vol_info.vol.m5v) ;
				fwrite(tmp_buf , strlen(tmp_buf), 1 , temper_f) ;
			}
			fclose(temper_f);
			return MXJ_SUCCESS ;
			}
			temp_info.TimeCount -- ;
	    return MXJ_SUCCESS ;
		}
	return MXJ_SUCCESS ;
erro:
	printf("TEMP_ERROR: %s %s %d \n", __FILE__, __FUNCTION__, __LINE__) ;
	return -MXJ_FAIL ;

}







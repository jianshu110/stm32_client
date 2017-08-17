#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <curl/curl.h>
#include <microhttpd.h>
#include "common.h"
#include "i2c_adaptor.h"
#include "file_process.h"
#include "json_data.h"
#include "cJSON.h"

httpd_t httpd={

	.ret_str="success" ,
	.version="v2" ,

} ;

struct MHD_Daemon *mydaemon ; 
char local_upload_data[DEFAULE_1024] ;

/*
 @json_data 转换后json保存缓冲
*/


int genenrate_json_data(char * json_data , int type)
{
	printf("genenrate_json_data\r\n");
	checked_parameter(json_data);
	cJSON * pJson = NULL;
	char * json_p ;
	unsigned char *Txbuff = 0;
	
	pJson = cJSON_CreateObject();
	if(NULL == pJson)
		goto erro ;
	if(type == GET_VOL)
	{
 		
		cJSON_AddStringToObject(pJson, "message" , httpd.ret_str);
		cJSON_AddStringToObject(pJson, "version",httpd.version);
		cJSON_AddNumberToObject(pJson, "3.3V", vol_info.vol.s3_3v);
		cJSON_AddNumberToObject(pJson, "5V", vol_info.vol.m5v);
		cJSON_AddNumberToObject(pJson, "12V",vol_info.vol.m12v);
		cJSON_AddNumberToObject(pJson, "48V",vol_info.vol.m48v);
   
	}
	if(type == GTE_TEMP)
	{

		cJSON_AddStringToObject(pJson, "message" , httpd.ret_str);
		cJSON_AddStringToObject(pJson, "version",httpd.version);
		cJSON_AddNumberToObject(pJson, "temperature", temp_info.temp.EnvTemp);
	}
    json_p= cJSON_Print(pJson);
	strcpy(json_data,json_p) ;
	
	//debug("json:%s\r\n",json_data) ;
	cJSON_Delete(pJson) ;
	return MXJ_SUCCESS  ;
	
erro:
	printf("JSON_ERROR: %s %s %d %\n", __FILE__, __FUNCTION__, __LINE__) ;
	return -MXJ_FAIL ;

}






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
#include "httpd.h"
#include "cJSON.h"


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
 
		cJSON_AddStringToObject(pJson, "message" , "success");
		cJSON_AddStringToObject(pJson, "version","v2");
		cJSON_AddNumberToObject(pJson, "3.3V", vol_info.vol.s3_3v);
		cJSON_AddNumberToObject(pJson, "5V", vol_info.vol.m5v);
		cJSON_AddNumberToObject(pJson, "12V",vol_info.vol.m12v);
		cJSON_AddNumberToObject(pJson, "48V",vol_info.vol.m48v);
   
	}
	if(type == GTE_TEMP)
	{

		cJSON_AddStringToObject(pJson, "message" , "success");
		cJSON_AddStringToObject(pJson, "version","v2");
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

int answer_to_connection(void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls)
{
       
        struct MHD_Response *response;
		
		memset(local_upload_data , 0 , DEFAULE_1024);
		if(0 == strcmp(url, "/vol"))
		{
			printf("get voltage\r\n");
			if(genenrate_json_data(local_upload_data ,GET_VOL))
				goto httpd_error ;
		}
			if(0 == strcmp(url, "/temp"))
		{
			printf("get temp\r\n");
			if(genenrate_json_data(local_upload_data ,GTE_TEMP))
				goto httpd_error ;
		}
	
        response = MHD_create_response_from_buffer(strlen (local_upload_data),
                                            (void*) local_upload_data, MHD_RESPMEM_PERSISTENT);  
	   
         int ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
                MHD_destroy_response (response);     
                    
		 MHD_destroy_response (response);
      
        return ret;
		
httpd_error:
	printf("HTTPD_ERROR: %s %s %d %\n", __FILE__, __FUNCTION__, __LINE__) ;
	return -MXJ_FAIL ;
}

int mxj_init_httpd()
{
	mydaemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
	                     &answer_to_connection, NULL, MHD_OPTION_END);
	if (NULL == mydaemon)
	        return -MXJ_FAIL ;
	return MXJ_SUCCESS ;
	
}




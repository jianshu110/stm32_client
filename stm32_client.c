#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <microhttpd.h>
#include "i2c_adaptor.h"
#include "httpd.h"
#include "file_process.h"
#include "common.h"
#include "stm32_client.h"
char httpd_data[DEFAULE_1024] ;
int main(int argc , char ** argv)
{



	int answer_to_connection (void *cls, struct MHD_Connection *connection,
	                          const char *url,
	                          const char *method, const char *version,
	                          const char *upload_data,
	                          size_t *upload_data_size, void **con_cls)
	{

		struct MHD_Response *response;
		int ret;

		if (0 == strcmp(url, "/vol") )
		{
			genenrate_json_data(httpd_data ,GET_VOL);     
		}
		else if(0 == strcmp(url, "/temp"))
		{
			genenrate_json_data(httpd_data ,GTE_TEMP);
		}

		response = MHD_create_response_from_buffer (strlen (httpd_data),
		            (void*) httpd_data, MHD_RESPMEM_PERSISTENT);
		            
		ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
		MHD_destroy_response (response);

		return ret;
	}

	struct MHD_Daemon *daemon;
	daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
	&answer_to_connection, NULL, MHD_OPTION_END);
	if (NULL == daemon)
	return MXJ_FAIL;

	while(1)
	{
		mxj_voltage_proc () ;
    	mxj_temperature_proc() ;
		sleep(1) ;
	}
}



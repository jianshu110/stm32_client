#ifndef __HTTPD_H
#define __HTTPD_H

#ifdef __cplusplus
    extern "C" {
#endif

typedef struct{
	char ret_str[16] ;
	char version[12] ;
}httpd_t ;

int mxj_init_httpd() ;
int genenrate_json_data(char * json_data , int type) ;
extern httpd_t httpd ;

#ifdef __cplusplus
}
#endif

#endif



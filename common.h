#ifndef __COMMON_H
#define __COMMON_H
#include <stdio.h>
#include "cJSON.h"


/*返回值定义*/
enum mxj_errno_e {
	MXJ_SUCCESS = 0 ,
	MXJ_FAIL ,
	MXJ_INVALID	,
	MXJ_IO_ERRO,
	MXJ_ACK_ERRO 
};

/*http 的端口定义*/
#define PORT 1006

/*功能定义*/
#define DEBUG 0
#define checked_parameter(para)  do {if((para) == NULL) return MXJ_INVALID ;} while(0)

/*时间长度定义*/	
#define GRANULARITY_MACRO_SECOND 100000
#define S_1SEC 1
#define S_2SEC 2
#define S_3SEC 3
#define U_1MSEC 1000 
#define U_10MSEC 10000 
#define U_100MSEC 100000 

/*字符的长度定义*/
#define DEFAULE_LEN 32 
#define DEFAULE_16 16
#define DEFAULE_32 32
#define DEFAULE_64 64
#define DEFAULE_128 128
#define DEFAULE_256 256
#define DEFAULE_512 512
#define DEFAULE_1024 1024
#define DEFAULE_2048 2048
#define DEFAULE_2176 2176

/*默认的帧类型*/
#define DEFAULE_SUB_TYPE 0x01 

enum type{
	
	GET_VOL = 1 ,
	GET_VOR   ,
	GET_KEY   ,
	TRAN_PRO  ,
	MAJOR_HEART_BEAT ,
	MINOR_HEART_BEAT ,
	STM32_CLIENT_UPDATE ,
	GTE_TEMP ,
};
enum boole{
	FAIL = 0 ,
	TRUE 
} ;

enum update_stat{
	DISABLE = 0 ,
	ENABLE
} ;

enum run_stat{
	STOP = 0 ,
	START 
} ;

#if 1
	#define debug(args...)  printf(args)
#else 
	#define debug 
#endif

#endif



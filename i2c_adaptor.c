#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <semaphore.h>
#include "i2c_adaptor.h"
#include "common.h"
#include <fcntl.h> 
#include "stm32_client.h"
 
unsigned char txbuff[DEFAULE_2048] ; /*i2c发送数据缓存*/ 
unsigned char rxbuff[DEFAULE_2048] ; /*i2c接收数据缓存*/

int get_head_size()
{
	return (int)sizeof(frame_head_t) ;
}

/* CRC-8 x8+x5+x4+1 0x31（0x131）*/
const unsigned char crc_table[] =
{
    0x00,0x31,0x62,0x53,0xc4,0xf5,0xa6,0x97,0xb9,0x88,0xdb,0xea,0x7d,0x4c,0x1f,0x2e,
    0x43,0x72,0x21,0x10,0x87,0xb6,0xe5,0xd4,0xfa,0xcb,0x98,0xa9,0x3e,0x0f,0x5c,0x6d,
    0x86,0xb7,0xe4,0xd5,0x42,0x73,0x20,0x11,0x3f,0x0e,0x5d,0x6c,0xfb,0xca,0x99,0xa8,
    0xc5,0xf4,0xa7,0x96,0x01,0x30,0x63,0x52,0x7c,0x4d,0x1e,0x2f,0xb8,0x89,0xda,0xeb,
    0x3d,0x0c,0x5f,0x6e,0xf9,0xc8,0x9b,0xaa,0x84,0xb5,0xe6,0xd7,0x40,0x71,0x22,0x13,
    0x7e,0x4f,0x1c,0x2d,0xba,0x8b,0xd8,0xe9,0xc7,0xf6,0xa5,0x94,0x03,0x32,0x61,0x50,
    0xbb,0x8a,0xd9,0xe8,0x7f,0x4e,0x1d,0x2c,0x02,0x33,0x60,0x51,0xc6,0xf7,0xa4,0x95,
    0xf8,0xc9,0x9a,0xab,0x3c,0x0d,0x5e,0x6f,0x41,0x70,0x23,0x12,0x85,0xb4,0xe7,0xd6,
    0x7a,0x4b,0x18,0x29,0xbe,0x8f,0xdc,0xed,0xc3,0xf2,0xa1,0x90,0x07,0x36,0x65,0x54,
    0x39,0x08,0x5b,0x6a,0xfd,0xcc,0x9f,0xae,0x80,0xb1,0xe2,0xd3,0x44,0x75,0x26,0x17,
    0xfc,0xcd,0x9e,0xaf,0x38,0x09,0x5a,0x6b,0x45,0x74,0x27,0x16,0x81,0xb0,0xe3,0xd2,
    0xbf,0x8e,0xdd,0xec,0x7b,0x4a,0x19,0x28,0x06,0x37,0x64,0x55,0xc2,0xf3,0xa0,0x91,
    0x47,0x76,0x25,0x14,0x83,0xb2,0xe1,0xd0,0xfe,0xcf,0x9c,0xad,0x3a,0x0b,0x58,0x69,
    0x04,0x35,0x66,0x57,0xc0,0xf1,0xa2,0x93,0xbd,0x8c,0xdf,0xee,0x79,0x48,0x1b,0x2a,
    0xc1,0xf0,0xa3,0x92,0x05,0x34,0x67,0x56,0x78,0x49,0x1a,0x2b,0xbc,0x8d,0xde,0xef,
    0x82,0xb3,0xe0,0xd1,0x46,0x77,0x24,0x15,0x3b,0x0a,0x59,0x68,0xff,0xce,0x9d,0xac
};


unsigned char get_crc(unsigned char  *ptr, int len) 
{
    unsigned char  crc = 0x00;
	if(len == 0 )
		return crc ;
    while (len--)
    {
        crc = crc_table[crc ^ *ptr++];
    }
    return (crc);
}


/*帧解析*/
int frame_parsing(i2c_frame_t *frame_info , unsigned char* obtain)
{
	checked_parameter(frame_info) ;
	checked_parameter(obtain) ;
	
	memset(frame_info , 0 , sizeof(i2c_frame_t));
	memcpy(&frame_info->frame_head , obtain , sizeof(frame_info->frame_head));
	
	frame_info->tran_data = (obtain+sizeof(frame_info->frame_head));

	
	return MXJ_SUCCESS ;
error:
	printf("PARSING_ERROR: %s %s %d %\n", __FILE__, __FUNCTION__, __LINE__) ;
	return -MXJ_FAIL;	
}


/*组帧*/
int frame_generate( unsigned char *generated_data , unsigned char type ,
						    unsigned char subtype ,int size , unsigned char *deliver )
{
	checked_parameter(generated_data) ;
	i2c_frame_t frame_info ;
	
	frame_info.frame_head.start_tag = (unsigned short)0XDCDA ;
	frame_info.frame_head.type = type ;
	frame_info.frame_head.suptype = subtype ;
	frame_info.frame_head.len = size ; 
	frame_info.frame_head.crc = get_crc(deliver,size);
	memset(generated_data , 0 ,(sizeof(frame_info.frame_head)+size)) ;
	memcpy(generated_data , &frame_info.frame_head , sizeof(frame_info.frame_head)) ;
	memcpy((generated_data+sizeof(frame_info.frame_head)) , deliver ,size);
	return MXJ_SUCCESS ;
	
error:
	printf("GENERATE_ERROR: %s %s %d \n", __FILE__, __FUNCTION__, __LINE__) ;
	return -MXJ_FAIL ;
}


int is_complete(unsigned char *getdata )
{
	//int i = 0 ;
	checked_parameter(getdata) ;
	
	unsigned char getCrc = 0 ;
	i2c_frame_t frame_info ;
	
	frame_parsing(&frame_info , getdata ) ;
	
/*打印接收日志*/
	
		time_t   timep;
		time   (&timep);
		printf("IS_COMPLETE_DEBUG ->\n \
		frame_head.start_tag:%x \n \
		frame_head.type     :%d \n \
		frame_head.len      :%d \n \
		time                :%s \n\n",	frame_info.frame_head.start_tag , \
						frame_info.frame_head.type, \
						frame_info.frame_head.len, \
						ctime(&timep));
	
	if(frame_info.frame_head.start_tag !=(unsigned short)0XDCDA)
		goto error ;
	
	if(frame_info.frame_head.len > (DEFAULE_2048-13 ))
		goto error ;
	
	getCrc = get_crc(frame_info.tran_data,frame_info.frame_head.len);
	
	if(getCrc != frame_info.frame_head.crc)
		goto error ;	
	return MXJ_SUCCESS ;

error:
	printf("IS_COMPLETE_ERROR: %s %s %d %\n", __FILE__, __FUNCTION__, __LINE__) ;
	return -MXJ_FAIL;
}


/*传递数据*/
int delivery(void* dest, void*src, int len)
{
	checked_parameter(dest) ;
	checked_parameter(src) ;
	
	memcpy(dest,(src+sizeof(frame_head_t)),len);
	
	return MXJ_SUCCESS ;
}

/*
   @data 发送数据缓存
   @size 接受的数据长度
*/
int mxj_i2c_send(unsigned char* data , int size) //_i2c_write
{
	checked_parameter(data);
	
	unsigned int len = 0 ;
	unsigned int i2c_fd ;
	
	
	i2c_fd = open("/dev/stm8_s103", O_WRONLY) ;  
    if (i2c_fd < 0) 
		goto error ; 
	
	len = write(i2c_fd , data , size)	;
	if (len < 0) 
		goto error ;
	
/*ok*/	
	close(i2c_fd);
	return len ;
error:
	printf("I2C_SEND_ERROR: %s %s %d %\n", __FILE__, __FUNCTION__, __LINE__) ; 
	close(i2c_fd) ;
	return -MXJ_IO_ERRO ;
}

/*i2c 读取函数*/
int _i2c_read(unsigned char *rxdata ,int rx_size )
{
	checked_parameter(rxdata);
	unsigned int i2c_fd ;
	unsigned int len ;


	i2c_fd = open("/dev/stm8_s103", O_RDWR) ;  
    if (i2c_fd < 0) 
        goto erro; 
    
	len = read(i2c_fd , rxdata , rx_size)	;
	if (len < 0) 
       goto erro ; 
	
/*ok*/
  	close(i2c_fd) ;
	return len ;
erro:
	close(i2c_fd) ;
	printf("I2C_READ_ERROR: %s %s %d \n", __FILE__, __FUNCTION__, __LINE__) ;
	return -MXJ_IO_ERRO ;
}


/*
   @获取指定的stm32数据
   @rxdata 接受数据缓存
   @txBuff 发送数据缓存
   @size 接受的数据长度
*/
int mxj_i2c_transfer(unsigned char *rxdata , unsigned char *txBuff , int rx_size ,int tx_size) 
{

	checked_parameter(rxdata);
	checked_parameter(txBuff);
	uint32_t i2c_fd ;
	uint32_t len ;
	
/*发送请求*/	
	int ret = mxj_i2c_send(txBuff,tx_size);	
	if(ret < 0 )
		goto WRITE_error;
	
/*延迟等待stm32处理数据*/		
	usleep(300000) ;
	
/*读取返回数据*/	
	i2c_fd = open("/dev/stm8_s103", O_RDWR) ;  
    if (i2c_fd < 0) 
        goto READ_error; 
     
	len = read(i2c_fd , rxdata , rx_size)	;
	if (len < 0) 
       goto READ_error; 
	
/*ok*/
	close(i2c_fd) ;
	return len ; 		
READ_error: 
	close(i2c_fd) ;
	printf("I2C_RECEIVE_ERROR: %s %s %d %\n", __FILE__, __FUNCTION__, __LINE__) ;
	return -MXJ_IO_ERRO ;
	
WRITE_error: 
	printf("I2C_RECEIVE_ERROR: %s %s %d %\n", __FILE__, __FUNCTION__, __LINE__) ;
	return -MXJ_IO_ERRO ;


}

int i2c_retry(unsigned char *rxdata , unsigned char *txBuff , int rx_size ,int tx_size)
{
	int ret ;
	ret = mxj_i2c_transfer(rxbuff , txbuff,rx_size ,tx_size); 
	if(ret < 0)
		goto error ;
	if(is_complete(rxbuff))
		goto error ;
	return MXJ_SUCCESS ;

	
error: 
printf("I2C_RETRY_ERROR: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__) ;
return -MXJ_FAIL;	
}

/*
   @通用传输接口
   
   @获取的信息缓存
*/
int general_transport(void* obtain, int obtain_len ,
								unsigned char type  ,unsigned char subtype  , 
								unsigned char* send_data ,int send_len ) 
{

	i2c_frame_t *frame_info ;
	int ret ;
	int RetryCount = 0 ;//i2c接受出错重试次数
	
/*全局变量初始化*/
	memset(rxbuff , 0 , DEFAULE_2048) ;	
	memset(txbuff , 0 , DEFAULE_2048) ;
	
	if(frame_generate(txbuff, type , subtype,send_len,send_data) )
		goto error ;
	
/*接收数据*/
	if(obtain_len != 0)
	{	
		ret = mxj_i2c_transfer(rxbuff , txbuff,obtain_len+get_head_size() ,send_len+get_head_size()); 
		if(ret < 0)
			goto error ;		
		
		if(is_complete(rxbuff))
		{
			//retry=1;
			for(RetryCount ; RetryCount >= 3 ;RetryCount++) //重试3次
			{
				if(!i2c_retry(rxbuff , txbuff,obtain_len+get_head_size() ,send_len+get_head_size())) 
					break ;
				if(RetryCount >= 3)
					goto error ;
			}
			
		}
		if(delivery( obtain,rxbuff,obtain_len))
			goto error ;
/*ok*/		
		return MXJ_SUCCESS ;
	}
	
/*发送数据*/	
	else
	{
		ret = mxj_i2c_send(txbuff , send_len+get_head_size()) ;
		if(ret < 0)
			goto error ;
/*ok*/		
		return MXJ_SUCCESS ;
	}
	
error: 
	printf("GENERAL_TRANS_ERROR: %s %s %d %\n", __FILE__, __FUNCTION__, __LINE__) ;
	return -MXJ_FAIL;	
}





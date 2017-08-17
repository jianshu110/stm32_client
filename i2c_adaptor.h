#ifndef __I2C_ADAPTOR_H
#define __I2C_ADAPTOR_H

#ifdef __cplusplus
    extern "C" {
#endif
/*
  @֡��ʽ
*/

/*֡ͷ*/
typedef struct{

	unsigned short start_tag  ;
	unsigned char type ;
	unsigned char suptype ;
	int len ;
	char obligate[3] ;
	unsigned char crc ;
	
}frame_head_t ;


/*��֡*/
typedef struct {
	int retry ;
	frame_head_t frame_head ;
	unsigned char * tran_data ;	
	
} i2c_frame_t ;


#ifdef __cplusplus
}
#endif

#endif



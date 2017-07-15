#ifndef __FILE_PROCESS_H
#define __FILE_PROCESS_H


#ifdef __cplusplus
    extern "C" {
#endif

#include "common.h"
#include "i2c_adaptor.h"

#define TEMP_PATH "/home/pi/log/stm32_client/temperature.log"
#define TEMP_BACKUP_PATH "/home/pi/log/stm32_client/temperature.log.backup"

#define VOLTAGE_PATH "/home/pi/log/stm32_client/voltage.log"
#define VOLTAGE_BACKUP_PATH "/home/pi/log/stm32_client/voltage.log.backup"

#define FILE_DEFAULE_LEN 32
#define FILE_HEAD_LEN 4
#define RECARD_PERIOD 20
#define TOTAL (3*60*24*30) /*20ms*/


#define LOG_GET_VOR GET_VOR
#define LOG_GET_VOL GET_VOL
#define LOG_SUBTYPE_DEFAULT SUBTYPE_DEFAULT
#define LOG_RECORD_OFFSET_ADDR 12
#define LOG_RECORD_OFFSET_LEN 4

/*�ɼ����¶���Ϣ*/
typedef struct{
	
	char EnvTemp ;  /*�Ž����ڲ��¶�*/
	char obligate[3] ;
	
} temp_t ;

typedef struct {
	int running_stat ;
	int delay ; /*����ʱ����*/
	int TimeCount ;
	int time ;
	temp_t temp ;
	int count ; /*��ǰ��¼�ĸ���*/
	int max_count ;
	char log_path[128] ; /*log��·��*/
} temp_log_t ;


/*�ɼ��ĵ�ѹ��Ϣ*/
typedef struct{

	short s3_3v;
	short m5v ;
	short m12v ;
	unsigned short m48v ;

} vol_t ;

typedef struct {
	int running_stat ;
	int delay ; /*����ʱ����*/
	int TimeCount ;
	vol_t vol ;
	long	time ;
	int count ; /*��ǰ��¼�ĸ���*/
	int max_count ;
	char log_path[128] ; /*log��·��*/

}vol_log_t ;



int mxj_temperature_proc() ;
int mxj_voltage_proc () ;

extern temp_log_t temp_info ;
extern vol_log_t vol_info ;
#ifdef __cplusplus
}
#endif


#endif


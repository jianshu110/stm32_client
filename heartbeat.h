#ifndef __HEARD_BEAT_H
#define __HEARD_BEAT_H


#ifdef __cplusplus
    extern "C" {
#endif

typedef struct{
int running_stat;
int delay ; /*����ʱ����*/
int TimeCount ;
int err_count ;/*stm32�������ش������*/
int err_max ;/*stm32�������ش����������*/
}heardbeat_t ;

#ifdef __cplusplus
}
#endif

int mxj_ma_heartbeat_proc() ;
extern heardbeat_t MaHeardBeat ;

#endif


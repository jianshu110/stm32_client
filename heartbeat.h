#ifndef __HEARD_BEAT_H
#define __HEARD_BEAT_H


#ifdef __cplusplus
    extern "C" {
#endif

typedef struct{
int running_stat;
int delay ; /*运行时间间隔*/
int TimeCount ;
int err_count ;/*stm32连续返回错误次数*/
int err_max ;/*stm32连续返回错误的最大次数*/
}heardbeat_t ;

#ifdef __cplusplus
}
#endif

int mxj_ma_heartbeat_proc() ;
extern heardbeat_t MaHeardBeat ;

#endif


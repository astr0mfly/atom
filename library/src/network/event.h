#ifndef _SM_EVENT_H_ 
#define _SM_EVENT_H_

#ifdef _WIN32
#include <Windows.h>
#define event_handle HANDLE
#elif __unix__
#include <pthread.h>
typedef struct
{
    bool            bState;
    bool            bManualReset;
    pthread_mutex_t mtxLock;
    pthread_cond_t  condEvent;
}event_t;
#define event_handle event_t*
#endif

//返回值：NULL 出错
event_handle create_EVENT(bool ManualReset_F, bool InitState_F);

//返回值：0 等到事件，-1出错
int wait_EVENT(event_handle Event_F);

//返回值：0 等到事件，1 超时，-1出错
int timedWait_EVENT(event_handle Event_F, long Milliseconds_F);

//返回值：0 成功，-1出错
int set_EVENT(event_handle Event_F);

//返回值：0 成功，-1出错
int reset_EVENT(event_handle Event_F);

//返回值：无
void destroy_EVENT(event_handle Event_F);
#endif  //_SM_EVENT_H_
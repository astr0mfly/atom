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

//����ֵ��NULL ����
event_handle create_EVENT(bool ManualReset_F, bool InitState_F);

//����ֵ��0 �ȵ��¼���-1����
int wait_EVENT(event_handle Event_F);

//����ֵ��0 �ȵ��¼���1 ��ʱ��-1����
int timedWait_EVENT(event_handle Event_F, long Milliseconds_F);

//����ֵ��0 �ɹ���-1����
int set_EVENT(event_handle Event_F);

//����ֵ��0 �ɹ���-1����
int reset_EVENT(event_handle Event_F);

//����ֵ����
void destroy_EVENT(event_handle Event_F);
#endif  //_SM_EVENT_H_
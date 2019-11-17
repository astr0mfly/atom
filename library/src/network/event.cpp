#include "event.h"

#ifdef __unix__

#include <sys/time.h>
#include <errno.h>

#endif

#include <iostream>

event_handle create_EVENT(bool ManualReset_F, bool InitState_F)
{

#ifdef _WIN32
    HANDLE hEvent = CreateEvent(NULL, ManualReset_F, InitState_F, NULL);
#elif __unix__

    event_handle hEvent = new(std::nothrow) event_t;
    if (hEvent == NULL) {
        return NULL;
    }

    hEvent->bState = InitState_F;
    hEvent->bManualReset = ManualReset_F;

    if (pthread_mutex_init(&hEvent->mtxLock, NULL)) {
        delete hEvent;
        return NULL;

    }

    if (pthread_cond_init(&hEvent->condEvent, NULL)) {
        pthread_mutex_destroy(&hEvent->mtxLock);
        delete hEvent;
        return NULL;
    }
#endif

    return hEvent;
}

int wait_EVENT(event_handle Event_F)
{

#ifdef _WIN32

    DWORD ret = WaitForSingleObject(Event_F, INFINITE);
    if (ret == WAIT_OBJECT_0) {
        return 0;
    }

    return -1;
#elif __unix__

    if (pthread_mutex_lock(&Event_F->mtxLock)) {
        return -1;
    }

    while (!Event_F->bState) {
        if (pthread_cond_wait(&Event_F->condEvent, &Event_F->mtxLock)) {
            pthread_mutex_unlock(&Event_F->mtxLock);
            return -1;
        }
    }

    if (!Event_F->bManualReset) {
        Event_F->bState = false;
    }

    if (pthread_mutex_unlock(&Event_F->mtxLock)) {
        return -1;
    }

    return 0;
#endif
}

int timedWait_EVENT(event_handle Event_F, long Milliseconds_F)
{
#ifdef _WIN32
    DWORD ret = WaitForSingleObject(Event_F, Milliseconds_F);
    if (ret == WAIT_OBJECT_0) {
        return 0;
    }
    if (ret == WAIT_TIMEOUT) {
        return 1;
    }
    return -1;

#elif __unix__
    int rc = 0;
    struct timespec abstime;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    abstime.tv_sec = tv.tv_sec + Milliseconds_F / 1000;
    abstime.tv_nsec = tv.tv_usec * 1000 + (Milliseconds_F % 1000) * 1000000;
    if (abstime.tv_nsec >= 1000000000) {
        abstime.tv_nsec -= 1000000000;
        abstime.tv_sec++;
    }

    if (pthread_mutex_lock(&Event_F->mtxLock) != 0) {
        return -1;
    }

    while (!Event_F->bState) {
        if (rc = pthread_cond_timedwait(&Event_F->condEvent,
                                        &Event_F->mtxLock, &abstime)) {
            if (rc == ETIMEDOUT) break;
            pthread_mutex_unlock(&Event_F->mtxLock);
            return -1;
        }
    }

    if (rc == 0 && !Event_F->bManualReset) {
        Event_F->bState = false;
    }

    if (pthread_mutex_unlock(&Event_F->mtxLock) != 0) {
        return -1;
    }

    if (rc == ETIMEDOUT) {
        //timeout return 1
        return 1;
    }

    //wait event success return 0
    return 0;
#endif
}

int set_EVENT(event_handle Event_F)
{
#ifdef _WIN32

    return !SetEvent(Event_F);

#elif __unix__
    if (pthread_mutex_lock(&Event_F->mtxLock) != 0) {
        return -1;
    }

    Event_F->bState = true;

    if (Event_F->bManualReset) {
        if (pthread_cond_broadcast(&Event_F->condEvent)) {
            return -1;
        }
    }
    else {
        if (pthread_cond_signal(&Event_F->condEvent)) {
            return -1;
        }
    }

    if (pthread_mutex_unlock(&Event_F->mtxLock) != 0) {
        return -1;
    }

    return 0;
#endif
}

int reset_EVENT(event_handle Event_F)
{
#ifdef _WIN32

    //ResetEvent 返回非零表示成功
    if (ResetEvent(Event_F)) {
        return 0;
    }

    return -1;

#elif __unix__
    if (pthread_mutex_lock(&Event_F->mtxLock) != 0) {
        return -1;
    }

    Event_F->bState = false;

    if (pthread_mutex_unlock(&Event_F->mtxLock) != 0) {
        return -1;
    }

    return 0;

#endif
}

void destroy_EVENT(event_handle Event_F)
{
#ifdef _WIN32

    CloseHandle(Event_F);

#elif __unix__

    pthread_cond_destroy(&Event_F->condEvent);
    pthread_mutex_destroy(&Event_F->mtxLock);

    delete Event_F;

#endif
}
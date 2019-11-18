#ifndef MUTEX_PROCESS_H_
#define MUTEX_PROCESS_H_

#include <algorithm>

#ifdef __unix__
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <memory.h>
#elif _WIN32

#include <WinSock2.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

/*

The name can have a "Global\" or "Local\" prefix to explicitly create the object in the global or session name space. The remainder of the name can contain any character except the backslash character (\). For more information, see Kernel Object Namespaces.
 Fast user switching is implemented using Terminal Services sessions. Kernel object names must follow the guidelines outlined for Terminal Services so that applications can support multiple users.
以上内容说明Mutex的对象名存在命名空间的区分，如果想要在多个session中使用同一个命名互斥体，应使用“Global\”前缀，

#define MUTEX_NAME _T("Global\\TestGlobalMutex")
*/
class MutexProcess
{
public:
    explicit MutexProcess(const char *name=nullptr);
    ~MutexProcess();

    bool lock(DWORD dwTime);
    bool unlock();

private:
    char m_acMutexName[32]; //锁的名字

#ifdef __unix__
    sem_t* m_pSem;          //锁的操作指针
#elif _WIN32
    HANDLE m_hMutex;        //锁的句柄
#endif
};

#endif  //MUTEX_PROCESS_H_
#include "mutex_process.h"

#pragma warning(disable:4800)

#ifdef __unix__

MutexProcess::MutexProcess(const char* name)
{
    memset(m_acMutexName, 0, sizeof(m_acMutexName));
    int min = strlen(name) > (sizeof(m_acMutexName) - 1) ? (sizeof(m_acMutexName) - 1) : strlen(name);
    strncpy(m_acMutexName, name, min);
    m_pSem = sem_open(name, O_RDWR | O_CREAT, 0644, 1);
}

MutexProcess::~MutexProcess()
{
    int ret = sem_close(m_pSem);
    if (0 != ret)
    {
        printf("sem_close error %d\n", ret);
    }
    sem_unlink(m_acMutexName);
}

bool MutexProcess::Lock()
{
    int ret = sem_wait(m_pSem);
    if (ret != 0)
    {
        return false;
    }
    return true;
}

bool MutexProcess::UnLock()
{
    int ret = sem_post(m_pSem);
    if (ret != 0)
    {
        return false;
    }
    return true;
}

#elif _WIN32

/*
@brief      : Constructors
@param      :
*/
MutexProcess::MutexProcess(const char* name)
{
    UINT iMtxNameLen = sizeof(m_acMutexName);
    std::fill(m_acMutexName, m_acMutexName + iMtxNameLen, NULL);
    UINT min = strlen(name) > (--iMtxNameLen) ? iMtxNameLen : strlen(name);
    std::copy(name, name + min, m_acMutexName);
    m_hMutex = CreateMutex(NULL, false, m_acMutexName);
}

/*
@brief      : Destructor
@param      :
*/
MutexProcess::~MutexProcess()
{
    CloseHandle(m_hMutex);
}

/*
@brief      : lock and while try to lock until timeout
@param      :
*/
bool MutexProcess::lock(DWORD dwTime)
{
    //»¥³âËø´´½¨Ê§°Ü
    if (INVALID_HANDLE_VALUE == m_hMutex) {
        return false;
    }

    DWORD dwRet = ::WaitForSingleObject(m_hMutex, dwTime);
    return (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED);
}

/*
@brief      : ReleaseMutex
@param      :
*/
bool MutexProcess::unlock()
{
    return (bool)::ReleaseMutex(m_hMutex);
}

#endif

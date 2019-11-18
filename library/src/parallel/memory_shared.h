#ifndef MEMORY_SHARED_H_
#define MEMORY_SHARED_H_


#include <WinSock2.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <string>
#include <process.h>

#include "mutex_process.h"

/**
*@brief     : MemoryShared
*@detail    : 共享内存类
*/
class MemoryShared
{
public:
    MemoryShared();
    MemoryShared(const std::string&, DWORD, BOOL = FALSE);
    ~MemoryShared();

    BOOL Open(const std::string& strName, BOOL bReadOnly);
    BOOL MapAt(DWORD dwOffset, DWORD dwSize);
    void Unmap();
    LPVOID GetMemory() const;
    HANDLE GetHandle() const;
    BOOL Lock(DWORD dwTime);
    void Unlock();
    void Close();

private:
    MutexProcess   *m_pinstLock;    //创建进程锁
    HANDLE          m_hFileMap;
    LPVOID          m_pMemory;
    std::wstring    m_strName;
    BOOL            m_bReadOnly;
    DWORD           m_dwMappedSize;

    MemoryShared(const MemoryShared& other)=delete;
    MemoryShared& operator= (const MemoryShared& other)=delete;
};

#endif  //MEMORY_SHARED_H_
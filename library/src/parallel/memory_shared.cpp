#include "memory_shared.h"

/*
@brief      : Constructor
@param      :
*/
MemoryShared::MemoryShared() :
    m_pinstLock(nullptr)
{ }

/*
@brief      : reload Constructor
@param      :
*/
MemoryShared::MemoryShared(const std::string& strLockName
                           , DWORD dwSize, BOOL bReadOnly) :
    m_hFileMap(NULL),
    m_pMemory(NULL),
    m_bReadOnly(bReadOnly),
    m_dwMappedSize(dwSize)
{
    m_pinstLock = new MutexProcess(strLockName.c_str());
}

/*
@brief      : Destructor
@param      :
*/
MemoryShared::~MemoryShared()
{

    Close();//换成手动析构，不然重复释放了。

    if (m_pinstLock) {
        delete m_pinstLock;
    }
}

/*
@brief      : find a shared memory or create it
@param      : [in] const std::string& strName, BOOL bReadOnly
*/
BOOL MemoryShared::Open(const std::string& strName, BOOL bReadOnly)
{
    m_hFileMap = ::OpenFileMapping(bReadOnly ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS,
                                   FALSE,
                                   strName.empty() ? NULL : strName.c_str());
    if (!m_hFileMap) {
        if (m_dwMappedSize <= 0) {
            return FALSE;
        }
        m_hFileMap = ::CreateFileMapping(INVALID_HANDLE_VALUE,
                                         NULL,
                                         PAGE_READWRITE,
                                         0,
                                         m_dwMappedSize,
                                         strName.empty() ? NULL : strName.c_str());
        if (!m_hFileMap) {
            printf("%d\n", GetLastError());
            Close();
            return FALSE;
        }
    }

    m_bReadOnly = bReadOnly;
    return TRUE;
}

/*
@brief      : create a handle that mapping from kernel memory
@param      : [in] const std::string& strName, BOOL bReadOnly
*/
BOOL MemoryShared::MapAt(DWORD dwOffset, DWORD dwSize)
{
    if (!m_hFileMap) {
        return FALSE;
    }

    if (dwSize > ULONG_MAX) {
        return FALSE;
    }

    ULARGE_INTEGER ui;//一个64位的无符号整型值
    ui.QuadPart = static_cast<ULONGLONG>(dwOffset);
    m_pMemory = ::MapViewOfFile(m_hFileMap,
                                m_bReadOnly ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS,
                                ui.HighPart,
                                ui.LowPart,
                                dwSize);
    return (m_pMemory != NULL);
}

/*
@brief      : Unbundling mapping
@param      : [in]
*/
void MemoryShared::Unmap()
{
    if (m_pMemory) {
        ::UnmapViewOfFile(m_pMemory);
        m_pMemory = NULL;
    }
}

/*
@brief      : GetMemory
@param      :
*/
LPVOID MemoryShared::GetMemory() const
{
    return m_pMemory;
}

/*
@brief      : GetHandle of memory
@param      :
*/
HANDLE MemoryShared::GetHandle() const
{
    return m_hFileMap;
}

/*
@brief      : process lock, used to protect memory
@param      : [in] dwTime
*/
BOOL MemoryShared::Lock(DWORD dwTime)
{
    if (!m_pinstLock) {
        return m_pinstLock->lock(dwTime);
    }

    return FALSE;
}

/*
@brief      : process unlock
@param      : 
*/
void MemoryShared::Unlock()
{
    if (!m_pinstLock) {
        m_pinstLock->unlock();
    }
}

/*
@brief      : release resource
@param      :
*/
void MemoryShared::Close()
{
    Unmap();

    if (m_hFileMap) {
        ::CloseHandle(m_hFileMap);
        m_hFileMap = NULL;
    }
}
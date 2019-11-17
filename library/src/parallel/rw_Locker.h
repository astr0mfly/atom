#ifndef COMM_RW_LOCKER_H_
#define COMM_RW_LOCKER_H_

#include <atomic>
#include <thread>

/*
写独占，读共享。
能嵌套加锁，
*/

#define LOCKER_STATUS_WRITE (-1)
#define LOCKER_STATUS_FREE (0)
#define LOCKER_STATUS_SHARE (1) //正整数都可以

/**
*@brief     : RwLocker
*@detail    : 读写锁的对象
*/
class RwLocker
{
public:
    RwLocker(bool writeFirst=false);
    RwLocker(const RwLocker &)=delete;
    RwLocker &operator=(const RwLocker &)=delete;
    virtual ~RwLocker()=default;

    int readLock();
    int readUnlock();
    int writeLock();
    int writeUnlock();

private:
    static const std::thread::id scm_NullThread;    //空的线程ID,用来初始化写锁的ID
    const bool          cm_bIsWriteFisrt;           //是否写优先模式
    std::thread::id     m_instWriteThreadId;        //写线程的ID,用于区分读/写线程
    std::atomic_int     m_atmLockCount;             //读锁计数
    std::atomic_uint    m_atmWriteWaitCount;        //写锁等待的计数
};


#endif //COMM_RW_LOCKER_H_
#include "rw_locker.h"
#include <cstdlib>
#include <cassert>
#include <stdexcept>

const std::thread::id RwLocker::scm_NullThread;

/**
*@brief     : RwLocker::RwLocker
*@param     : [i]bool writeFirst
*@note      : 构造函数
*@return    :
*/
RwLocker::RwLocker(bool writeFirst) :
    cm_bIsWriteFisrt(writeFirst),
    m_instWriteThreadId(),
    m_atmLockCount(0),
    m_atmWriteWaitCount(0)
{ }

/**
*@brief     : RwLocker::readLock
*@param     : 
*@note      : 读锁
*@return    : 锁的计数
*/
int RwLocker::readLock() {
    // ==时为独占写状态,不需要加锁
    if (std::this_thread::get_id() != m_instWriteThreadId) {
        int count;
        if (cm_bIsWriteFisrt) {//写优先模式下,要检测等待写的线程数为0(m_atmWriteWaitCount==0)
            do {
                while ((count = m_atmLockCount) == LOCKER_STATUS_WRITE
                       || m_atmWriteWaitCount > 0)
                { }//写锁定时等待
            } while (!m_atmLockCount.compare_exchange_weak(count, count + 1));
        }
        else {
            do {
                while ((count = m_atmLockCount) == LOCKER_STATUS_WRITE)
                { } //写锁定时等待
            } while (!m_atmLockCount.compare_exchange_weak(count, count + 1));
        }
    }
    return m_atmLockCount;
}

/**
*@brief     : RwLocker::readLock
*@param     :
*@note      : 读锁解除
*@return    : 当前读锁的计数
*/
int RwLocker::readUnlock() {
    // ==时为独占写状态,不需要加锁
    if (std::this_thread::get_id() != m_instWriteThreadId) {
        --m_atmLockCount;
    }

    return m_atmLockCount;
}

/**
*@brief     : RwLocker::readLock
*@param     :
*@note      : 增加写锁
*@return    : 当前读锁的计数
*/
int RwLocker::writeLock() {
    // ==时为独占写状态,避免重复加锁
    if (std::this_thread::get_id() != m_instWriteThreadId) {
        ++m_atmWriteWaitCount;//写等待计数器加1
        // 没有线程读取时(加锁计数器为0)，置为-1加写入锁，否则等待
        for (int zero = LOCKER_STATUS_FREE;
             !this->m_atmLockCount.compare_exchange_weak(zero, LOCKER_STATUS_WRITE);
             zero = LOCKER_STATUS_FREE) {
            ;
        }

        --m_atmWriteWaitCount;//获取锁后,计数器减1
        m_instWriteThreadId = std::this_thread::get_id();
    }

    return m_atmLockCount;
}

/**
*@brief     : RwLocker::writeUnlock
*@param     :
*@note      : 写锁解除
*@return    : 当前读锁的计数
*/
int RwLocker::writeUnlock() {
    if (std::this_thread::get_id() != m_instWriteThreadId) {
        throw std::runtime_error("writeLock/Unlock mismatch");
    }
    assert(LOCKER_STATUS_WRITE == m_atmLockCount);
    m_instWriteThreadId = scm_NullThread;
    m_atmLockCount.store(LOCKER_STATUS_FREE);

    return m_atmLockCount;
}
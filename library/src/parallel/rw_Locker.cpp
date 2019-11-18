#include "rw_locker.h"
#include <cstdlib>
#include <cassert>
#include <stdexcept>

const std::thread::id RwLocker::scm_NullThread;

/**
*@brief     : RwLocker::RwLocker
*@param     : [i]bool writeFirst
*@note      : ���캯��
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
*@note      : ����
*@return    : ���ļ���
*/
int RwLocker::readLock() {
    // ==ʱΪ��ռд״̬,����Ҫ����
    if (std::this_thread::get_id() != m_instWriteThreadId) {
        int count;
        if (cm_bIsWriteFisrt) {//д����ģʽ��,Ҫ���ȴ�д���߳���Ϊ0(m_atmWriteWaitCount==0)
            do {
                while ((count = m_atmLockCount) == LOCKER_STATUS_WRITE
                       || m_atmWriteWaitCount > 0)
                { }//д����ʱ�ȴ�
            } while (!m_atmLockCount.compare_exchange_weak(count, count + 1));
        }
        else {
            do {
                while ((count = m_atmLockCount) == LOCKER_STATUS_WRITE)
                { } //д����ʱ�ȴ�
            } while (!m_atmLockCount.compare_exchange_weak(count, count + 1));
        }
    }
    return m_atmLockCount;
}

/**
*@brief     : RwLocker::readLock
*@param     :
*@note      : �������
*@return    : ��ǰ�����ļ���
*/
int RwLocker::readUnlock() {
    // ==ʱΪ��ռд״̬,����Ҫ����
    if (std::this_thread::get_id() != m_instWriteThreadId) {
        --m_atmLockCount;
    }

    return m_atmLockCount;
}

/**
*@brief     : RwLocker::readLock
*@param     :
*@note      : ����д��
*@return    : ��ǰ�����ļ���
*/
int RwLocker::writeLock() {
    // ==ʱΪ��ռд״̬,�����ظ�����
    if (std::this_thread::get_id() != m_instWriteThreadId) {
        ++m_atmWriteWaitCount;//д�ȴ���������1
        // û���̶߳�ȡʱ(����������Ϊ0)����Ϊ-1��д����������ȴ�
        for (int zero = LOCKER_STATUS_FREE;
             !this->m_atmLockCount.compare_exchange_weak(zero, LOCKER_STATUS_WRITE);
             zero = LOCKER_STATUS_FREE) {
            ;
        }

        --m_atmWriteWaitCount;//��ȡ����,��������1
        m_instWriteThreadId = std::this_thread::get_id();
    }

    return m_atmLockCount;
}

/**
*@brief     : RwLocker::writeUnlock
*@param     :
*@note      : д�����
*@return    : ��ǰ�����ļ���
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
#ifndef COMM_RW_LOCKER_H_
#define COMM_RW_LOCKER_H_

#include <atomic>
#include <thread>

/*
д��ռ��������
��Ƕ�׼�����
*/

#define LOCKER_STATUS_WRITE (-1)
#define LOCKER_STATUS_FREE (0)
#define LOCKER_STATUS_SHARE (1) //������������

/**
*@brief     : RwLocker
*@detail    : ��д���Ķ���
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
    static const std::thread::id scm_NullThread;    //�յ��߳�ID,������ʼ��д����ID
    const bool          cm_bIsWriteFisrt;           //�Ƿ�д����ģʽ
    std::thread::id     m_instWriteThreadId;        //д�̵߳�ID,�������ֶ�/д�߳�
    std::atomic_int     m_atmLockCount;             //��������
    std::atomic_uint    m_atmWriteWaitCount;        //д���ȴ��ļ���
};


#endif //COMM_RW_LOCKER_H_
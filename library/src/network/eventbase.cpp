#include "eventbase.hpp"

#include <vector>
#include <algorithm>
#include "../include/unistd.h"
#include "../include/bounded_queue.hpp"
#include "../include/thread_pool.hpp"
#include "time.hpp"
#include "poller.hpp"
#include "channel.hpp"
#include "socket.hpp"

#ifdef WIN32
#include <windows.h>
#elif __unix__
#include <pthread.h>
#endif
inline unsigned int getThreadId()
{
#ifdef WIN32
    return ::GetCurrentThreadId();
#else
    return thread_self();
#endif
}
namespace net {

static const int sc_iMaxTimeout = 0x7FFFFFFF;

EventBase::EventBase(int ThreadNum_F, int QueueSize_F) :
    m_bRunning__(true),
    m_pinstPoller__(new Poller()),
    m_iNextTimeOut__(sc_iMaxTimeout),
    m_uiSeq__(0),
    m_pinstTasks__(new BoundedQueue<Task>(QueueSize_F)),
    m_pinstPool__(new ThreadPool<Task>(m_pinstTasks__, ThreadNum_F)),
    m_qwPid__(getThreadId())
{ }

EventBase::~EventBase()
{
    delete m_pinstPool__;
    delete m_pinstTasks__;
    delete m_pinstPoller__;
}

Poller* EventBase::getPoller()
{
	return m_pinstPoller__;
}

void EventBase::loop()
{
	while (m_bRunning__.load()) {
        m_pinstPoller__->loopOnce(min(3000, m_iNextTimeOut__));
		__handleTimeout();
	}

	if (m_iNextTimeOut__ != sc_iMaxTimeout) {
        m_pinstPoller__->loopOnce(m_iNextTimeOut__);
		__handleTimeout();
	}
    else {
        m_pinstPoller__->loopOnce(0);
	}
    m_mtxAction__.lock();
    if (m_mapRepeatTasks__.size()) {
        m_mapRepeatTasks__.clear();
    }
    if (m_mapPendingTasks__.size()) {
        m_mapPendingTasks__.clear();
    }
    m_mtxAction__.unlock();

	m_pinstPool__->clear();
}

void EventBase::exit()
{

	if (!m_bRunning__.load()) {
		return ;
	}
    m_bRunning__ = false;
    m_mtxAction__.lock();
	__wakeUp();
    m_mtxAction__.unlock();
}

//�����Ƿ���һ�����ֽڣ����������¼���selector����������¼��Լ������Ϣ����ط�Χ����Ϣ��
//net������������������ԣ�ֱ�����������64���¼���
void EventBase::__wakeUp()
{ }

void EventBase::__refresh()
{
	if (m_mapPendingTasks__.empty()) {
		m_iNextTimeOut__ = sc_iMaxTimeout;//��ʼ��
	}
    else {
		const auto it = m_mapPendingTasks__.begin()->first;
		int64_t tmp = it.first - Time::now();
        m_iNextTimeOut__ = (tmp <= 0) ? 0 : int(tmp);
	}
}

void EventBase::runNow(Task &&Task_F)
{
	runAfter(0, std::forward<Task>(Task_F));
}

TimerId EventBase::runAfter(int64_t MilliSec_F, Task &&Task_F)
{
	if (!m_bRunning__.load()) {
		return TimerId();
	}
	TimerId TimerId_F({ Time::now() + MilliSec_F, m_uiSeq__++});
	m_mtxAction__.lock();
    m_mapPendingTasks__.insert({TimerId_F, std::move(Task_F)});
    if (getThreadId() != m_qwPid__) {
        __wakeUp();
    }
    else {
        __refresh();
    }

	m_mtxAction__.unlock();
	return TimerId_F;
}

TimerId EventBase::runEvery(int64_t MilliSec_F, Task &&Task_F)
{
	if (!m_bRunning__) {
		return TimerId();
	}
	uint32_t seq = m_uiSeq__++;
	TimeRep rep({MilliSec_F, Time::now()});
	TimerId TimerId_F({rep.second, seq});
    m_mtxAction__.lock();
	m_mapRepeatTasks__.insert({seq, rep});
	m_mapPendingTasks__.insert({TimerId_F, std::move(Task_F)});
    if (getThreadId() != m_qwPid__) {
        __wakeUp();
    }
    else {
        __refresh();
    }
	m_mtxAction__.unlock();
	return {MilliSec_F, seq};
}
//ˢ��TimerId:TimerId_F��ʱ���Task_F������pending
void EventBase::rescheduleAfter(TimerId *TimerId_F, int64_t MilliSec_F, Task &&Task_F)
{
	if (!m_bRunning__) {
		return ;
	}
	TimerId nid({Time::now() + MilliSec_F, m_uiSeq__++});

    m_mtxAction__.lock();
	const auto &it = m_mapPendingTasks__.find(*TimerId_F);
    if (it != m_mapPendingTasks__.end()) {
        m_mapPendingTasks__.erase(it);
    }
	m_mapPendingTasks__.insert({nid, std::move(Task_F)});
	*TimerId_F = nid;
    if (getThreadId() != m_qwPid__) {
        __wakeUp();
    }
    else {
        __refresh();
    }

	m_mtxAction__.unlock();
}
//ˢ��TimerId:TimerId_F��ʱ�䣬����pending
void EventBase::rescheduleAfter(const TimerId &TimerId_F, int64_t MilliSec_F)
{
	if (!m_bRunning__) {
		return ;
	}
	TimerId nid({ Time::now() + MilliSec_F, m_uiSeq__++});
    m_mtxAction__.lock();
	auto it = m_mapPendingTasks__.find(TimerId_F);
	if (it == m_mapPendingTasks__.end()) {
		m_mtxAction__.unlock();
		return ;
	}
	Task Task_F = std::move(it->second);
	m_mapPendingTasks__.erase(it);
	m_mapPendingTasks__.insert({nid, std::move(Task_F)});
    if (getThreadId() != m_qwPid__) {
        __wakeUp();
    }
    else {
        __refresh();
    }

	m_mtxAction__.unlock();
}

//�Ƴ�����
void EventBase::cancel(const TimerId &TimerId_F)
{
	m_mtxAction__.lock();
	auto instReapTask = m_mapRepeatTasks__.find(TimerId_F.second);
	if (instReapTask != m_mapRepeatTasks__.end()) {
		auto instPendTask = m_mapPendingTasks__.find({instReapTask->second.second, TimerId_F.second});
        if (instPendTask != m_mapPendingTasks__.end()) {
            m_mapPendingTasks__.erase(instPendTask);
        }
        m_mapRepeatTasks__.erase(instReapTask);
	}
    else {
		auto instPendTask = m_mapPendingTasks__.find(TimerId_F);
        if (instPendTask != m_mapPendingTasks__.end()) {
            m_mapPendingTasks__.erase(instPendTask);
        }
	}
	m_mtxAction__.unlock();
}

//���¼���ʱ�䵽�ִ������
void EventBase::__handleTimeout()
{
	TimerId now({ Time::now(), 0xFFFFFFFF});
	std::vector<Task> expired;//���ڵ�����
	m_mtxAction__.lock();
	for (; m_bRunning__.load() && !m_mapPendingTasks__.empty() && m_mapPendingTasks__.begin()->first <= now; ) {
		expired.push_back(m_mapPendingTasks__.begin()->second);
		const TimerId &TimerId_F = m_mapPendingTasks__.begin()->first;
		auto it = m_mapRepeatTasks__.find(TimerId_F.second);//��repeat����pendding�����
		if (it != m_mapRepeatTasks__.end()) {
			TimerId nid { now.first + it->second.first, TimerId_F.second };
			it->second.second = nid.first;//ˢ��ʱ��
			m_mapPendingTasks__.insert({nid, std::move(m_mapPendingTasks__.begin()->second)});//����pending
		}
		m_mapPendingTasks__.erase(m_mapPendingTasks__.begin());//ɾ��ԭ��pengding,��������
	}
	__refresh();
	m_mtxAction__.unlock();
    for (uint32_t i = 0; i < expired.size(); ++i) {
        m_pinstTasks__->push(std::move(expired[i]));//��ɸѡ�������������С��༴�̳߳�
    }
}

} // namespace net

#ifndef _EVENT_BASE_HPP_
#define _EVENT_BASE_HPP_

#include <map>
#include <mutex>
#include <atomic>

#include "..\..\rsc\common\utility.h"

namespace net {

class Poller;
class Channel;
template<typename T> class ThreadPool;
template<typename T> class BoundedQueue;

typedef std::pair<int64_t, uint32_t> TimerId;//first:时间生效的计数，second,当前Task_F的序号。
typedef std::pair<int64_t, int64_t>  TimeRep;//first:loop的时间间隔，second，当前Task_F的序号。

class EventBase : private NoCopy
{
public:
	EventBase(int ThreadNum_F, int QueueSize_F);
	~EventBase();

	void loop();
	void exit();

	void runNow(Task &&Task_F);
	TimerId runAfter(int64_t MilliSec_F, Task &&Task_F);
	TimerId runEvery(int64_t MilliSec_F, Task &&Task_F);
	void rescheduleAfter(TimerId *TimerId_F, int64_t MilliSec_F, Task &&Task_F);
	void rescheduleAfter(const TimerId &TimerId_F, int64_t MilliSec_F);
	void cancel(const TimerId &TimerId_F);
    Poller* getPoller();

private:
	void __handleTimeout();
	void __wakeUp();
	void __refresh();

	std::atomic_bool     m_bRunning__;
	Poller  *m_pinstPoller__;

	uint64_t m_qwPid__;//quadra words
	int      m_iNextTimeOut__;

	uint32_t m_uiSeq__;

	BoundedQueue<Task> *m_pinstTasks__;
	ThreadPool<Task>   *m_pinstPool__;

	std::mutex                  m_mtxAction__;
	std::map<uint32_t, TimeRep> m_mapRepeatTasks__;
	std::map<TimerId, Task>     m_mapPendingTasks__;
};

} // namespace net

#endif /* _EVENT_BASE_HPP_ */
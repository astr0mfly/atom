#ifndef _BOUNDED_QUEUE_HPP_
#define _BOUNDED_QUEUE_HPP_

#include <atomic>
#include <mutex>
#include <condition_variable>
#include "..\..\rsc\common\utility.h"

namespace dsm {

template<typename T>
class BoundedQueue : private NoCopyTemplate<T>
{
public:
    BoundedQueue(int Capacity_F);
    ~BoundedQueue();

    inline void push(T &);//不阻塞的话最多容纳 n-1个。运转起来可容纳n个。
    inline void push(T &&);
    inline T pop();
    inline void clear();

private:
    std::atomic_bool        m_bClear__;
    int                     m_iBeg__;
    int                     m_iEnd__;
    int                     m_Capacity__;
    T                      *m_instQueue__;
    std::mutex              m_mtxLock__;
    std::condition_variable m_cvEmpty__;
    std::condition_variable m_cvFull__;
};

template<typename T>
BoundedQueue<T>::BoundedQueue(int Capacity_F) :
    m_bClear__(false),
    m_iBeg__(0),
    m_iEnd__(0),
    m_Capacity__(Capacity_F)
{
    if (m_Capacity__ <= 0) {
        m_Capacity__ = 8;
    }

    if (m_Capacity__ > 1024) {
        m_Capacity__ = 1024;
    }

	m_instQueue__ = new T[m_Capacity__];
}

template<typename T>
BoundedQueue<T>::~BoundedQueue()
{
	delete [] m_instQueue__;
}

template<typename T>
inline void BoundedQueue<T>::clear()
{
	if (m_bClear__.load()) {
		return ;
	}
	m_bClear__ = true;
	m_cvEmpty__.notify_all();
}

template<typename T>
inline void BoundedQueue<T>::push(T &Value_F)
{
    using namespace std;
    unique_lock<std::mutex> uLocker(m_mtxLock__);
    while (((m_iEnd__+1)%m_Capacity__) == m_iBeg__) {
        m_cvFull__.wait(uLocker);
    }
	m_instQueue__[m_iEnd__] = Value_F;
    if (++m_iEnd__ == m_Capacity__) {
        m_iEnd__ = 0;
    }
	m_cvEmpty__.notify_one();
}

template<typename T>
inline void BoundedQueue<T>::push(T &&Value_F)
{
    using namespace std;
    unique_lock<std::mutex> uLocker(m_mtxLock__);
    while (((m_iEnd__ + 1) % m_Capacity__) == m_iBeg__) {
        m_cvFull__.wait(uLocker);//不和容器空的判断在同条件。
    }
    m_instQueue__[m_iEnd__] = Value_F;
    if (++m_iEnd__ == m_Capacity__) {
        m_iEnd__ = 0;
    }
    m_cvEmpty__.notify_one();
}

template<typename T>
inline T BoundedQueue<T>::pop()
{
    using namespace std; 
    unique_lock<std::mutex> uLocker(m_mtxLock__);
    while (m_iBeg__ == m_iEnd__ && !m_bClear__.load()) {
        m_cvEmpty__.wait(uLocker);
    }

	if (m_bClear__.load()) {
		return T();
	}
	T Obj(m_instQueue__[m_iBeg__]);
    if (++m_iBeg__ == m_Capacity__) {
        m_iBeg__ = 0;
    }
	m_cvFull__.notify_one();
 	return move(Obj);
}

} // namespace dsm

#endif /* _BOUNDED_QUEUE_HPP_ */
#ifndef _THREAD_POOL_HPP_
#define _THREAD_POOL_HPP_

#include <atomic>
#include "..\..\rsc\common\utility.h"
#include "bounded_queue.hpp"
#include "thread.hpp"

namespace dsm {

template<typename T>
class ThreadPool : private NoCopyTemplate<T>
{
public:
    ThreadPool(BoundedQueue<T>* Queue_F, int ThreadNum_F);
    ~ThreadPool();

    void run();
    void clear();

private:
    std::atomic_int8_t      m_atmRunning__;
    int                     m_iThreadNum__;
    BoundedQueue<T>        *m_pqueTask__;
    Mission                **m_ppThreads__;
};

template<typename T>
ThreadPool<T>::ThreadPool(BoundedQueue<T> *Queue_F, int ThreadNum_F) :
    m_atmRunning__(false),
    m_iThreadNum__(ThreadNum_F),
    m_pqueTask__(Queue_F)
{
    if (m_iThreadNum__ <= 0) {
        m_iThreadNum__ = 1;
    }
    else if (m_iThreadNum__ > 8) {
        m_iThreadNum__ = 8;
    }

	m_ppThreads__ = new Mission*[m_iThreadNum__];

	for (int i = 0; i < m_iThreadNum__; ++i) {
		m_ppThreads__[i] = new Mission([this] { this->run(); });
		m_ppThreads__[i]->start();
	}
	m_atmRunning__.store(true);
}

template<typename T>
void ThreadPool<T>::run()
{
	for (;;) {
		T task = m_pqueTask__->pop();

		if (!m_atmRunning__.load())
			break;

		task();
	}
}

template<typename T>
void ThreadPool<T>::clear()
{
    if (!m_atmRunning__.load()) {
        return ;
    }

	m_atmRunning__.store(false);

	m_pqueTask__->clear();

    for (int i = 0; i < m_iThreadNum__; ++i) {
        m_ppThreads__[i]->stop();
    }
}

template<typename T>
ThreadPool<T>::~ThreadPool()
{
    clear();
    for (int i = 0; i < m_iThreadNum__; ++i) {
        delete m_ppThreads__[i];
    }

	delete [] m_ppThreads__;
}

} // namespace dsm

#endif /* _THREAD_POOL_HPP_ */
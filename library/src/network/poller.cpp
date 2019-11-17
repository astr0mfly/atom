#ifdef __unix__
#include <unistd.h>
#elif _WIN32
#include "..\include\unistd.h"
#endif

#include <cstring>
#include <cassert>
#include <algorithm>
#include <utility>
#include <chrono>
#include <thread>

#include "..\..\rsc\common\utility.h"
#include "poller.hpp"
#include "channel.hpp"
#include "socket.hpp"

namespace net {

Poller::Poller() :
    m_uiEventCount__(0)
{
    m_vecChls__.resize(WSA_MAXIMUM_WAIT_EVENTS);
    memset(m_ahEvents__, (int)INVALID_HANDLE_VALUE, sizeof(m_ahEvents__));
    memset(&m_stNetWorkEvent__, 0, sizeof(m_stNetWorkEvent__));
}

Poller::~Poller()
{ }

void Poller::addChannel(Channel *Channel_F)
{
    Channel_F->getIndex() = m_uiEventCount__;
    m_mtxArrLock__.lock();
    m_vecChls__[m_uiEventCount__] = Channel_F;
    m_sockArr__[m_uiEventCount__] = Channel_F->fd();
    m_ahEvents__[m_uiEventCount__] = Channel_F->events();
    m_mtxArrLock__.unlock();
    if (++m_uiEventCount__ >= WSA_MAXIMUM_WAIT_EVENTS) {
        printf("too many connections\n");
        return ;
    }
}

void Poller::updateChannel(Channel *Channel_F)
{ }

void Poller::removeChannel(Channel *Channel_F)
{
    using namespace std;

    uint32_t uiOperIndex = Channel_F->getIndex();
    Channel_F->getIndex() = 0;
    m_mtxArrLock__.lock();

    memmove_s(&m_ahEvents__[uiOperIndex], sizeof(m_ahEvents__) - uiOperIndex*sizeof(WSAEVENT),
        &m_ahEvents__[uiOperIndex+1], sizeof(m_ahEvents__) - (uiOperIndex + 1)*sizeof(WSAEVENT));
    memmove_s(&m_sockArr__[uiOperIndex], sizeof(m_sockArr__) - uiOperIndex*sizeof(WSAEVENT),
        &m_sockArr__[uiOperIndex + 1], sizeof(m_sockArr__) - (uiOperIndex + 1)*sizeof(WSAEVENT));

    Channels_T::iterator it = m_vecChls__.begin() + uiOperIndex;
    for(; it != m_vecChls__.end(); ++it) {
        if (*it) {
            ((*it)->getIndex())--;
        }
    }
    m_vecChls__.erase(m_vecChls__.begin() + uiOperIndex);

    m_mtxArrLock__.unlock();

    m_uiEventCount__--;
}

void Poller::loopOnce(int Ms_F)
{
    SOCKET sockClient = INVALID_SOCKET;
    SOCKET sockServer = INVALID_SOCKET;
    //数组内任意一个WSAEVENT有信号了，返回对应的索引值
    if (m_uiEventCount__ == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(Ms_F));
        return ;
    }

    std::lock_guard<std::mutex> lg(m_mtxArrLock__);
    DWORD dwIndex = WSAWaitForMultipleEvents(m_uiEventCount__, m_ahEvents__
                                             , false, Ms_F, false);
    if ((dwIndex == WSA_WAIT_TIMEOUT) || (dwIndex == WSA_WAIT_FAILED)) {
        return ;
    }
    if (m_vecChls__.empty()) {
        return ;
    }
    sockServer = m_vecChls__[dwIndex - WSA_WAIT_EVENT_0]->fd();
    WSAEnumNetworkEvents(sockServer, m_ahEvents__[dwIndex - WSA_WAIT_EVENT_0],
                         &m_stNetWorkEvent__);

    if (m_stNetWorkEvent__.lNetworkEvents & FD_ACCEPT) {
        printf("doacpt(%d)\r\n", dwIndex);
        if (m_stNetWorkEvent__.iErrorCode[FD_ACCEPT_BIT] != 0) {
            printf("FD_ACCEPT failed with error %d\n",
                   m_stNetWorkEvent__.iErrorCode[FD_ACCEPT_BIT]);
            return ;
        }
        m_vecChls__[dwIndex - WSA_WAIT_EVENT_0]->handleAccept();
    }
    //客户端接收
    if (m_stNetWorkEvent__.lNetworkEvents & FD_READ) {
        printf("doread(%d)\r\n", dwIndex);
        if (m_stNetWorkEvent__.iErrorCode[FD_READ_BIT] != 0) {
            printf("FD_READ failed with error %d\n",
                   m_stNetWorkEvent__.iErrorCode[FD_READ_BIT]);
            return ;
        }
        m_vecChls__[dwIndex - WSA_WAIT_EVENT_0]->handleRead();
    }
    //客户端发送
    if (m_stNetWorkEvent__.lNetworkEvents & FD_WRITE) {
        printf("dosend(%d)\r\n", dwIndex);
        if (m_stNetWorkEvent__.iErrorCode[FD_WRITE_BIT] != 0) {
            printf("FD_WRITE failed with error %d\n",
                   m_stNetWorkEvent__.iErrorCode[FD_WRITE_BIT]);
            return ;
        }
        m_vecChls__[dwIndex - WSA_WAIT_EVENT_0]->handleWrite();
    }
    //断开连接
    if (m_stNetWorkEvent__.lNetworkEvents & FD_CLOSE) {
        printf("doclse(%d)\r\n", dwIndex);
        if (m_stNetWorkEvent__.iErrorCode[FD_CLOSE_BIT] != 0) {
            printf("FD_CLOSE failed with error %d\n",
                   m_stNetWorkEvent__.iErrorCode[FD_CLOSE_BIT]);
            return ;
        }
        m_vecChls__[dwIndex - WSA_WAIT_EVENT_0]->handleClose();
    }
}

} // namespace net

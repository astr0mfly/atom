#ifndef _POLLER_HPP_
#define _POLLER_HPP_

#include <vector>
#include <mutex>
#include <winsock2.h>
#include "..\..\rsc\common\utility.h"

namespace net {

class Channel;

//windows:EventSelect linux:Poll
class Poller : private NoCopy
{
public:
    typedef enum _SELECT_FD_TYPE_E {
        E_SELECT_FD_READ,
        E_SELECT_FD_WRITE,
        E_SELECT_FD_EXCP,

        E_SELECT_FD_BTM
    }E_SELECT_FD_TYPE;
    typedef std::vector<Channel *> Channels_T;
	Poller();
	~Poller();

	void addChannel(Channel *Channel_F);
	void updateChannel(Channel *Channel_F);
	void removeChannel(Channel *Channel_F);

	void loopOnce(int Ms_F);

private:
    UINT                    m_uiEventCount__;
    Channels_T              m_vecChls__;
                    
    WSAEVENT                m_ahEvents__[WSA_MAXIMUM_WAIT_EVENTS];
    SOCKET                  m_sockArr__[WSA_MAXIMUM_WAIT_EVENTS];
    WSANETWORKEVENTS        m_stNetWorkEvent__;
    std::mutex              m_mtxArrLock__;
};

} // namespace net

#endif /* _POLLER_HPP_ */
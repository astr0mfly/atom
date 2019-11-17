#ifndef _CHANNEL_HPP_
#define _CHANNEL_HPP_

#include <utility>
#include "..\..\rsc\common\utility.h"
#include "event.h"

namespace net {

class Poller;
class Connection;

typedef std::function<SOCKET()> AcceptCallBack;
typedef std::function<void()> ReadCallBack;
typedef std::function<void()> WriteCallBack;

class Channel : private NoCopy
{
public:
    Channel(int Sock_F, Poller *Poller_F, const AcceptCallBack &AcceptCb_F);
	Channel(int Sock_F, Poller *Poller_F, const ReadCallBack &ReadCb_F,
            const WriteCallBack &WriteCb_F);
	virtual ~Channel();

	int fd() const;
    event_handle events() const;

    void onAccept(const AcceptCallBack &AcceptCb_F);
	void onRead(const ReadCallBack &ReadCb_F);
	void onWrite(const WriteCallBack &WriteCb_F);
    SOCKET handleAccept();
	void handleRead();
	void handleWrite();
    void handleClose();
    USHORT &getIndex(){ return m_nIndex__; }
    friend class Connection;

private:
    USHORT          m_nIndex__;/* First:acceptIndex Second:operateIndex */
	int             m_iFd__;
    event_handle    m_hEvents__;
	Poller         *m_pinstPoller__;

    AcceptCallBack  m_funcAccept__;
	ReadCallBack    m_funcRead__;
	WriteCallBack   m_funcWrite__;
};

} // namespace net

#endif /* _CHANNEL_HPP_ */
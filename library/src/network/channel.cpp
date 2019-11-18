#include "socket.hpp"
#include "channel.hpp"
#include "poller.hpp"


namespace net {

Channel::Channel(int Sock_F, Poller* Poller_F, const AcceptCallBack& AcceptCb_F) :
    m_nIndex__(0),
    m_iFd__(Sock_F),
    m_pinstPoller__(Poller_F),
    m_funcAccept__(AcceptCb_F),
    m_funcRead__(nullptr),
    m_funcWrite__(nullptr)
{
    m_hEvents__ = create_EVENT(false, false);
    WSAEventSelect(m_iFd__, m_hEvents__, FD_ACCEPT | FD_CLOSE);
    m_pinstPoller__->addChannel(this);
}

Channel::Channel(int Sock_F, Poller *Poller_F, const ReadCallBack &ReadCb_F,
	             const WriteCallBack &WriteCb_F) :
    m_nIndex__(0),
    m_iFd__(Sock_F),
    m_pinstPoller__(Poller_F),
    m_funcAccept__(nullptr),
    m_funcRead__(ReadCb_F),
    m_funcWrite__(WriteCb_F),
    m_hEvents__(INVALID_HANDLE_VALUE)
{
    m_hEvents__ = create_EVENT(false, false);
    WSAEventSelect(m_iFd__, m_hEvents__, FD_WRITE | FD_READ | FD_CLOSE);
	m_pinstPoller__->addChannel(this);
}

Channel::~Channel()
{
    m_funcAccept__ = nullptr;
    m_funcRead__ = nullptr;
    m_funcWrite__ = nullptr;
	m_pinstPoller__->removeChannel(this);
    if (m_hEvents__ != INVALID_HANDLE_VALUE) {
        destroy_EVENT(m_hEvents__);
    }
}

int Channel::fd() const
{
	return m_iFd__;
}

event_handle Channel::events() const
{
	return m_hEvents__;
}

void Channel::onAccept(const AcceptCallBack &AcceptCb_F)
{
    m_funcAccept__ = AcceptCb_F;
}

void Channel::onRead(const ReadCallBack &ReadCb_F)
{
	m_funcRead__ = ReadCb_F;
}

void Channel::onWrite(const WriteCallBack &WriteCb_F)
{
	m_funcWrite__ = WriteCb_F;
}

SOCKET Channel::handleAccept()
{
    return m_funcAccept__ ? m_funcAccept__() : INVALID_SOCKET;
}

void Channel::handleRead()
{
    if (m_funcRead__) {
        m_funcRead__();
    }
}

void Channel::handleWrite()
{
    if (m_funcWrite__) {
        m_funcWrite__();
    }
}

void Channel::handleClose()
{
    Socket tmp(this->fd());
    tmp.close();
}

} // namespace net

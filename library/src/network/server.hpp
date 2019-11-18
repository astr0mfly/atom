#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <vector>

#include "..\..\rsc\common\utility.h"
#include "socket.hpp"
#include "connection.hpp"

namespace net {

class Channel;
class EventBase;

class Server : private NoCopy
{
public:
    typedef std::vector<Connection *> Connections_T;
	Server(EventBase *EventBase_F, uint16_t Port_F);
	virtual ~Server();

	void start();
	void close();
    void broadcastMsg(const char* data, uint32_t len);
    void acceptMember();
    void rejectMember();

	void onConnect(const ConnectCallBack &Connect_F);
    void onTuneIn(const ConnectCallBack& TuneIn_F);
    Connections_T &connections();
	uint16_t port() const;

    const EndPoint addr();

    void sendMsg(EndPoint &Remote_F, std::string &&Msg_F);

    Poller* getPoller();

protected:
	uint16_t                m_nPort_;
    uint32_t                m_uiHostIp;
	EventBase              *m_pinstEventBase_;
	Socket                  m_instSocket_;
	Channel                *m_pinstListen_;
    Connection             *m_pinstJoinLink_;

    Connections_T           m_vecConnections_;
	ConnectCallBack         m_funcConnectCb_;
    ConnectCallBack         m_funcTuneIn_Cb_;

private:
    int __handleAccept();
};

} // namespace net

#endif /* _SERVER_HPP_ */
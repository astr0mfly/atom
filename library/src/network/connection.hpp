#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

#include "..\..\rsc\common\utility.h"
#include "socket.hpp"
#include "endpoint.hpp"
#include "buffer.hpp"
#include "channel.hpp"

namespace net {

class Poller;
class Connection;

typedef std::function<void(Connection *)> ConnectCallBack;

class Connection : private NoCopy
{
public:
	Connection(const Socket &Sock_F, Poller *Poller_F);
	Connection(const EndPoint &Server_F, Poller *Poller_F);
    Connection(const Socket &Sock_F, Poller *Poller_F, bool &&UdpFlag_F);
	virtual ~Connection();

	bool success() const;

	Buffer &getInput();//输入 Buffer, 收到的消息
	Buffer &getOutput();//输出 Buffer, 要发送的消息

	void handleRead();
	void handleWrite();

	void onRead(const ReadCallBack &Read_F);
	void onWrite(const WriteCallBack &Write_F);

	bool close();

	void send(const char *Str_F);
	void send(Buffer &Buffer_F);
	void send(const char *Data_F, uint32_t DataLen_F);

	void sendOutput();
    void handleRecvfrom();

    Socket          m_sockUsed_;
protected:
	bool            m_bConnected_;
	Channel        *m_pinstChannel_;
	Buffer          m_instInput_;
	Buffer          m_instOutput_;

	ReadCallBack    m_funcRead_;
	WriteCallBack   m_funcWrite_;
};

} // namespace net

#endif /* _CONNECTION_HPP_ */
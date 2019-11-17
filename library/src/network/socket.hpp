#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#ifdef __unix__

#elif _WIN32

#include <ws2tcpip.h>//socklen_t
#include <windows.h>

#endif

#include "endpoint.hpp"

namespace net {

class Socket
{
public:
	Socket();
	Socket(int Fd_F);
	~Socket();

	int fd() const;
	bool valid() const;

	bool createTcp();
    bool createUdp();
	bool close();
	bool connect(const EndPoint &Remote_F);
    bool bind(const EndPoint& Local_F);
	bool bind(uint16_t Port_F);
	bool listen(int Backlog_F=5);
	int accept();

	uint32_t write(const char *Data_F, uint32_t DateLen_F, bool *Blocked_F=nullptr);
	uint32_t read(char *Data_F, uint32_t DateLen_F, bool *Blocked_F=nullptr);
    uint32_t sendTo(const EndPoint &Remote_F, const char* Data_F, uint32_t DateLen_F, bool* Blocked_F);
    uint32_t sendTo(char* Data_F, uint32_t DateLen_F, struct sockaddr* Server_F, int ServerSize_F, bool *Blocked_F=nullptr);
    uint32_t recvFrom(char* Data_F, uint32_t DateLen_F, struct sockaddr* Client_F=nullptr, int *ClientSize_F=nullptr, bool* Blocked_F=nullptr);

	bool setOption(int Value_F, bool Flag_F);
	bool getOption(int Value_F, int *Ret_F);
	bool getPeerName(EndPoint *Peer_F);
	bool getSockName(EndPoint *Mine_F);


	bool addFlag(int Flag_F);
	bool setNonBlock();
	bool setResuseAddress();
    bool setCancelNagle();

    static int getHostIp(uint32_t &Ip_F);

private:
	int      m_iFd__;
};

} // namespace net

#endif /* _SOCKET_HPP_ */
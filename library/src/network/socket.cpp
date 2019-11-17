#include "socket.hpp"

#ifdef __unix__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#elif _WIN32

#include <ws2tcpip.h>//socklen_t
#include <WinSock2.h>
#include <windows.h>
#include <intsafe.h>
using ssize_t = intptr_t;

#include <corecrt_io.h>
#endif

#include <fcntl.h>
#include <cstring>
#include <cassert>
#include <cerrno>

#include "..\..\dfx\comon\debug.h"

namespace net {
    
Socket::Socket() :
    m_iFd__(INVALID_SOCKET)
{ }

Socket::Socket(int Fd_F) :
    m_iFd__(Fd_F)
{ }

Socket::~Socket()
{ }

int Socket::fd() const
{
	return m_iFd__;
}

bool Socket::valid() const
{
	return m_iFd__ != INVALID_SOCKET;
}

bool Socket::createTcp()
{
	m_iFd__ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return m_iFd__ != SOCKET_ERROR;
}

bool Socket::createUdp()
{
    m_iFd__ = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    return m_iFd__ != SOCKET_ERROR;
}

bool Socket::close()
{
	int iRes = 0;

	if (m_iFd__ != INVALID_SOCKET) {
#ifdef __unix__
        iRes = !::close(m_iFd__);
        if (iRes == -1) {
            m_iFd__ = -1;
            return false;
        }
#elif _WIN32
        iRes = ::closesocket(m_iFd__);
        if (iRes == SOCKET_ERROR) {
            auto ErrorCode = WSAGetLastError();
            if (ErrorCode == WSAENOTSOCK) {
                m_iFd__ = INVALID_SOCKET;
            }
            else {
                wprintf(L"closesocket function failed with error: %ld\n", ErrorCode);
            }
            return false;
        }
#endif
		m_iFd__ = INVALID_SOCKET;
	}

	return true;
}

bool Socket::connect(const EndPoint &Remote_F)
{
	sockaddr_in stServer;
	memset(&stServer, 0, sizeof(stServer));
    stServer.sin_family      = AF_INET;
    stServer.sin_port        = htons(Remote_F.port());
    stServer.sin_addr.s_addr = Remote_F.address();

    if (::connect(m_iFd__, (const struct sockaddr *) &stServer, sizeof(stServer))
        == -1) {
        wprintf(L"connect function failed with error: %d\n", WSAGetLastError());
        this->close();
        return false;
    }
	return true;
}

bool Socket::bind(const EndPoint& Local_F)
{
    sockaddr_in stServer;
    memset(&stServer, 0, sizeof(stServer));
    stServer.sin_family = AF_INET;
    stServer.sin_port = htons(Local_F.port());
    stServer.sin_addr.s_addr = Local_F.address();

    if (::bind(m_iFd__, (const struct sockaddr *) &stServer, sizeof(stServer))
        == -1) {
        wprintf(L"bind function failed with error: %d\n", WSAGetLastError());
        this->close();
        return false;
    }
    return true;
}

bool Socket::bind(uint16_t Port_F)
{
	sockaddr_in stServer;
	memset(&stServer, 0, sizeof(stServer));
    stServer.sin_family      = AF_INET;
    stServer.sin_port        = htons(Port_F);
    stServer.sin_addr.s_addr = htonl(INADDR_ANY);

    if (::bind(m_iFd__, (const struct sockaddr *) &stServer, sizeof(stServer))
        == -1) {
        wprintf(L"bind function failed with error: %d\n", WSAGetLastError());
        this->close();
        return false;
    }
    return true;
}

bool Socket::listen(int Backlog_F)
{
    if (::listen(m_iFd__, Backlog_F) == -1) {
        wprintf(L"listen function failed with error: %d\n", WSAGetLastError());
        this->close();
        return false;
    }
	return true;
}

int Socket::accept()
{
	sockaddr_in stClient;
	memset(&stClient, 0, sizeof(stClient));
	socklen_t iLen = sizeof(stClient);
	int iFd = ::accept(m_iFd__, (struct sockaddr *)&stClient, &iLen);
    if (iFd == INVALID_SOCKET) {
        logError("accept Fail LastWords:%d", WSAGetLastError());
    }
	return iFd;
}

bool Socket::setOption(int Value_F, bool Flag_F)
{
	return !::setsockopt(m_iFd__, SOL_SOCKET, Value_F, (char*)&Flag_F, sizeof(Flag_F));
}

bool Socket::getOption(int Value_F, int *Ret_F)
{
	socklen_t iLen = sizeof(*Ret_F);
	return !::getsockopt(m_iFd__, SOL_SOCKET, Value_F, (char *)Ret_F, &iLen);
}

bool Socket::setResuseAddress()
{
	int iFlag = 1;
	return !::setsockopt(m_iFd__, SOL_SOCKET, SO_REUSEADDR, (char*)&iFlag, sizeof(iFlag));
}
bool Socket::setCancelNagle()
{
    int iFlag = 1;
    return !::setsockopt(m_iFd__, IPPROTO_TCP, TCP_NODELAY, (char *)& iFlag, sizeof(iFlag));
}

bool Socket::getPeerName(EndPoint *Peer_F)
{
	sockaddr_in staddr;
	memset(&staddr, 0, sizeof(staddr));
	socklen_t iLen = sizeof(staddr);
	if (!::getpeername(m_iFd__, (struct sockaddr *)& staddr, &iLen)) {
        if (Peer_F) {
            *Peer_F = EndPoint(ntohs(staddr.sin_port), staddr.sin_addr.s_addr);
        }
		return true;
	}
	return false;
}

bool Socket::getSockName(EndPoint *Mine_F)
{
	sockaddr_in staddr;
	memset(&staddr, 0, sizeof(staddr));
	socklen_t iLen = sizeof(staddr);
	if (!getsockname(m_iFd__, (struct sockaddr *)& staddr, &iLen)) {
        if (Mine_F) {
            *Mine_F = EndPoint(ntohs(staddr.sin_port), staddr.sin_addr.s_addr);
        }
		return true;
	}
	return false;
}

int Socket::getHostIp(uint32_t &Ip_F)
{
    char strHostName[100];
    memset(strHostName, 0, sizeof(strHostName));
    ::gethostname(strHostName, 100);

    addrinfo hints, *res;
    int err=0;

    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((err = getaddrinfo(strHostName, NULL, &hints, &res)) != 0) {
        printf("error %d : %s\r\n", err, gai_strerror(err));
        return err;
    }
    Ip_F = ((sockaddr_in*)(res->ai_addr))->sin_addr.s_addr;
    freeaddrinfo(res);

    return 0;
}

bool Socket::addFlag(int Flag_F)
{
#ifdef __unix__
	int value = fcntl(m_iFd__, F_GETFL, 0);
	assert(value != -1);
	return !fcntl(m_iFd__, F_SETFL, value | Flag_F);
#elif _WIN32
    int iResult;
    u_long iMode = (u_long)Flag_F;
    iResult = ioctlsocket(m_iFd__, FIONBIO, (u_long*)& iMode);
    if (iResult != NO_ERROR) {
        printf("ioctlsocket failed with error: %ld\n", iResult);
        return false;
    }

    return true;
#endif
}

bool Socket::setNonBlock()
{
#ifdef __unix__
    int iResult = fcntl(m_iFd__, F_GETFL, 0);
    assert(iResult != -1);
    return !fcntl(m_iFd__, F_SETFL, iResult | O_NONBLOCK);
#elif _WIN32
    int iResult;
    u_long iMode = 0x1;
    iResult = ioctlsocket(m_iFd__, FIONBIO, (u_long*)& iMode);
    if (iResult != NO_ERROR) {
        printf("ioctlsocket failed with error: %ld\n", iResult);
        return false;
    }

    return true;
#endif
}

uint32_t Socket::write(const char *Data_F, uint32_t DataLen_F, bool *Block_F)
{
	uint32_t uiWritten = 0;
#ifdef __unix__

	for (; uiWritten < DataLen_F;) {
		ssize_t iWriting = ::write(m_iFd__, Data_F + uiWritten, DataLen_F - uiWritten);
		if (iWriting > 0) {
			uiWritten += iWriting;
			continue ;
		} else if (iWriting == -1) {
            if (errno == EINTR) {
                continue ;
            }
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
                if (Block_F) {
                    *Block_F = true;
                }
				break ;
			}
		}
        else if (iWriting == 0) {
            logInfo("Connection closed");
            break;
        }

        logError("write()");
		break;
	}

#elif _WIN32

    for (; uiWritten < DataLen_F;) {
        ssize_t iWriting = ::send(m_iFd__, Data_F + uiWritten, DataLen_F - uiWritten, 0);
        if (iWriting > 0) {
            uiWritten += iWriting;
            continue ;
        }
        else if (iWriting == SOCKET_ERROR) {
            int res = WSAGetLastError();
            if (res == WSAEINTR) {
                continue ;
            }
            else if (res == WSAEWOULDBLOCK) {
                if (Block_F) {
                    *Block_F = true;
                }
                logInfo("Recv()--WSAEWOULDBLOCK");
                break ;
            }
        }
        else if (iWriting == 0) {
            logInfo("Connection closed");
            break ;
        }
        logError("send() error:%d", WSAGetLastError());
        break;
    }

#endif
	return uiWritten;
}

uint32_t Socket::read(char *Data_F, uint32_t DataLen_F, bool *Block_F)
{
	uint32_t uiHasRead = 0;
	ssize_t iReading;
#ifdef __unix__
	for (; uiHasRead < DataLen_F && (iReading = ::read(m_iFd__, Data_F + uiHasRead, DataLen_F - uiHasRead));) {
		if (iReading == -1) {
            if (errno == EINTR) {
                continue;
            }

            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                if (Block_F) {
                    *Block_F = true;
                }
            }
            else {
                logError("read()");
            }

			break;
		}
		uiHasRead += iReading;
	}
#elif _WIN32
    for (; uiHasRead < DataLen_F && (iReading = ::recv(m_iFd__, Data_F + uiHasRead, DataLen_F - uiHasRead, 0));) {
        if (iReading > 0) {
            uiHasRead += iReading;
            continue;
        }
        else if (iReading == SOCKET_ERROR) {
            int res = WSAGetLastError();
            if (res == WSAEINTR) {
                continue;
            }
            else if (res == WSAEWOULDBLOCK) {
                if (Block_F) {
                    *Block_F = true;
                }
                //logInfo("Recv()--WSAEWOULDBLOCK");
                break;
            }
        }
        else if (iReading == 0) {
            logInfo("Connection closed");
            break;
        }
        logError("Recv() error:%d", WSAGetLastError());
        break;
    }

#endif
	return uiHasRead;
}

uint32_t Socket::sendTo(const EndPoint& Remote_F, const char* Data_F, uint32_t DataLen_F, bool* Block_F)
{
    sockaddr_in stServer;
    memset(&stServer, 0, sizeof(stServer));
    stServer.sin_family = AF_INET;
    stServer.sin_port = htons(Remote_F.port());
    stServer.sin_addr.s_addr = Remote_F.address();

    int iSent = sendto(m_iFd__, Data_F, DataLen_F, 0, (struct sockaddr *)&stServer, sizeof(struct sockaddr));
    if (iSent == SOCKET_ERROR) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            *Block_F = true;
        }
        else {
            logError("sendTo()");
        }
    }

    return iSent;
}
uint32_t Socket::sendTo(char* Data_F, uint32_t DataLen_F, struct sockaddr* Server_F, int ServerSize_F, bool* Block_F)
{
    int iSent = sendto(m_iFd__, Data_F, DataLen_F, 0, Server_F, ServerSize_F);
    if (iSent == SOCKET_ERROR) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            if (Block_F) {
                *Block_F = true;
            }
        }
        else {
            logError("sendTo()");
        }
    }

    return iSent;

}

uint32_t Socket::recvFrom(char* Data_F, uint32_t DataLen_F, struct sockaddr *Client_F, int *ClientSize_F, bool* Block_F)
{
    if (!(Client_F && ClientSize_F)) {
        sockaddr_in stclient;
        int clientAddrSize = sizeof(stclient);
        Client_F = (struct sockaddr *)&stclient;
        ClientSize_F = &clientAddrSize;
    }

    int received = recvfrom(m_iFd__, Data_F, DataLen_F, 0, Client_F, ClientSize_F);
    if (received == SOCKET_ERROR) {
#ifdef __unix__
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            if (Block_F) {
                *Block_F = true;
            }
        }
        else {
            logError("recvfrom()");
        }
#elif _WIN32
        auto ErrorCode = WSAGetLastError();
        printf("recvfrom failed with error %d ", ErrorCode);
        if (ErrorCode == WSAEMSGSIZE) {
            printf("Message too long. received:%d DataLen_F:%d \r\n", received, DataLen_F);
        }
#endif
    }

    return received;
}

} // namespace net

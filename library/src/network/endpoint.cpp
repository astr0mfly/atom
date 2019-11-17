#ifdef __unix__
#include <arpa/inet.h>
#elif _WIN32
#include <ws2tcpip.h>
#include <windows.h> 
#endif

#include <cassert>

#include "endpoint.hpp"

namespace net {

EndPoint::EndPoint():
    m_nPort__(0),
    m_uiAddress__(0)
{ }

EndPoint::EndPoint(uint16_t port, uint32_t address):
    m_nPort__(port),
    m_uiAddress__(address)
{ }

EndPoint::EndPoint(uint16_t port, const char *str):
    m_nPort__(port)
{
	assert(inet_pton(AF_INET, str, &m_uiAddress__) == 1);
}

EndPoint::~EndPoint()
{ }


std::string EndPoint::toString() const
{
	char buf[MaxLen];
	assert(inet_ntop(AF_INET, (void *)&m_uiAddress__, buf, MaxLen));
    //@Ip:Port,
	return '@' + std::string(buf) + ':' + std::to_string(m_nPort__) + ',';
}

EndPoint EndPoint::str2Addr(std::string AddrIn_F)
{
    uint32_t uiIp = 0;
    inet_pton(AF_INET, AddrIn_F.substr(AddrIn_F.find_first_of('@')+1, AddrIn_F.find_first_of(':')).c_str(), &uiIp);
    uint16_t nPort = atol(AddrIn_F.substr(AddrIn_F.find_first_of(':')+1, AddrIn_F.find_first_of(',')).c_str());
    
    return EndPoint(nPort, uiIp);
}

} // namespace net

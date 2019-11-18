#include <cstring>
#include <cerrno>
#include <cassert>

#include "connection.hpp"
#include "channel.hpp"

namespace net {

Connection::Connection(const EndPoint &Server_F, Poller *Poller_F) :
    m_funcRead_(nullptr),
    m_funcWrite_(nullptr),
    m_bConnected_(false),
    m_pinstChannel_(nullptr)
{
    using namespace std;
    //client connect
    char byMaxConTimes = 5;
    do {
        assert(m_sockUsed_.createTcp());
        if (m_sockUsed_.connect(Server_F)) {
            assert(m_sockUsed_.setNonBlock());
            break ;
        }
        printf("socket connect server %s failed, remaining connect times:%d, %d :(\n",
                Server_F.toString().c_str(), byMaxConTimes, WSAGetLastError());
        m_sockUsed_.close();
    } while(byMaxConTimes--);

    if (byMaxConTimes > 0) {
        m_pinstChannel_ = new Channel(m_sockUsed_.fd(),
            Poller_F,
            bind(&Connection::handleRead, this),
            bind(&Connection::handleWrite, this));
        m_bConnected_ = true;
    }
}

Connection::Connection(const Socket &Sock_F, Poller *Poller_F):
    m_sockUsed_(Sock_F),
    m_funcRead_(nullptr),
    m_funcWrite_(nullptr)
{   //server connect
	assert(m_sockUsed_.setNonBlock());
	m_pinstChannel_ = new Channel(m_sockUsed_.fd(),
                           Poller_F,
		                   [this]() { this->handleRead(); },
                           [this]() { this->handleWrite(); });
	m_bConnected_ = true;
}

Connection::Connection(const Socket &Sock_F, Poller *Poller_F, bool &&Udpflag_F):
    m_sockUsed_(Sock_F),
    m_bConnected_(Udpflag_F),
    m_funcRead_(nullptr),
    m_funcWrite_(nullptr)
{
    m_pinstChannel_ = new Channel(m_sockUsed_.fd(),
        Poller_F,
        [this]() { this->handleRecvfrom(); },
        nullptr);
}

Connection::~Connection()
{
	close();
}

bool Connection::close()
{
	if (m_sockUsed_.valid()) {
		m_bConnected_ = false;
        if (m_pinstChannel_) {
            delete m_pinstChannel_;
            m_pinstChannel_ = nullptr;
        }
		return m_sockUsed_.close();
	}
	return true;
}

bool Connection::success() const
{
	return m_bConnected_;
}

Buffer& Connection::getInput()
{
	return m_instInput_;
}

Buffer& Connection::getOutput()
{
	return m_instOutput_;
}

void Connection::onRead(const ReadCallBack &readcb)
{
	m_funcRead_ = readcb;
}

void Connection::onWrite(const WriteCallBack &writecb)
{
	m_funcWrite_ = writecb;
}

void Connection::handleRead()
{
	if (!m_bConnected_) {
		printf("connection has closed :(\n");
		return ;
	}
	m_instInput_.reset();
	bool bBlocked = false;
	uint32_t uiHasRead = m_sockUsed_.read(m_instInput_.end(),
                                          m_instInput_.space(), &bBlocked);
	if (!uiHasRead && !bBlocked) {
		this->close();
		return ;
	}

	m_instInput_.advanceTail(uiHasRead);
    if (m_funcRead_ && uiHasRead) {
        m_funcRead_();
    }
}

void Connection::handleWrite()
{
	if (!m_bConnected_) {
		printf("connection has closed :(\n");
		return ;
	}
    if (!m_instOutput_.size()) {
        return ;
    }
	bool bBlocked = false;
	uint32_t uihasWriten = m_sockUsed_.write(m_instOutput_.begin(),
                                             m_instOutput_.size(), &bBlocked);
	if (!uihasWriten && !bBlocked) {
		close();
		return ;
	}
    if (m_instOutput_.size()) {
	    m_instOutput_.advanceHead(uihasWriten);
    }

	if (m_instOutput_.empty()) {
        if (m_funcWrite_) {
            m_funcWrite_();
        }
	}
}

void Connection::handleRecvfrom()
{
    if (!m_bConnected_) {
        printf("there is not an UDP connect :(\n");
        return ;
    }
    m_instInput_.clear();
    bool bBlocked = false;
    uint32_t uiHasRead = m_sockUsed_.recvFrom(m_instInput_.begin(), m_instInput_.space(), nullptr, nullptr, &bBlocked);
    if (!uiHasRead && !bBlocked) {
        close();
        return ;
    }

    if (m_funcRead_ && uiHasRead) {
        m_funcRead_();
    }
}

void Connection::send(const char *str)
{
	send(str, strlen(str));
}

void Connection::send(Buffer &buffer)
{
	m_instOutput_.read(buffer.begin(), buffer.size());
	sendOutput();
    buffer.clear();
}

void Connection::send(const char *str, uint32_t len)
{
	m_instOutput_.read(str, len);
	sendOutput();
}

void Connection::sendOutput()
{
	if (!m_bConnected_) {
        printf("connection has closed :(\n");
		m_instOutput_.clear();
		return ;
	}
	bool bBlocked = false;
	uint32_t uiHasWriten = m_sockUsed_.write(m_instOutput_.begin(),
                                             m_instOutput_.size(), &bBlocked);
	if (!uiHasWriten && !bBlocked) {
		close();
		return ;
	}
    if (m_instOutput_.size() >= uiHasWriten) {
        m_instOutput_.advanceHead(uiHasWriten);
    }
    if (m_instOutput_.size()) {
        m_instOutput_.adjust();
    }
}

} // namespace net

#include <cassert>
#include "server.hpp"
#include "eventbase.hpp"
#include "channel.hpp"
#include "connection.hpp"

namespace net {

#define DSM_BC_PORT 12580

Server::Server(EventBase *EventBase_F, uint16_t Port_F) :
    m_nPort_(Port_F),
    m_uiHostIp(0),
    m_pinstEventBase_(EventBase_F),
    m_pinstListen_(nullptr),
    m_funcConnectCb_(nullptr),
    m_pinstJoinLink_(nullptr)
{
    Socket::getHostIp(m_uiHostIp);
}

Server::~Server()
{
    if (m_pinstListen_) {
        close();
    }

    for (const auto &it : m_vecConnections_) {
        delete it;
    }
}

void Server::close()
{
    if (m_pinstListen_) {
        delete m_pinstListen_;
        m_pinstListen_ = nullptr;
    }

	m_instSocket_.close();

    for (const auto &it : m_vecConnections_) {
        it->close();
    }

    rejectMember();
}

void Server::start()
{
	assert(m_instSocket_.createTcp()
           && m_instSocket_.setResuseAddress());
    uint8_t byFailTimes = 10;
    for (;m_instSocket_.bind(m_nPort_) != true; m_nPort_++, byFailTimes--) {
        assert(m_nPort_ > 1023);
        assert(byFailTimes > 0);
    }
    m_instSocket_.listen();
	m_pinstListen_ = new Channel(m_instSocket_.fd(), 
                                 m_pinstEventBase_->getPoller(),
                                 [this]() {
                                     return this->__handleAccept();
                                 });
}

void Server::broadcastMsg(const char* Data_F, uint32_t DataLen_F)
{
    Socket sockBC;
    bool blocked = 0;
    assert(sockBC.createUdp()
           && sockBC.setOption(SO_BROADCAST, 1)
           && sockBC.sendTo(std::move(EndPoint({ DSM_BC_PORT, htonl(INADDR_BROADCAST) })),
                            Data_F, DataLen_F, &blocked));

    sockBC.close();
}

void Server::acceptMember()
{
    Socket sockAc;
    sockAc.createUdp();

    sockAc.setOption(SO_BROADCAST, 1);
    sockAc.setOption(SO_REUSEADDR, 1);
    assert(sockAc.bind(DSM_BC_PORT) == true);

    m_pinstJoinLink_ = new Connection(sockAc, m_pinstEventBase_->getPoller(), true);
    m_pinstJoinLink_->onRead([this]() {
            if (m_funcTuneIn_Cb_) {
                m_funcTuneIn_Cb_(m_pinstJoinLink_);
            }
        });

}

void Server::rejectMember()
{
    if (m_pinstJoinLink_) {
        delete m_pinstJoinLink_;
        m_pinstJoinLink_ = nullptr;
    }
}

void Server::onConnect(const ConnectCallBack &Connect_F)
{
	m_funcConnectCb_ = Connect_F;
}

void Server::onTuneIn(const ConnectCallBack &TuneIn_F)
{
    m_funcTuneIn_Cb_ = TuneIn_F;
}

int Server::__handleAccept()
{
	int fd = m_instSocket_.accept();
	assert(fd > 0);
    Connection *con = new Connection(Socket(fd), m_pinstEventBase_->getPoller());
    if (m_funcConnectCb_) {
        m_funcConnectCb_(con);
    }

	m_vecConnections_.emplace_back(con);
    return fd;
}

std::vector<Connection *>& Server::connections()
{
	return m_vecConnections_;
}

uint16_t Server::port() const
{
	return m_nPort_;
}

const EndPoint Server::addr() {
    EndPoint instLocal;
    Socket sockTmp(m_instSocket_.fd());
    sockTmp.getSockName(&instLocal);
    if ((instLocal.address() == 0)
        ||((instLocal.address() == 2130706433))) {
        //如果IP是 “0.0.0.0”或者“127.0.0.1”则替换为真实地址
        instLocal(m_uiHostIp);
    }
    
    return std::move(instLocal);
}

void Server::sendMsg(EndPoint &Remote_F, std::string &&Msg_F) {
    Connection con(Remote_F, m_pinstEventBase_->getPoller());
    m_pinstEventBase_->runNow([&con, &Msg_F]() {
        con.send(Msg_F.c_str(), Msg_F.size());
        });
}

Poller* Server::getPoller() {
    return m_pinstEventBase_->getPoller();
}

} // name space net

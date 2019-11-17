#ifndef _ENDPOINT_HPP_
#define _ENDPOINT_HPP_

#include <string>

namespace net {

//ÖÕ¶Ë£¬ÃèÊöÒ»¸öÍøÂç¶Ë¿Ú£¬¼´Ip+Port
class EndPoint
{
public:
	static const uint32_t MaxLen = 16;
    EndPoint();
	EndPoint(uint16_t Port_F, uint32_t Address_F);
	EndPoint(uint16_t Port_F, const char *Address_F);
    EndPoint(const EndPoint &AddrIn_F) {
        m_nPort__ = AddrIn_F.m_nPort__;
        m_uiAddress__ = AddrIn_F.m_uiAddress__;
    }

    EndPoint& operator() (const EndPoint &AddrIn_F)
    {
        m_nPort__ = AddrIn_F.m_nPort__;
        m_uiAddress__ = AddrIn_F.m_uiAddress__;
        return (*this);
    }
    
    EndPoint &operator() (uint32_t Address_F)
    {
        m_uiAddress__ = Address_F;

        return (*this);
    }

    EndPoint& operator() (uint16_t Port_F, uint32_t Address_F)
    {
        m_nPort__ = Port_F;
        m_uiAddress__ = Address_F;

        return (*this);
    }
    EndPoint& operator= (const EndPoint& AddrIn_F)
    {
        m_nPort__ = AddrIn_F.m_nPort__;
        m_uiAddress__ = AddrIn_F.m_uiAddress__;

        return (*this);
    }

    bool operator!= (const EndPoint &Right_F)
    {
        return (m_nPort__ ^ Right_F.m_nPort__) || (m_uiAddress__ ^ Right_F.m_uiAddress__);
    }

    bool operator!= (EndPoint &Right_F)
    {
        return (m_nPort__ ^ Right_F.m_nPort__) || (m_uiAddress__ ^ Right_F.m_uiAddress__);
    }

    bool operator== (const EndPoint &Right_F)
    {
        return (m_nPort__ == Right_F.m_nPort__) && (m_uiAddress__ == Right_F.m_uiAddress__);
    }
    bool operator== (EndPoint &Right_F)
    {
        return (m_nPort__ == Right_F.m_nPort__) && (m_uiAddress__ == Right_F.m_uiAddress__);
    }

    bool operator<(const EndPoint &Right_F)
    {
        if (m_uiAddress__ < Right_F.m_uiAddress__) {
            return true;
        }
        else if (m_uiAddress__ == Right_F.m_uiAddress__ && m_nPort__ < Right_F.m_nPort__) {
            return true;
        }

        return false;
    }

	~EndPoint();

	uint16_t port() const;
	uint32_t address() const;
	std::string toString() const;
    static EndPoint str2Addr(std::string AddrIn_F);
private:
	uint16_t m_nPort__;
	uint32_t m_uiAddress__;
};

inline uint16_t EndPoint::port() const
{
    return m_nPort__;
}

inline uint32_t EndPoint::address() const
{
    return m_uiAddress__;
}

} // namespace net

#endif /* _ENDPOINT_HPP_ */
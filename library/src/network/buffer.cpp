#include <cstring>
#include <cassert>

#include "buffer.hpp"

namespace dsm {

Buffer::Buffer() :
    m_pcData__(new char[sc_uiMaxLen]),
    m_uiCap__(sc_uiMaxLen)
{
	memset(m_pcData__, 0, m_uiCap__);
	clear();
}

Buffer::~Buffer()
{
	delete [] m_pcData__;
}

uint32_t Buffer::size() const
{
	return m_uiSize__;
}

bool Buffer::empty() const
{
	return !m_uiSize__;
}

char *Buffer::data() const
{

	m_pcData__[m_uiEnd__] = '\0';
	return m_pcData__ + m_uiBeg__;
}

char *Buffer::begin() const
{
	return m_pcData__ + m_uiBeg__;
}

char *Buffer::end() const
{
	return m_pcData__ + m_uiEnd__;
}

uint32_t Buffer::space() const
{
	return m_uiCap__ - m_uiEnd__;
}

void Buffer::clear()
{
	m_uiBeg__  = 0;
	m_uiEnd__  = 0;
	m_uiSize__ = 0;
}

void Buffer::reset()
{
	if (empty()) {
		assert(m_uiBeg__ == m_uiEnd__);
        clear();
	}
}

void Buffer::adjust()
{
	memmove_s(m_pcData__, m_uiCap__, begin(), m_uiSize__);
	m_uiBeg__ = 0;
	m_uiEnd__ = m_uiSize__ /* + m_uiBeg__ */;
}

void Buffer::advanceHead(uint32_t DataLen_F)
{
	assert(m_uiSize__ >= DataLen_F);

	m_uiBeg__  += DataLen_F;
	m_uiSize__ -= DataLen_F;
}

void Buffer::advanceTail(uint32_t DataLen_F)
{
	assert(m_uiEnd__ + DataLen_F <= m_uiCap__);
	m_uiEnd__  += DataLen_F;
	m_uiSize__ += DataLen_F;
}

void Buffer::unGet(uint32_t DataLen_F)
{
	assert(m_uiBeg__ >= DataLen_F);
	m_uiBeg__  -= DataLen_F;
	m_uiSize__ += DataLen_F;
}

void Buffer::resize(uint32_t NewSize_F)
{
    uint32_t uiNewCap = m_uiCap__;
    while (NewSize_F > uiNewCap) {
        uiNewCap <<= 1;//À©ÈÝÁ½±¶
    }
    char* buf = new char[uiNewCap];
    memcpy_s(buf, uiNewCap, begin(), m_uiSize__);
    delete[] m_pcData__;
    m_pcData__ = buf;
    m_uiBeg__ = 0;
    m_uiEnd__ = m_uiSize__;
    m_uiCap__ = uiNewCap;
}

void Buffer::read(const char *Data_F, uint32_t DataLen_F)
{
	if (m_uiEnd__ + DataLen_F > m_uiCap__) {    //À©ÈÝ
        resize(m_uiEnd__ + DataLen_F);
	}

	assert(m_uiEnd__ + DataLen_F <= m_uiCap__);
	memcpy_s(end(), space(), Data_F, DataLen_F);
	m_uiEnd__  += DataLen_F;
	m_uiSize__ += DataLen_F;
}

void Buffer::write(char *Data_F, uint32_t DataLen_F)
{
	assert(m_uiSize__ >= DataLen_F);
	memcpy_s(Data_F, DataLen_F, begin(), DataLen_F);
	m_uiBeg__  += DataLen_F;
	m_uiSize__ -= DataLen_F;
}

} // namespace net

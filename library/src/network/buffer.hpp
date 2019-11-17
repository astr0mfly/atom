#ifndef _BUFFER_HPP_
#define _BUFFER_HPP_

#include "..\..\rsc\common\utility.h"

namespace net {

class Buffer : private NoCopy
{
public:
	Buffer();
	~Buffer();

	uint32_t size() const;
	bool empty() const;
	char *data() const;//获取数据的头指针，尾部添加\0
	char *begin() const;//获取头指针
	char *end() const;//获取尾部指针
	uint32_t space() const;//剩余空间
	void read(const char *Data_F, uint32_t DataLen_F);
	void write(char *Data_F, uint32_t DataLen_F);
	void advanceHead(uint32_t DataLen_F);//头标记向后偏移,pop
	void advanceTail(uint32_t DataLen_F);//尾标记向后偏移,push
	void reset();
	void clear();
	void adjust();//把数据往前移
	void unGet(uint32_t DataLen_F);//头标记前移，取消pop
    void resize(uint32_t NewSize_F);

private:
    static const uint32_t sc_uiMaxLen = 4096;

	char     *m_pcData__;//缓冲区头部指针
	uint32_t  m_uiSize__;//缓冲区数据大小
	uint32_t  m_uiCap__;//缓冲区容量
	uint32_t  m_uiBeg__;//begin 序号
	uint32_t  m_uiEnd__;//end 序号
};

} // namespace net

#endif /* _BUFFER_HPP_ */
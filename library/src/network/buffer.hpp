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
	char *data() const;//��ȡ���ݵ�ͷָ�룬β�����\0
	char *begin() const;//��ȡͷָ��
	char *end() const;//��ȡβ��ָ��
	uint32_t space() const;//ʣ��ռ�
	void read(const char *Data_F, uint32_t DataLen_F);
	void write(char *Data_F, uint32_t DataLen_F);
	void advanceHead(uint32_t DataLen_F);//ͷ������ƫ��,pop
	void advanceTail(uint32_t DataLen_F);//β������ƫ��,push
	void reset();
	void clear();
	void adjust();//��������ǰ��
	void unGet(uint32_t DataLen_F);//ͷ���ǰ�ƣ�ȡ��pop
    void resize(uint32_t NewSize_F);

private:
    static const uint32_t sc_uiMaxLen = 4096;

	char     *m_pcData__;//������ͷ��ָ��
	uint32_t  m_uiSize__;//���������ݴ�С
	uint32_t  m_uiCap__;//����������
	uint32_t  m_uiBeg__;//begin ���
	uint32_t  m_uiEnd__;//end ���
};

} // namespace net

#endif /* _BUFFER_HPP_ */
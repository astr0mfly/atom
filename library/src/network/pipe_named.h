#ifndef PIPE_NAMED_H_
#define PIPE_NAMED_H_
#include <iostream>

#include <WinSock2.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include "simTools.h"

#define PIPE_DEFAULT_BUF_SIZE 1024
#define PIPE_DEFAULT_TIME_OUT 1000

/**
*@brief     : _PIPE_MODE_E
*@detail    : ��ǰ�˶���� K-V ������
*@note      : 
            PIPE_ACCESS_DUPLEX �ܵ���˫���
            PIPE_ACCESS_INBOUND ���ݴӿͻ���������������
            PIPE_ACCESS_OUTBOUND ���ݴӷ������������ͻ���
*/
typedef enum _PIPE_MODE_E
{
    E_PIPE_CLIENT,
    E_PIPE_SERVER,

    E_PIPE_BUTTOM
}PIPE_MODE_E;

/**
*@brief     : _PIPE_SRV_SETTING_S
*@detail    : pipe �������������
*/
typedef struct _PIPE_SRV_SETTING_S
{
    DWORD                   dwOpenMode;             //���ݷ���
    DWORD                   dwPipeMode;             //���ݴ�������
    DWORD                   nMaxInstances;          //���������
    DWORD                   nOutBufferSize;         //���buffer�Ĵ�С
    DWORD                   nInBufferSize;          //����buffer�Ĵ�С
    DWORD                   nDefaultTimeOut;        //Ĭ�ϳ�ʱʱ��
    LPSECURITY_ATTRIBUTES   lpSecurityAttributes;   //��ȫ����
    _PIPE_SRV_SETTING_S():
        dwOpenMode(PIPE_ACCESS_DUPLEX),
        dwPipeMode(PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT),
        nMaxInstances(PIPE_UNLIMITED_INSTANCES),
        nOutBufferSize(PIPE_DEFAULT_BUF_SIZE),
        nInBufferSize(PIPE_DEFAULT_BUF_SIZE),
        nDefaultTimeOut(PIPE_DEFAULT_TIME_OUT),
        lpSecurityAttributes(NULL)
    { }
}PIPE_SRV_SETTING_S, *PPIPE_SRV_SETTING_S;

/**
*@brief     : PipeNamed
*@detail    : �����ܵ�
*/
class PipeNamed
{
public:
    PipeNamed(PIPE_MODE_E, const char *);
    ~PipeNamed();

    void regRspProc(std::function<void()> funcRspForReq);
    int sendPipeMsg(const char* pcBuff, UINT uiBufLen);
    int sendPipeMsg(const char* pcBuff, UINT uiBufLen, VOID* pvbuff, UINT buffLen);
    int getPipeMsgRsp(VOID* pvbuff, UINT buffLen);

    void readBuffer(char **ppcBuff, UINT *puiBufLen);
    void writeBuffer(const char* pcBuff, UINT uiBufLen);

private:
    VOID __responseForReq();
    int __createServer();
    inline VOID __closePipe();
    static DWORD WINAPI __WorkerThread(LPVOID);

    INT __createClient();
    CHAR                    m_strName[32];  //�ܵ�����
    PIPE_SRV_SETTING_S      m_stSrvSet;     //pipe ����
    HANDLE                  m_hPipe;

    CHAR                    m_Recvbuffer[PIPE_DEFAULT_BUF_SIZE];
    ULONG                   m_ulRecvBytes;
    CHAR                    m_Sendbuffer[PIPE_DEFAULT_BUF_SIZE];
    ULONG                   m_ulSendBytes;


    std::thread             m_hMaster;//���̶߳���
    HANDLE                  m_hWorker;//�����߳̾��

    std::function<void()>   m_funcRspForReq;//��Ϣ����ĺ���ָ��

    std::atomic<BOOL>       atmRunState;//�����߳��˳���flag
    std::atomic<BOOL>       atmRspArrive;//�����Ƿ񵽴�ı�־
};

#endif //PIPE_NAMED_H_